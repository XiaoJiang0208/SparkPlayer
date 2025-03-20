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


void audioCallback(void* userData, Uint8* stream, int len) {
    SparkMediaControler* ctrl = static_cast<SparkMediaControler*>(userData);
    SDL_memset(stream, ctrl->audio_spec.silence, len);  // 默认填充静音
    int remaining = len;
    Uint8* ptr = stream;

    while (remaining > 0 && ctrl->isPlay) {
        uint8_t* aud_data[1] = {nullptr};
        int aud_size[1] = {0};
        if (ctrl->m_codec.getFinalAudFrame(aud_data, aud_size)) {
            // 无数据则跳出循环
            break;
        }
        int copySize = aud_size[0] < remaining ? aud_size[0] : remaining;
        SDL_memcpy(ptr, aud_data[0], copySize);
        ptr += copySize;
        remaining -= copySize;
    }
}
int SparkMediaControler::setAudioDevice()
{
    audio_spec.freq=48000;
    audio_spec.format = AUDIO_S16SYS;
    audio_spec.channels = 2;
    audio_spec.silence = 0;
    audio_spec.samples = 1024;
    audio_spec.callback = audioCallback;
    audio_spec.userdata = this;
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
    codec_thead = new std::thread(&SparkMediaControler::playImg, this);
    codec_thead->detach();
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
    //codec_thead = new std::thread(&SparkMediaControler::playAudio, this);
}
void SparkMediaControler::closeMedia(){
    haveFile = false;
    m_codec.closeFile();
}

QImage *SparkMediaControler::getImg()
{
    return image_frame;
}

// void SparkMediaControler::playAudio(){
//     auto start = std::chrono::high_resolution_clock::now();
//     SDL_PauseAudioDevice(audio_device_id,0);
//     while (true)
//     {
//         if (!haveFile)
//         {
//             return;
//         }
//         if (isPlay)
//         {
//             if (m_codec.isEnd())
//             {
//                 isPlay = false;
//             }


//             // uint8_t* aud_data[1];
//             // int aud_size[1]={0};
//             // if (!m_codec.getFinalAudFrame(aud_data,aud_size))
//             // {  
//             //     int res = SDL_QueueAudio(audio_device_id, aud_data[0], aud_size[0]);
//             //     if (res == -1)
//             //     {
//             //         qDebug() << "SDL_QueueAudio error:" << SDL_GetError();
//             //     }
//             // }
//             SDL_Delay(5);

//         }
//     }
    
// }

void SparkMediaControler::playImg()
{
    qDebug()<<"in";
    SDL_PauseAudioDevice(audio_device_id,0);
    while (isPlay)
    {
        if (!haveFile)
        {
            return;
        }
        if (m_codec.isEnd())
        {
            isPlay = false;
        }

        uint8_t* data[1] = { reinterpret_cast<uint8_t*>(image_frame->bits()) };
        int linesize[1] = { static_cast<int>(image_frame->bytesPerLine()) };
        if(!m_codec.getFinalVidFrame(data,linesize)){
            emit onImageDone();
        }
    }
    SDL_PauseAudioDevice(audio_device_id,1);
    qDebug()<<"nice";
}

bool SparkMediaControler::getStatus()
{
    return isPlay;
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