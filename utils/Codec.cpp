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
    overload = false;

    
    qDebug() << av_version_info();
}

Codec::~Codec()
{
    closeFile();
}

int32_t Codec::openFile(const char *path){
    std::lock_guard<std::mutex> lock1(m_audPacketQueueMutex);
    std::lock_guard<std::mutex> lock2(m_vidPacketQueueMutex);
    m_sync.InitClock();
    const AVCodec *pVidCodec = nullptr; // 视频编解码器
    const AVCodec *pAudCodec = nullptr; // 音频编解码器
    int res = 0;
    m_picStreamIndex = -1;
    m_audStreamIndex = -1;
    m_vidStreamIndex = -1;
    m_picHeight = 0;
    m_picWidth = 0;
    m_outAudSetting.sample=0;
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
        return -1;
    }

    // 查找流信息，不传入选项参数
    //if(avformat_find_stream_info(m_pAvFormatCtx, nullptr) < 0) {
    //    qDebug() << "无法获取流信息";
    //    return -1;
    //}
    
    // 打印媒体文件的详细信息
    av_dump_format(m_pAvFormatCtx, 0, NULL, 0);
    m_mediaType = MediaType::audio;
    // 查找视频流
    for (uint32_t i = 0; i < m_pAvFormatCtx->nb_streams; i++)
    {
        AVStream *pAvStream = m_pAvFormatCtx->streams[i];
        if (pAvStream->codecpar->codec_type == AVMEDIA_TYPE_VIDEO)
        {
            if (pAvStream->disposition & AV_DISPOSITION_ATTACHED_PIC) {
                m_picStreamIndex = i;
                m_picHeight = pAvStream->codecpar->height;
                m_picWidth = pAvStream->codecpar->width;
                continue;
            }
            m_mediaType = MediaType::video;
            
            if (m_picStreamIndex < 0){
                m_picHeight = pAvStream->codecpar->height;
                m_picWidth = pAvStream->codecpar->width;
            }           
            // 检查视频流的宽高
            if (pAvStream->codecpar->width <= 0 || pAvStream->codecpar->height <= 0)
            {
                qWarning() << "视频流"<<i<<"宽高错误";
                continue;
            }
            
            // 初始化解码器
            changeVideoStream(i);
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

    if (pVidCodec == nullptr && pAudCodec == nullptr)
    {
        qWarning() << "找不到有效的流";
        return -1;
    }

    if (pAudCodec != nullptr)
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
            return -1;
        }
        // 打开音频解码器
        res = avcodec_open2(m_pAudCodecCtx, pAudCodec, nullptr);
        if (res < 0)
        {
            qWarning() << "打开音频解码器失败";
            return -1;
        }
    }
    
    return 0;
}

void Codec::closeFile(){
    std::lock_guard<std::mutex> lock(m_audPacketQueueMutex);
    std::lock_guard<std::mutex> lock2(m_vidPacketQueueMutex);
    std::lock_guard<std::mutex> lock3(m_avFormatCtxMutex);
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

int32_t Codec::changeVideoStream(int32_t i)
{
    AVStream *pAvStream = m_pAvFormatCtx->streams[i];
    const AVCodec *pVidCodec = avcodec_find_decoder(pAvStream->codecpar->codec_id);
    if (pVidCodec == nullptr)
    {
        qWarning() << "找不到适用于"<<pAvStream->codecpar->codec_id<<"的视频解码器";
        return -1;
    }
    // 分配视频解码器上下文
    AVCodecContext* pVidCodecCtx = avcodec_alloc_context3(pVidCodec);
    if (pVidCodecCtx == nullptr)
    {
        qWarning() << "分配视频解码器上下文失败";
        return -1;
    }
    // 设置视频解码器上下文参数
    int res = avcodec_parameters_to_context(pVidCodecCtx, pAvStream->codecpar);
    if (res < 0)
    {
        qWarning() << "设置视频解码器上下文参数失败";
        avcodec_free_context(&pVidCodecCtx);
        return -1;
    }
    // 打开视频解码器
    res = avcodec_open2(pVidCodecCtx, pVidCodec, nullptr);
    if (res < 0)
    {
        qWarning() << "打开视频解码器失败";
        avcodec_free_context(&pVidCodecCtx);
        return -1;
    }
    m_vidStreamIndex = i;
    m_pVidCodecCtx = pVidCodecCtx;
}

void Codec::changeAudioStream(int32_t i)
{
    // TODO 音频流切换
}

int32_t Codec::readPacket(){
    std::lock_guard<std::mutex> lock(m_avFormatCtxMutex);    
    
    int res = 0;

    AVPacket *pPacket = av_packet_alloc();
    if (!m_pAvFormatCtx)
    {
        return -1;
    }
    
    // 读取数据包
    res = av_read_frame(m_pAvFormatCtx, pPacket);
    if (res == AVERROR_EOF)
    {
        qDebug() << "读取到文件结尾";
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

int32_t Codec::packetDecoder(std::queue<AVPacket *> &packetQueue, int stream_index)
{

    
    if (packetQueue.empty())
    {
        return -1;
    }
    
    AVPacket *pPacket = packetQueue.front();
    AVFrame *pOutFrame = nullptr;
    int res = 0;
    
    if (stream_index == m_vidStreamIndex) // 视频
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
        double delay = videoPts - m_sync.getClock();
        if (delay < -1.0) {
            overload = true;
            qDebug()<<"nonono"<<delay;
            av_frame_free(&pOutFrame);
            av_packet_free(&pPacket);
            packetQueue.pop();
            while (!packetQueue.empty())
            {
                pPacket = packetQueue.front();
                if (pPacket->flags & AV_PKT_FLAG_KEY) {
                    break;
                }
                av_packet_free(&packetQueue.front());
                packetQueue.pop();
            }
            
            return -1;
        }
        if (delay > 1.0)
        {
            if (delay >1.0) qDebug()<<"fix!!!!";
            qDebug()<<"nonono"<<delay;
            av_frame_free(&pOutFrame);
            if (pPacket) av_packet_free(&pPacket);
            packetQueue.pop();
            return 0;
        } 
        else if (delay > 0.0) // 可设置阈值，根据实际情况丢弃过期帧
        {
            // 等待delay对应的微秒数，此处使用av_usleep（注意单位为微秒
            // qDebug()<<"666:"<<delay;
            av_usleep(static_cast<unsigned int>(delay * 1000000));
        }

        m_vidBuffer.push(pOutFrame);
        if (pPacket) av_packet_free(&pPacket);
        packetQueue.pop();
        overload = false;
        return 0;

    } else if (stream_index == m_audStreamIndex) //音频
    {
        if (m_audBuffer.size()>2){
            return 0;
        }
        res = decodePacketToFrame(m_pAudCodecCtx, pPacket, &pOutFrame);
        if (pOutFrame == nullptr){
            if (pPacket) av_packet_free(&pPacket);
            packetQueue.pop();
            return res;
        }
        // int64_t delay = swr_get_delay(pSwrCtx, m_pAvFormatCtx->streams[m_audStreamIndex]->codecpar->sample_rate);
        // int out_samples = av_rescale_rnd(delay + pOutFrame->nb_samples,
        //                                 m_outAudSetting.sample_rate,
        //                                 m_pAvFormatCtx->streams[m_audStreamIndex]->codecpar->sample_rate,
        //                                 AV_ROUND_UP);
        // m_outAudSetting.sample = out_samples;
        m_audBuffer.push(pOutFrame);
        if (pPacket) av_packet_free(&pPacket);
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

int32_t Codec::videoFrameConvert(const AVFrame *pInFrame, OutVideoFrameSetting &settings , uint8_t* data[1],int linesize[1])
{
    // 计算缩放比例（等比例缩放，使得图像能完整显示在目标画布内）
    int width = settings.width;
    int height = settings.height;
    if (settings.width <= 0 ||settings.height <= 0){
        width = pInFrame->width;
        height = pInFrame->height;
    }
    double ratioWidth = (double)width / pInFrame->width;
    double ratioHeight = (double)height / pInFrame->height;
    double ratio = std::min(ratioWidth, ratioHeight);

    int newWidth = static_cast<int>(pInFrame->width * ratio);
    int newHeight = static_cast<int>(pInFrame->height * ratio);
    int xOffset = (width - newWidth) / 2; 
    int yOffset = (height - newHeight) / 2;

    // 确保输出缓冲区已分配并足够大，这里采用 RGB32，每个像素4字节
    // 如果输出内存未分配，则需先分配内存
    // 示例：假如你未在调用前准备内存，则可在此分配临时内存
    const AVPixFmtDescriptor* desc = av_pix_fmt_desc_get(settings.pixelFormat);
    if (!desc) {
        qWarning() << "无法获取像素格式描述";
        return -1;
    }
    int bytesPerPixel = av_get_bits_per_pixel(desc) / 8;

    
    // 填充目标画布为黑色背景
    for (int y = 0; y < height; y++) {
        memset(data[0] + y * linesize[0], 0, width * bytesPerPixel);
    }

    // 为缩放后的图像分配临时缓冲区
    uint8_t *tempData[4] = { nullptr };
    int tempLinesize[4] = { 0 };
    int ret = av_image_alloc(tempData, tempLinesize, newWidth, newHeight, settings.pixelFormat, 1);
    if (ret < 0) {
        return ret;
    }

    // 创建缩放上下文
    SwsContext *swsCtx = sws_getContext(
        pInFrame->width, pInFrame->height, (AVPixelFormat)pInFrame->format,
        newWidth, newHeight, settings.pixelFormat,
        SWS_BILINEAR, nullptr, nullptr, nullptr);
    if (!swsCtx) {
        av_freep(&tempData[0]);
        return -1;
    }

    // 执行缩放转换
    int scaledH = sws_scale(
        swsCtx,
        pInFrame->data, pInFrame->linesize,
        0, pInFrame->height,
        tempData, tempLinesize);
    sws_freeContext(swsCtx);
    if (scaledH != newHeight) {
        av_freep(&tempData[0]);
        return -1;
    }

    // 将缩放后的图像拷贝到目标画布居中位置
    for (int y = 0; y < newHeight; y++) {
        memcpy(
            data[0] + ((y + yOffset) * linesize[0] + xOffset * bytesPerPixel),
            tempData[0] + y * tempLinesize[0],
            newWidth * bytesPerPixel);
    }
    
    av_freep(&tempData[0]);
    return 0;
}

int32_t Codec::initAudioContext()
{
    // 分配 SwrContext
    pSwrCtx = swr_alloc();
    if (!pSwrCtx) {
        qWarning() << "分配重采样上下文失败";
        return -1;
    }

    // 配置输出声道布局
    AVChannelLayout out_ch_layout;
    av_channel_layout_default(&out_ch_layout, m_outAudSetting.channel_count);

    // 根据采样位深选择输出采样格式
    AVSampleFormat out_format = AV_SAMPLE_FMT_NONE;
    switch (m_outAudSetting.sample_fmt) {
        case 8:
            out_format = AV_SAMPLE_FMT_U8;
            break;
        case 16:
            out_format = AV_SAMPLE_FMT_S16;
            break;
        case 24:
            out_format = AV_SAMPLE_FMT_S32;
            break;
        case 32:
            out_format = AV_SAMPLE_FMT_FLT;
            break;
        case 63:
            out_format = AV_SAMPLE_FMT_DBL;
            break;
        default:
            qWarning() << "不支持的采样位深:" << m_outAudSetting.sample_fmt;
            swr_free(&pSwrCtx);
            return -1;
    }

    // 配置重采样参数
    int res = swr_alloc_set_opts2(&pSwrCtx,
                                  &out_ch_layout,
                                  out_format,
                                  m_outAudSetting.sample_rate,
                                  &(m_pAvFormatCtx->streams[m_audStreamIndex]->codecpar->ch_layout),
                                  static_cast<AVSampleFormat>(m_pAvFormatCtx->streams[m_audStreamIndex]->codecpar->format),
                                  m_pAvFormatCtx->streams[m_audStreamIndex]->codecpar->sample_rate,
                                  0,
                                  nullptr);
    qDebug() << "audio sample_rate: " << m_pAvFormatCtx->streams[m_audStreamIndex]->codecpar->sample_rate;
    if (res < 0) {
        qWarning() << "配置重采样上下文失败:" << res;
        swr_free(&pSwrCtx);
        return res;
    }

    // 提高重采样质量
    //swr_set_quality(pSwrCtx, 10); // 设置最高质量

    // 初始化 SwrContext
    res = swr_init(pSwrCtx);
    if (res < 0) {
        qWarning() << "初始化重采样上下文失败:" << res;
        swr_free(&pSwrCtx);
        return res;
    }

    return 0;
}

int32_t Codec::audioFrameConvert(const AVFrame *pInFrame, OutAudioFrameSetting &settings, uint8_t *data[1], int linesize[1])
{
    if (!pInFrame) {
        qWarning() << "pInFrame is null";
        return -1;
    }


    int in_samples = pInFrame->nb_samples;
    // 计算重采样延时并根据采样率比例重算输出采样点数
    if (!pSwrCtx) return -1;
    int64_t delay = swr_get_delay(pSwrCtx, m_pAvFormatCtx->streams[m_audStreamIndex]->codecpar->sample_rate);
    int out_samples = av_rescale_rnd(delay + in_samples,
                                     settings.sample_rate,
                                     m_pAvFormatCtx->streams[m_audStreamIndex]->codecpar->sample_rate,
                                     AV_ROUND_UP);
    AVChannelLayout out_ch_layout;
    av_channel_layout_default(&out_ch_layout, m_outAudSetting.channel_count);
    // 根据采样位深选择输出采样格式
    AVSampleFormat out_format = AV_SAMPLE_FMT_NONE;
    switch (m_outAudSetting.sample_fmt) {
        case 8:
            out_format = AV_SAMPLE_FMT_U8;
            break;
        case 16:
            out_format = AV_SAMPLE_FMT_S16;
            break;
        case 24:
            out_format = AV_SAMPLE_FMT_S32;
            break;
        case 32:
            out_format = AV_SAMPLE_FMT_FLT;
            break;
        case 63:
            out_format = AV_SAMPLE_FMT_DBL;
            break;
        default:
            qWarning() << "不支持的采样位深:" << m_outAudSetting.sample_fmt;
            return -1;
    }
    // 分配输出缓冲区
    int res = av_samples_alloc(data, linesize,
                           out_ch_layout.nb_channels,
                           out_samples,
                           out_format,
                           0);
    if (res < 0) {
        qWarning() << "分配缓冲区失败";
        return -1;
    }

    // 执行重采样转换
    int converted_samples = swr_convert(pSwrCtx,
                                       data,
                                       out_samples,
                                       pInFrame->data,
                                       in_samples);
    m_outAudSetting.sample = converted_samples;
    if (converted_samples < 0) {
        qWarning() << "重采样转换失败:" << converted_samples;
        av_freep(&data[0]);
        return -1;
    }
    //m_outAudSetting.sample = converted_samples;
    // 可选：重新计算输出数据大小
    linesize[0] = av_samples_get_buffer_size(nullptr,
                                             out_ch_layout.nb_channels,
                                             converted_samples,
                                             out_format,
                                             0);
    if (linesize[0] < 0) {
        qWarning() << "计算输出缓冲区大小失败";
        av_freep(&data[0]);
        return -1;
    }

    // 添加调试输出
    //qDebug() << "输入采样率:" << pInFrame->sample_rate;
    //qDebug() << "输出采样率:" << settings.sample_rate;
    //qDebug() << "输入采样格式:" << pInFrame->format;
    //qDebug() << "输出采样格式:" << out_format;
    //qDebug() << "声道数:" << out_ch_layout.nb_channels;
    //qDebug() << "输入样本数:" << in_samples;
    //qDebug() << "计算的输出样本数:" << out_samples;
    //qDebug() << "转换后的样本数:" << converted_samples;
    //qDebug() << "Linesize[0]:" << linesize[0];

    return converted_samples;
}

void Codec::threadReadPacket()
{
    qDebug()<<"readin";
    while (m_isDecoding)
    {
        if (m_audPacketQueue.empty() || (m_vidPacketQueue.empty() && m_mediaType == video))
        {
            if (readPacket()==AVERROR_EOF && m_audPacketQueue.empty() && (m_mediaType == audio || m_vidPacketQueue.empty())){
                
                m_isDecoding = false;
                m_isEnd = true;
            }
        }
        //qDebug()<<m_sync.getClock();
    }
    qDebug()<<"readout";
}

void Codec::threadDecodeVideo()
{
    qDebug()<<"vidin";
    while (m_isDecoding)
    {
        m_vidPacketQueueMutex.lock();
        packetDecoder(m_vidPacketQueue,m_vidStreamIndex);
        m_vidPacketQueueMutex.unlock();
    }
    qDebug()<<"vidout";
    
}

void Codec::threadDecodeAudio()
{
    initAudioContext();
    qDebug()<<"audin";
    while (m_isDecoding)
    {
        m_audPacketQueueMutex.lock();
        packetDecoder(m_audPacketQueue,m_audStreamIndex);
        m_audPacketQueueMutex.unlock();
    }
    qDebug()<<"audout";
    swr_free(&pSwrCtx);
}

void Codec::startDecoding()
{
    if (m_isEnd)
    {
        setSeekTime(0.0);
    }
    
    m_isDecoding = true;
    m_isEnd = false;
    std::thread tRead(&Codec::threadReadPacket,this);
    if (tRead.joinable())
    {
        tRead.detach();
    }
    if (m_mediaType == video) {
        std::thread tVideo(&Codec::threadDecodeVideo,this);
        if ( tVideo.joinable())
        {
            tVideo.detach();
        }
    }
    std::thread tAudio(&Codec::threadDecodeAudio,this);
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

int32_t Codec::getFinalVidFrame(uint8_t *data[1], int linesize[1])
{
    if(m_vidBuffer.empty()) return -1;
    
    int res=0;
    res = videoFrameConvert(m_vidBuffer.front(),m_outVidSetting,data,linesize);
    if (!m_vidBuffer.front())
    {
        return -1;
    }
    
    av_frame_free(&(m_vidBuffer.front()));
    m_vidBuffer.pop();
    return res;
}

int32_t Codec::getFinalAudFrame(uint8_t *data[1], int linesize[1])
{

    int res=0;
    if(m_audBuffer.empty()) return -1;
    res = audioFrameConvert(m_audBuffer.front(),m_outAudSetting,data,linesize);
    
        double audioPts = m_audBuffer.front()->pts * av_q2d(m_pAvFormatCtx->streams[m_audStreamIndex]->time_base);
        
    m_sync.setClock(audioPts);
    av_frame_free(&(m_audBuffer.front()));
    m_audBuffer.pop();
    return res;
}

void Codec::setOutVideo(int width, int height)
{
    m_outVidSetting.width=width;
    m_outVidSetting.height=height;

}

OutVideoFrameSetting Codec::getOutVideoSettings()
{
    return m_outVidSetting;
}

OutVideoFrameSetting Codec::getRawVideoSettings()
{
    OutVideoFrameSetting rt = {m_pAvFormatCtx->streams[m_vidStreamIndex]->codecpar->width,
                                m_pAvFormatCtx->streams[m_vidStreamIndex]->codecpar->height,
                                static_cast<AVPixelFormat>(m_pAvFormatCtx->streams[m_vidStreamIndex]->codecpar->format)};
    return rt;
}

void Codec::setOutAudio(int sample_rate, int channel_count, int sample_fmt)
{
    m_outAudSetting.sample_rate=sample_rate;
    m_outAudSetting.channel_count=channel_count;
    m_outAudSetting.sample_fmt=sample_fmt;
}

OutAudioFrameSetting Codec::getOutAudioSettings()
{
    return m_outAudSetting;
}

OutAudioFrameSetting Codec::getRawAudioSettings()
{
    return OutAudioFrameSetting();
}

int Codec::getAudioSamples()
{
    return m_outAudSetting.sample;
}

double Codec::getSeekTime()
{
    return m_sync.getClock();
}

void Codec::setSeekTime(double time)
{
    // TODO 修复音频包堆积问题
    //std::lock_guard<std::mutex> lock(m_audPacketQueueMutex);
    //std::lock_guard<std::mutex> lock2(m_vidPacketQueueMutex);
    std::lock_guard<std::mutex> lock3(m_avFormatCtxMutex);

    if (!m_pAvFormatCtx || m_audStreamIndex < 0) {
        qWarning() << "无效的格式上下文或音频流索引";
        return;
    }

    m_audPacketQueueMutex.lock();
    while (!m_audPacketQueue.empty())
    {
        av_packet_free(&(m_audPacketQueue.front()));
        m_audPacketQueue.pop();
    }
    m_audPacketQueueMutex.unlock();
    while (!m_audBuffer.empty())
    {
        av_frame_free(&(m_audBuffer.front()));
        m_audBuffer.pop();
    }
    m_vidPacketQueueMutex.lock();
    while (!m_vidPacketQueue.empty())
    {
        av_packet_free(&(m_vidPacketQueue.front()));
        m_vidPacketQueue.pop();
    }
    m_vidPacketQueueMutex.unlock();
    // 刷新解码器内部缓存，避免残留数据
    if (m_pAudCodecCtx) {
        avcodec_flush_buffers(m_pAudCodecCtx);
    }
    if (m_pVidCodecCtx) {
        avcodec_flush_buffers(m_pVidCodecCtx);
    }
    int64_t pts;
    int res;
    if (m_mediaType == video) {
        pts = time / av_q2d(m_pAvFormatCtx->streams[m_vidStreamIndex]->time_base);
        res = av_seek_frame(m_pAvFormatCtx,m_vidStreamIndex,pts,AVSEEK_FLAG_BACKWARD);    
    } else {
        pts = time / av_q2d(m_pAvFormatCtx->streams[m_audStreamIndex]->time_base);
        res = av_seek_frame(m_pAvFormatCtx,m_audStreamIndex,pts,AVSEEK_FLAG_BACKWARD);
    }
    
    if (res < 0)
    {
        qWarning() << "seek失败";
        return;
    }
    
    m_sync.setClock(pts);
    
}

double Codec::getTime()
{
    //获取视频时长
    return m_pAvFormatCtx->duration / AV_TIME_BASE;
}

int32_t Codec::getTitleImgHeight(fs::path path)
{
    Codec codec;
    if (codec.openFile(path.c_str())<0) {
        codec.closeFile();
        return -1;
    }
    int32_t h = codec.m_picHeight;
    codec.closeFile();
    return h;
}

int32_t Codec::getTitleImgWidth(fs::path path)
{
    Codec codec;
    if (codec.openFile(path.c_str())<0) {
        codec.closeFile();
        return -1;
    }
    int32_t w = codec.m_picWidth;
    codec.closeFile();
    return w;
}

int32_t Codec::getTitleImg(fs::path path, int width, int height, uint8_t *data[1], int linesize[1])
{
    int32_t res;
    Codec codec;
    if (codec.openFile(path.c_str())<0) {
        codec.closeFile();
        return -1;
    }

    if (codec.m_picStreamIndex>0)
    {
        codec.changeVideoStream(codec.m_picStreamIndex);
        codec.setOutVideo(width,height);
        do
        {
            codec.readPacket();
            res = codec.packetDecoder(codec.m_vidPacketQueue,codec.m_vidStreamIndex);
            if (res == AVERROR_EOF) break;
        } while (codec.getFinalVidFrame(data,linesize) < 0);
        codec.closeFile();
        return res;
    } 
    else if (codec.m_mediaType == video)
    {
        codec.setOutVideo(width,height);
        do
        {
            codec.readPacket();
            res = codec.packetDecoder(codec.m_vidPacketQueue,codec.m_vidStreamIndex);
            if (res == AVERROR_EOF) break;
        } while (codec.getFinalVidFrame(data,linesize) < 0);
        codec.closeFile();
        return res;
    } 
    else {
        codec.closeFile();
        return -1;
    }
    
    
}
