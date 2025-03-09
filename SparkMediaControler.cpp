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
    qDebug()<<audio_format.channelCount();
    qDebug()<<audio_format.sampleRate();
    qDebug()<<audio_format.sampleSize();
    qDebug()<<audio_format.codec();
    
    m_codec.setOutAudio(audio_format.sampleRate(),audio_format.channelCount(),audio_format.sampleSize());
    if (audio_output == nullptr)
    {
        audio_output = new QAudioOutput(audio_format);
    }
    
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
    codec_thead = new std::thread(&SparkMediaControler::codec, this);
    codec_thead->detach();

    setAudioFormat(); // 设置音频设备格式
    audio_device = audio_output->start();
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

            
            uint8_t* data[1] = { reinterpret_cast<uint8_t*>(image_frame->bits()) };
            int linesize[1] = { static_cast<int>(image_frame->bytesPerLine()) };
            if(!m_codec.getFinalVidFrame(data,linesize)){
                emit onImageDone();
            }

            uint8_t* aud_data[1];
            int aud_size[1]={0};
            if (!m_codec.getFinalAudFrame(aud_data,aud_size))
            {
                audio_device->write((const char*)aud_data[0],aud_size[0]);
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
}

SparkMediaControler::~SparkMediaControler()
{
    closeMedia();
    audio_device->deleteLater();
    audio_output->deleteLater();
}