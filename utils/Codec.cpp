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
    closeFile();
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

int32_t Codec::readFrame(){
    int res = 0;


    AVPacket *pPacket = av_packet_alloc();

    // 读取数据包
    res = av_read_frame(m_pAvFormatCtx, pPacket);
    if (res == AVERROR_EOF)
    {
        qWarning() << "读取到文件结尾";
        return -1;
    } else if (res < 0)
    {
        qWarning() << "读取数据包失败";
        return res;
    }
    if (pPacket->stream_index == m_vidStreamIndex)
    {
        AVFrame *pVideoFrame = nullptr;
        res = decodePacketToFrame(m_pVidCodecCtx, pPacket, &pVideoFrame);
        if (res == 0 && pVideoFrame != nullptr)
        {
            m_vidBuffer.push(pVideoFrame);
            return 0;
        }
    } else if (pPacket->stream_index == m_audStreamIndex)
    {
        AVFrame *pAudioFrame = nullptr;
        res = decodePacketToFrame(m_pAudCodecCtx, pPacket, &pAudioFrame);
        if (res == 0 && pAudioFrame != nullptr)
        {
            m_aioBuffer.push(pAudioFrame);
            return 0;
        }
        
    }
    
    return res;
}

int32_t Codec::decodePacketToFrame(AVCodecContext *pCodecCtx, const AVPacket *pPacket, AVFrame **ppFrame)
{
    AVFrame* pOutFrame = av_frame_alloc();
    int res = 0;

    res = avcodec_send_packet(pCodecCtx, pPacket);
    if (res == AVERROR(EAGAIN))
    {
        qDebug() << "缓冲区已满";
    }
    else if (res == AVERROR_EOF) // 数据包送入结束不再送入,但是可以继续可以从内部缓冲区读取解码后的音视频帧
    {
        qDebug() << "数据包送入结束不再送入,但是可以继续可以从内部缓冲区读取解码后的音视频帧";
    }
    else if (res < 0)  // 送入输入数据包失败
    {
        qDebug() << "送入数据包失败" << res;
        return res;
    }

    res = avcodec_receive_frame(pCodecCtx,pOutFrame);
    if (res == AVERROR(EAGAIN))
    {
        qDebug() << "需要更多数据包";
        av_frame_free(&pOutFrame);
        *ppFrame = nullptr;
        return 0;
    } else if (res == AVERROR_EOF)
    {
        qDebug() << "解包完毕后续不会再有包了";
        av_frame_free(&pOutFrame);
        return AVERROR_EOF;
    }   else if (res < 0)
    {
        qDebug() << "解包失败" << res;
        av_frame_free(&pOutFrame);
        *ppFrame = nullptr;
        return res;
    }
    
    *ppFrame = pOutFrame;

    return 0;
}

int32_t Codec::videoConvert(const AVFrame *pInFrame, /*AVPixelFormat eOutFormat, */int32_t out_width, int32_t out_height, uint8_t* data[1],int linesize[1])
{
    qDebug() << "pInFrame -> width:" << pInFrame->width
         << "height:" << pInFrame->height
         << "format:" << pInFrame->format;
    if (pInFrame == nullptr) {
        qWarning() << "pInFrame is null";
        return -1;
    }

    // 确保输出缓冲区已分配并足够大，这里采用 RGB32，每个像素4字节
    // 如果输出内存未分配，则需先分配内存
    // 示例：假如你未在调用前准备内存，则可在此分配临时内存
    int requiredSize = out_width * out_height * 4;
    if (data[0] == nullptr) {
        data[0] = (uint8_t*)av_malloc(requiredSize);
        if (data[0] == nullptr) {
            qWarning() << "无法分配内存";
            return -1;
        }
        linesize[0] = out_width * 4;
    }

    SwsContext *pSwsCtx = nullptr;

    pSwsCtx = sws_getContext(pInFrame->width,pInFrame->height,(AVPixelFormat)pInFrame->format, out_width, out_height, AV_PIX_FMT_RGB32, SWS_BICUBIC,nullptr,nullptr,nullptr);
    if (pSwsCtx == nullptr)
    {
        qWarning() << "创建格式转换器失败";
        return -1;
    }
    
    sws_scale(pSwsCtx, pInFrame->data, pInFrame->linesize, 0, out_height, data, linesize);

    sws_freeContext(pSwsCtx);

    return 0;
}

AVFrame *Codec::getVidFrame()
{
    return m_vidBuffer.front();
}

void Codec::popVidFrame()
{
    av_frame_free(&m_vidBuffer.front());
    m_vidBuffer.pop();
}

int32_t Codec::getVidBufferCount()
{
    return m_vidBuffer.size();
}
