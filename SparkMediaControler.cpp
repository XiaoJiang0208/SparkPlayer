#include "SparkMediaControler.h"

void SparkMediaControler::reSize(QSize size)
{
    this->size = size;
    delete image_frame;
    image_frame = new QImage(size,format);
    
}

void SparkMediaControler::reSize(int widht, int height)
{
    reSize(QSize(widht,height));
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
            if(m_codec.getVidBufferCount()>0){
                uint8_t* data[1] = { reinterpret_cast<uint8_t*>(image_frame->bits()) };
                int linesize[1] = { static_cast<int>(image_frame->bytesPerLine()) };
                if(!m_codec.videoFrameConvert(m_codec.getVidFrame(),size.width(),size.height(),data,linesize)){
                    emit onImageDone();
                    m_codec.popVidFrame();
                }else {
                    qDebug() << "null";
                }
            }
            if (m_codec.getAudBufferCount()>0)
            {
                
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
}

SparkMediaControler::~SparkMediaControler()
{
    
}