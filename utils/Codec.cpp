#include "Codec.h"

Codec::Codec(/* args */)
{
    m_pAvFormatCtx = nullptr;
    m_pVidCodecCtx = nullptr;
    m_vidStreamIndex = -1;
    m_pAudCodecCtx = nullptr;
    m_audStreamIndex = -1;
}

Codec::~Codec()
{
}

Codec *Codec::getInstance()
{
    static Codec codec;
    return &codec;
}

int32_t Codec::openFile(const char *path){

    const AVCodec *pVidCodec; // 视频编解码器
    const AVCodec *pAudCodec; // 音频编解码器
    int res = 0;

    res = avformat_open_input(&m_pAvFormatCtx, path, nullptr, nullptr); // 打开文件
    if (m_pAvFormatCtx == nullptr)
    {
        qWarning() << "打开文件失败";
        return -1;
    }
    res = avformat_find_stream_info(m_pAvFormatCtx, nullptr); // 查找流信息
    if (res == AVERROR_EOF)
    {
        qWarning() << "查找流信息失败";
        closeFile();
        return -1;
    }

    // 查找视频流
    for (uint32_t i = 0; i < m_pAvFormatCtx->nb_streams; i++)
    {
        AVStream *pAvStream = m_pAvFormatCtx->streams[i];
        if (pAvStream->codecpar->codec_type == AVMEDIA_TYPE_VIDEO)
        {
            // 检查视频流的宽高
            if (pAvStream->codecpar->width <= 0 || pAvStream->codecpar->height <= 0)
            {
                qWarning() << "视频流"<<i<<"宽高错误";
                continue;
            }
            
            // 查找视频解码器
            pVidCodec = avcodec_find_decoder(pAvStream->codecpar->codec_id);
            if (pVidCodec == nullptr)
            {
                qWarning() << "找不到适用于"<<pAvStream->codecpar->codec_id<<"的视频解码器";
                closeFile();
                continue;
            }

            m_vidStreamIndex = i;
            qInfo() <<"format:"<< pAvStream->codecpar->format << "width:"<< pAvStream->codecpar->width << "height:"<< pAvStream->codecpar->height;
        } else if (pAvStream->codecpar->codec_type == AVMEDIA_TYPE_AUDIO)
        {
            // 检查音频流的采样率和声道数
            if (pAvStream->codecpar->sample_rate <= 0 || pAvStream->codecpar->ch_layout.nb_channels <= 0)
            {
                qWarning() << "音频流"<<i<<"采样率错误";
                continue;
            }

            // 查找音频解码器
            pAudCodec = avcodec_find_decoder(pAvStream->codecpar->codec_id);
            if (pAudCodec == nullptr)
            {
                qWarning() << "找不到适用于"<<pAvStream->codecpar->codec_id<<"的音频解码器";
                continue;
            }
            
            m_audStreamIndex = i;
            qInfo() <<"sample_rate:"<< pAvStream->codecpar->sample_rate << "channels:"<< pAvStream->codecpar->ch_layout.nb_channels;
        }
        
    }

    if (pVidCodec == nullptr)
    {
        qWarning() << "找不到视频流";
        closeFile();
        return -1;
    }
    if (pAudCodec == nullptr)
    {
        qWarning() << "找不到音频流";
        closeFile();
        return -1;
    }

    // 打开视频解码器
    if (pVidCodec != nullptr)
    {
        // 分配视频解码器上下文
        m_pVidCodecCtx = avcodec_alloc_context3(pVidCodec);
        if (m_pVidCodecCtx == nullptr)
        {
            qWarning() << "分配视频解码器上下文失败";
            closeFile();
            return -1;
        }
        // 设置视频解码器上下文参数
        res = avcodec_parameters_to_context(m_pVidCodecCtx, m_pAvFormatCtx->streams[m_vidStreamIndex]->codecpar);
        if (res < 0)
        {
            qWarning() << "设置视频解码器上下文参数失败";
            closeFile();
            return -1;
        }
        // 打开视频解码器
        res = avcodec_open2(m_pVidCodecCtx, pVidCodec, nullptr);
        if (res < 0)
        {
            qWarning() << "打开视频解码器失败";
            closeFile();
            return -1;
        }
    }
    if (pAudCodec)
    {
        // 分配音频解码器上下文
        m_pAudCodecCtx = avcodec_alloc_context3(pAudCodec);
        if (m_pAudCodecCtx == nullptr)
        {
            qWarning() << "分配音频解码器上下文失败";
            closeFile();
            return -1;
        }
        // 设置音频解码器上下文参数
        res = avcodec_parameters_to_context(m_pAudCodecCtx, m_pAvFormatCtx->streams[m_audStreamIndex]->codecpar);
        if (res < 0)
        {
            qWarning() << "设置音频解码器上下文参数失败";
            closeFile();
            return -1;
        }
        // 打开音频解码器
        res = avcodec_open2(m_pAudCodecCtx, pAudCodec, nullptr);
        if (res < 0)
        {
            qWarning() << "打开音频解码器失败";
            closeFile();
            return -1;
        }
    }
    
    return 0;
}

void Codec::closeFile(){
    if (m_pAvFormatCtx != nullptr)
    {
        avformat_close_input(&m_pAvFormatCtx);
        m_pAvFormatCtx = nullptr;
    }
    if (m_pVidCodecCtx != nullptr)
    {
        avcodec_close(m_pVidCodecCtx);
        avcodec_free_context(&m_pVidCodecCtx);
        m_pVidCodecCtx = nullptr;
    }
    if (m_pAudCodecCtx != nullptr)
    {
        avcodec_close(m_pAudCodecCtx);
        avcodec_free_context(&m_pAudCodecCtx);
        m_pAudCodecCtx = nullptr;
    }
    
}