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
    m_sync.InitClock();
}

Codec::~Codec()
{
    closeFile();
}

int32_t Codec::openFile(const char *path){

    const AVCodec *pVidCodec = nullptr; // 视频编解码器
    const AVCodec *pAudCodec = nullptr; // 音频编解码器
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

    if (pVidCodec == nullptr && pAudCodec == nullptr)
    {
        qWarning() << "找不到有效的流";
        closeFile();
        return -1;
    }

    if (pAudCodec)
    {
        m_mediaType = MediaType::audio;
    }
    
    if (pVidCodec)
    {
        m_mediaType = MediaType::video;
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

int32_t Codec::packetDecoder(std::queue<AVPacket *> &packetQueue)
{
    if (packetQueue.empty())
    {
        return -1;
    }
    
    AVPacket *pPacket = packetQueue.front();
    AVFrame *pOutFrame = nullptr;
    int res = 0;

    if (pPacket->stream_index == m_vidStreamIndex) // 视频
    {
        // 将pts转换为秒数，假设m_vidTimeBase为视频time_base
        double videoPts = pPacket->pts * av_q2d(m_pAvFormatCtx->streams[m_vidStreamIndex]->time_base);
        // 计算与音频主时钟的差值
        double delay = videoPts - m_sync.getClock();
        if (delay < -1.0) {
            qDebug()<<"nonono"<<delay;
            av_packet_free(&pPacket);
            packetQueue.pop();
            return -1;
        }

        res = decodePacketToFrame(m_pVidCodecCtx, pPacket, &pOutFrame);
        if (res || pOutFrame==nullptr){
            av_packet_free(&pPacket);
            packetQueue.pop();
            return res;
        }

        // 将pts转换为秒数，假设m_vidTimeBase为视频time_base
        videoPts = pOutFrame->pts * av_q2d(m_pAvFormatCtx->streams[m_vidStreamIndex]->time_base);
        // 计算与音频主时钟的差值
        delay = videoPts - m_sync.getClock();

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
            qDebug()<<"666:"<<delay;
            av_usleep(static_cast<unsigned int>(delay * 1000000));
        }

        m_vidBuffer.push(pOutFrame);
        av_packet_free(&pPacket);
        packetQueue.pop();
        return 0;

    } else if (pPacket->stream_index == m_audStreamIndex) //音频
    {
        if (m_audBuffer.size()>2){
            return 0;
        }
        res = decodePacketToFrame(m_pAudCodecCtx, pPacket, &pOutFrame);
        if (pOutFrame == nullptr){
            av_packet_free(&pPacket);
            packetQueue.pop();
            return res;
        }

        
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

int32_t Codec::videoFrameConvert(const AVFrame *pInFrame, OutVideoFrameSetting &settings , uint8_t* data[1],int linesize[1])
{
    // 计算缩放比例（等比例缩放，使得图像能完整显示在目标画布内）
    double ratioWidth = (double)settings.width / pInFrame->width;
    double ratioHeight = (double)settings.height / pInFrame->height;
    double ratio = std::min(ratioWidth, ratioHeight);

    int newWidth = static_cast<int>(pInFrame->width * ratio);
    int newHeight = static_cast<int>(pInFrame->height * ratio);
    int xOffset = (settings.width - newWidth) / 2;
    int yOffset = (settings.height - newHeight) / 2;

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
    for (int y = 0; y < settings.height; y++) {
        memset(data[0] + y * linesize[0], 0, settings.width * bytesPerPixel);
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

int32_t Codec::audioFrameConvert(const AVFrame *pInFrame, OutAudioFrameSetting &settings, uint8_t *data[1], int linesize[1])
{
    int res = 0;
    if (!pInFrame) {
        qWarning() << "pInFrame is null";
        return -1;
    }

    // 分配 SwrContext
    SwrContext *pSwrCtx = swr_alloc();
    if (!pSwrCtx) {
        qWarning() << "分配重采样上下文失败";
        return -1;
    }

    // 配置输出通道布局（根据目标通道数）
    AVChannelLayout out_ch_layout;
    av_channel_layout_default(&out_ch_layout, settings.channel_count);
    

    // 根据采样位深选择输出采样格式
    AVSampleFormat out_format = AV_SAMPLE_FMT_NONE;
    switch (settings.sample_fmt) {
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
            out_format = AV_SAMPLE_FMT_NONE;
            break;
    }
    if (out_format == AV_SAMPLE_FMT_NONE) {
        qWarning() << "不支持的采样位深:" << settings.sample_fmt;
        swr_free(&pSwrCtx);
        return -1;
    }

    // 这里假设输入的 AVFrame 已设置好声道布局和采样格式
    // 使用输入 pInFrame->ch_layout 及 pInFrame->format，采样率为 pInFrame->sample_rate
    res = swr_alloc_set_opts2(&pSwrCtx,
                              &out_ch_layout,    // 输出声道布局
                              out_format,        // 输出采样格式（转换为packed格式）
                              settings.sample_rate, // 输出采样率
                              &(pInFrame->ch_layout), // 输入声道布局
                              (AVSampleFormat)pInFrame->format, // 输入采样格式
                              pInFrame->sample_rate, // 输入采样率
                              0,
                              nullptr);
    if (res < 0) {
        qWarning() << "配置重采样上下文失败:" << res;
        swr_free(&pSwrCtx);
        return res;
    }

    res = swr_init(pSwrCtx);
    if (res < 0) {
        qWarning() << "初始化重采样上下文失败:" << res;
        swr_free(&pSwrCtx);
        return res;
    }

    // 计算输出采样点数：延时加上输入采样点数
    int in_samples = pInFrame->nb_samples;
    //int64_t delay = swr_get_delay(pSwrCtx, pInFrame->sample_rate);
    int out_samples = swr_get_out_samples(pSwrCtx,pInFrame->nb_samples);

    // 获取通道数（通常可直接使用 pInFrame->ch_layout.nb_channels）
    int nb_channels = pInFrame->ch_layout.nb_channels;
    // 计算目标缓冲区大小
    int buffer_size = av_samples_get_buffer_size(nullptr, nb_channels,
                                                 out_samples, out_format, 0);
    if (buffer_size < 0) {
        qWarning() << "计算缓冲区大小失败";
        swr_free(&pSwrCtx);
        return -1;
    }

    if (data[0]!=NULL){
        delete [] data[0];
    }
    // 分配缓冲区并执行重采样
    data[0] = new uint8_t[buffer_size];
    int converted_samples = swr_convert(pSwrCtx, data, out_samples,
                                        pInFrame->data, in_samples);
    if (converted_samples < 0) {
        qWarning() << "重采样转换失败:" << converted_samples;
        swr_free(&pSwrCtx);
        delete[] data[0];
        return -1;
    }

    // 根据实际转换后的采样数重新计算输出数据大小
    linesize[0] = av_samples_get_buffer_size(nullptr, nb_channels, converted_samples,
                                             out_format, 0);
    if (linesize[0] < 0) {
        qWarning() << "重新计算输出缓冲区大小失败";
        swr_free(&pSwrCtx);
        delete[] data[0];
        return -1;
    }

    swr_free(&pSwrCtx);
    return 0;
}

void Codec::threadReadPacket()
{
    qDebug()<<"readin";
    while (m_isDecoding)
    {
        if (m_audPacketQueue.empty() || m_vidPacketQueue.empty())
        {
            if (readPacket()==AVERROR_EOF && m_audPacketQueue.empty() && m_vidPacketQueue.empty()){
                
                m_isDecoding = false;
                m_isEnd = true;
            }
        }
    }
    qDebug()<<"readout";
}

void Codec::threadDecodeVideo()
{
    qDebug()<<"vidin";
    while (m_isDecoding)
    {
        packetDecoder(m_vidPacketQueue);
    }
    qDebug()<<"vidout";
    
}

void Codec::threadDecodeAudio()
{
    qDebug()<<"audin";
    while (m_isDecoding)
    {
        packetDecoder(m_audPacketQueue);
    }
    qDebug()<<"audout";
    
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
    std::thread tVideo(&Codec::threadDecodeVideo,this);
    std::thread tAudio(&Codec::threadDecodeAudio,this);
    
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

int32_t Codec::getFinalVidFrame(uint8_t *data[1], int linesize[1])
{
    if(m_vidBuffer.empty()) return -1;
    
    int res=0;
    res = videoFrameConvert(m_vidBuffer.front(),m_outVidSetting,data,linesize);
    av_frame_free(&(m_vidBuffer.front()));
    m_vidBuffer.pop();
    return res;
}

int32_t Codec::getFinalAudFrame(uint8_t *data[1], int linesize[1])
{
    if(m_audBuffer.empty()) return -1;

    int res=0;
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

void Codec::setOutAudio(int sample_rate, int channel_count, int sample_fmt)
{
    m_outAudSetting.sample_rate=sample_rate;
    m_outAudSetting.channel_count=channel_count;
    m_outAudSetting.sample_fmt=sample_fmt;
}

double Codec::getSeekTime()
{
    return m_sync.getClock();
}

void Codec::setSeekTime(double time)
{
    if (!m_pAvFormatCtx || m_audStreamIndex < 0) {
        qWarning() << "无效的格式上下文或音频流索引";
        return;
    }
    int64_t pts = time / av_q2d(m_pAvFormatCtx->streams[m_audStreamIndex]->time_base);
    av_seek_frame(m_pAvFormatCtx,m_audStreamIndex,pts,AVSEEK_FLAG_BACKWARD);
    m_sync.setClock(pts);
    //pts = time / av_q2d(m_pAvFormatCtx->streams[m_vidStreamIndex]->time_base);
    //av_seek_frame(m_pAvFormatCtx,m_vidStreamIndex,pts,AVSEEK_FLAG_BACKWARD);

}

int32_t Codec::getTitleImg(QString path, int width, int height, uint8_t *data[1], int linesize[1])
{
    int32_t res;
    Codec codec;
    res = codec.openFile(path.toStdString().c_str());
    if (res) return res;

    if (codec.getMediaType()==MediaType::video)
    {
        do
        {
            codec.readPacket();
            res = codec.packetDecoder(codec.m_vidPacketQueue);
        } while (res = -11);
        if (res) return res;
        codec.setOutVideo(width,height);
        return codec.getFinalVidFrame(data,linesize);
    }
    
}
