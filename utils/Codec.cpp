#include "Codec.h"

Codec::Codec(/* args */)
{
    m_pAvFormatCtx = nullptr;
    m_pVidCodecCtx = nullptr;
    m_vidStreamIndex = -1;
    m_pAudCodecCtx = nullptr;
    m_audStreamIndex = -1;
    m_isDecoding = false;
    m_isEnd = false;
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

    // 查找流信息，不传入选项参数
    //if(avformat_find_stream_info(m_pAvFormatCtx, nullptr) < 0) {
    //    qDebug() << "无法获取流信息";
    //    return -1;
    //}
    
    // 打印媒体文件的详细信息
    av_dump_format(m_pAvFormatCtx, 0, NULL, 0);

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

    while (!m_audBuffer.empty())
    {
        av_frame_free(&(m_audBuffer.front()));
        m_audBuffer.pop();
    }
    while (!m_vidBuffer.empty())
    {
        av_frame_free(&(m_vidBuffer.front()));
        m_vidBuffer.pop();
    }
    while (!m_audPacketQueue.empty())
    {
        av_packet_free(&(m_audPacketQueue.front()));
        m_audPacketQueue.pop();
    }
    while (!m_vidPacketQueue.empty())
    {
        av_packet_free(&(m_vidPacketQueue.front()));
        m_vidPacketQueue.pop();
    }
    
}

int32_t Codec::readPacket(){
    int res = 0;


    AVPacket *pPacket = av_packet_alloc();

    // 读取数据包
    res = av_read_frame(m_pAvFormatCtx, pPacket);
    if (res == AVERROR_EOF)
    {
        qWarning() << "读取到文件结尾";
        return res;
    } else if (res < 0)
    {
        qWarning() << "读取数据包失败";
        return res;
    }
    if (pPacket->stream_index == m_vidStreamIndex)
    {
        m_vidPacketQueue.push(pPacket);
    } 
    else if (pPacket->stream_index == m_audStreamIndex)
    {
        m_audPacketQueue.push(pPacket);        
    }
    
    return res;
}

int32_t Codec::packetDecoder(std::queue<AVPacket *> &packetQueue)
{
    if (packetQueue.empty())
    {
        return -1;
    }
    
    AVPacket *pPacket = packetQueue.front();
    AVFrame *pOutFrame = nullptr;
    int res = 0;

    if (pPacket->stream_index == m_vidStreamIndex)
    {
        res = decodePacketToFrame(m_pVidCodecCtx, pPacket, &pOutFrame);
        if (res || pOutFrame==nullptr){
            av_packet_free(&pPacket);
            packetQueue.pop();
            return res;
        }

        // 将pts转换为秒数，假设m_vidTimeBase为视频time_base
        double videoPts = pOutFrame->pts * av_q2d(m_pAvFormatCtx->streams[m_vidStreamIndex]->time_base);
        // 计算与音频主时钟的差值
        double delay = videoPts - m_audioClock;

        if (delay > 0)
        {
            // 等待delay对应的微秒数，此处使用av_usleep（注意单位为微秒）
            av_usleep(static_cast<unsigned int>(delay * 1000000));
        }
        else if (delay < -0.05) // 可设置阈值，根据实际情况丢弃过期帧
        {
            av_frame_free(&pOutFrame);
            av_packet_free(&pPacket);
            packetQueue.pop();
            return 0;
        }

        m_vidBuffer.push(pOutFrame);
        av_packet_free(&pPacket);
        packetQueue.pop();
        return 0;

    } else if (pPacket->stream_index == m_audStreamIndex)
    {
        res = decodePacketToFrame(m_pAudCodecCtx, pPacket, &pOutFrame);
        if (pOutFrame == nullptr){
            av_packet_free(&pPacket);
            packetQueue.pop();
            return res;
        }

        double audioPts = pPacket->pts * av_q2d(m_pAvFormatCtx->streams[m_audStreamIndex]->time_base);
        // 计算等待时间，若 audioPts 在当前 m_audioClock 之后，则等待
        double delay = audioPts - m_audioClock;
        if (delay > 0)
        {
            av_usleep(static_cast<unsigned int>(delay * 1000000)); // 单位微秒
        }
        // 更新 m_audioClock（单位：秒）
        m_audioClock = audioPts;

        m_audBuffer.push(pOutFrame);
        av_packet_free(&pPacket);
        packetQueue.pop();
        return 0;

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
        return res;
    } else if (res == AVERROR_EOF)
    {
        qDebug() << "解包完毕后续不会再有包了";
        av_frame_free(&pOutFrame);
        return res;
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

int32_t Codec::videoFrameConvert(const AVFrame *pInFrame, OutVideoFrameSetting settings , uint8_t* data[1],int linesize[1])
{
    if (pInFrame == nullptr) {
        qWarning() << "pInFrame is null";
        return -1;
    }

    // 确保输出缓冲区已分配并足够大，这里采用 RGB32，每个像素4字节
    // 如果输出内存未分配，则需先分配内存
    // 示例：假如你未在调用前准备内存，则可在此分配临时内存
    int requiredSize = settings.width * settings.height * 4;
    if (data[0] == nullptr) {
        data[0] = (uint8_t*)av_malloc(requiredSize);
        if (data[0] == nullptr) {
            qWarning() << "无法分配内存";
            return -1;
        }
        linesize[0] = settings.width * 4;
    }

    SwsContext *pSwsCtx = nullptr;

    pSwsCtx = sws_getContext(pInFrame->width,pInFrame->height,(AVPixelFormat)pInFrame->format, settings.width, settings.height, AV_PIX_FMT_RGB32, SWS_BICUBIC,nullptr,nullptr,nullptr);
    if (pSwsCtx == nullptr)
    {
        qWarning() << "创建格式转换器失败";
        return -1;
    }
    
    sws_scale(pSwsCtx, pInFrame->data, pInFrame->linesize, 0, settings.height, data, linesize);

    sws_freeContext(pSwsCtx);

    return 0;
}

int32_t Codec::audioFrameConvert(const AVFrame *pInFrame, OutAudioFrameSetting settings, uint8_t *data[1], int linesize[1])
{
    int res = 0;
    if (pInFrame == nullptr) {
        qWarning() << "pInFrame is null";
        return -1;
    }
    
    SwrContext *pSwrCtx = swr_alloc();
    if (!pSwrCtx) {
        qWarning() << "分配重采样上下文失败";
        return -1;
    }

    // 获取声道布局
    AVChannelLayout ch_ly;
    av_channel_layout_default(&ch_ly, settings.channel_count);
    
    
    // 获取采样位深
    AVSampleFormat fmt;
    switch (settings.sample_fmt)
    {
    case 8:
        fmt = AV_SAMPLE_FMT_U8;
        break;
    case 16:
        fmt = AV_SAMPLE_FMT_S16;
        break;
    case 24:
        fmt = AV_SAMPLE_FMT_S32;
        break;
    case 32:
        fmt = AV_SAMPLE_FMT_FLT;
    case 63:
        fmt = AV_SAMPLE_FMT_DBL;
    default:
        fmt = AV_SAMPLE_FMT_NONE; // 不支持的采样位数
    }

    res = swr_alloc_set_opts2(&pSwrCtx,
                            &ch_ly,
                            fmt,
                            settings.sample_rate,
                            &(pInFrame->ch_layout),
                            AVSampleFormat(pInFrame->format),
                            pInFrame->sample_rate,0,nullptr);
    if(!res) return res;

    swr_init(pSwrCtx);
    if(!res) return res;

    // 计算重采样后的采样点数
    int in_samples = pInFrame->nb_samples;
    int64_t delay = swr_get_delay(pSwrCtx, m_pAudCodecCtx->sample_rate);
    int out_samples = av_rescale_rnd(delay + in_samples,
                                    settings.sample_rate,
                                    m_pAudCodecCtx->sample_rate,
                                    AV_ROUND_UP);
    av_samples_get_buffer_size(linesize,settings.channel_count,out_samples,fmt,0);
    if(linesize == nullptr) return -1;

    data[0] = new uint8_t[linesize[0]];
    res = swr_convert(pSwrCtx,data,out_samples,pInFrame->data,pInFrame->nb_samples);
    if(!res) return -1;

    return 0;
}

void Codec::threadReadPacket()
{
    while (m_isDecoding)
    {
        if (m_audPacketQueue.empty() || m_vidPacketQueue.empty())
        {
            if (readPacket()==-1 && m_audPacketQueue.empty() && m_vidPacketQueue.empty()){
                
                m_isDecoding == false;
                m_isEnd == true;
            }
        }
    }
    
}

void Codec::threadDecodeVideo()
{
    while (m_isDecoding)
    {
        packetDecoder(m_vidPacketQueue);
    }
    
}

void Codec::threadDecodeAudio()
{
    while (m_isDecoding)
    {
        packetDecoder(m_audPacketQueue);
    }
    
}

void Codec::startDecoding()
{
    m_isDecoding = true;
    m_isEnd = false;
    static std::thread tRead(&Codec::threadReadPacket,this);
    static std::thread tVideo(&Codec::threadDecodeVideo,this);
    static std::thread tAudio(&Codec::threadDecodeAudio,this);
    
    if (tRead.joinable())
    {
        tRead.detach();
    }
    if (tVideo.joinable())
    {
        tVideo.detach();
    }
    if (tAudio.joinable())
    {
        tAudio.detach();
    }
    
}

void Codec::stopDecoding(){
    m_isDecoding = false;
}

bool Codec::isEnd()
{
    return m_isEnd;
}

