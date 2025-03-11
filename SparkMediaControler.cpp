#include "SparkMediaControler.h"

void SparkMediaControler::reSize(QSize size)
{
    this->size = size;
    delete image_frame;
    image_frame = new QImage(size,format);
    m_codec.setOutVideo(size.width(),size.height());
}

void SparkMediaControler::reSize(int widht, int height)
{
    reSize(QSize(widht,height));
}

int SparkMediaControler::setAudioFormat()
{

    QAudioDeviceInfo deviceInfo = QAudioDeviceInfo::defaultOutputDevice();
    audio_format = deviceInfo.preferredFormat();
    audio_format.setSampleRate(48000);
    qDebug()<<audio_format.channelCount();
    qDebug()<<audio_format.sampleRate();
    qDebug()<<audio_format.sampleSize();
    qDebug()<<audio_format.codec();
    
    m_codec.setOutAudio(audio_format.sampleRate(),audio_format.channelCount(),audio_format.sampleSize());
    if (audio_output == nullptr)
    {
        audio_output = new QAudioOutput(audio_format);
        audio_output->setBufferSize(48000*2);
        qDebug() <<"dasdfasdfasdf"<< audio_output->bufferSize();
    }
    
    return 0;
}

int SparkMediaControler::setAudioDevice()
{
    audio_spec.freq=48000;
    audio_spec.format = AUDIO_S16SYS;
    audio_spec.channels = 2;
    audio_spec.silence = 0;
    audio_spec.samples = 4096;
    audio_spec.callback = nullptr;
    if ((audio_device_id = SDL_OpenAudioDevice(nullptr,0,&audio_spec, nullptr,SDL_AUDIO_ALLOW_ANY_CHANGE)) < 2){
        qWarning() << "open audio device failed ";
        return -1;
    }
    m_codec.setOutAudio(48000,2,16);
    
    return 0;
}

void SparkMediaControler::play()
{
    if (!haveFile)
    {
        qWarning() << "没有媒体文件";
        return;
    }
    m_codec.startDecoding();
    isPlay = true;
    emit onStatusChange();
}
void SparkMediaControler::pause()
{
    m_codec.stopDecoding();
    isPlay = false;
    emit onStatusChange();
}


void SparkMediaControler::openMedia(QString path){
    if(m_codec.openFile(path.toStdString().c_str())){
        haveFile = false;
        return;
    }
    haveFile = true;
    setAudioDevice();
    codec_thead = new std::thread(&SparkMediaControler::codec, this);
    codec_thead->detach();

}
void SparkMediaControler::closeMedia(){
    haveFile = false;
    m_codec.closeFile();
}

QImage *SparkMediaControler::getImg()
{
    return image_frame;
}

void SparkMediaControler::codec(){
    auto start = std::chrono::high_resolution_clock::now();
    SDL_PauseAudioDevice(audio_device_id,0);
    while (true)
    {
        if (!haveFile)
        {
            return;
        }
        if (isPlay)
        {
            if (m_codec.isEnd())
            {
                isPlay = false;
            }


            uint8_t* aud_data[1];
            int aud_size[1]={0};
            if (!m_codec.getFinalAudFrame(aud_data,aud_size))
            {
                
                int res = SDL_QueueAudio(audio_device_id, aud_data[0], aud_size[0]);
                if (res == -1)
                {
                    qDebug() << "SDL_QueueAudio error:" << SDL_GetError();
                }
                // int freeBytes = audio_output->bytesFree();
                // qDebug() <<"periodSize:   "<< audio_output->periodSize();

                // if(freeBytes >= aud_size[0]){
                //     //audio_device->write((const char*)aud_data[0], aud_size[0]);
                    
                    
                // } else {
                //     if (freeBytes-aud_size[0]<aud_size[0])
                //     {
                //         qDebug() << "缓冲区空间不足，当前剩余字节数:" << freeBytes;
                //         // 可选：等待一会儿再写入，或者清理缓冲区6后再写
                //         std::this_thread::sleep_for(std::chrono::milliseconds(10));
                        
                //     }
                    
                // }
                // auto end = std::chrono::high_resolution_clock::now();
                // std::chrono::duration<double> elapsed = end - start;
                // qDebug() << "realdelay:" << elapsed.count()*1000000;
                // start=end;
            }
            
            
            uint8_t* data[1] = { reinterpret_cast<uint8_t*>(image_frame->bits()) };
            int linesize[1] = { static_cast<int>(image_frame->bytesPerLine()) };
            if(!m_codec.getFinalVidFrame(data,linesize)){
                emit onImageDone();
            }

        }
    }
    
}

SparkMediaControler::SparkMediaControler()
{
    isPlay=false;
    haveFile=false;
    size.setWidth(1920);
    size.setHeight(1080);
    format = QImage::Format_RGB32;
    image_frame = new QImage(size,format);
    audio_output = nullptr;
    audio_device = nullptr;
    SDL_Init(SDL_INIT_AUDIO);
}

SparkMediaControler::~SparkMediaControler()
{
    closeMedia();
    audio_device->deleteLater();
    audio_output->deleteLater();
}