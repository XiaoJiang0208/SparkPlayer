#include "VideoBox.h"

VideoBox::VideoBox(QWidget *parent) : QLabel(parent)
{
    resize(90,50);
    move(10,parent->height()-this->height()-10);
    //SparkMediaControler::getInstance()->setVideoSize(90,50);
    connect(SparkMediaControler::getInstance(),&SparkMediaControler::onImageDone,this,&VideoBox::showimg);
    
}

VideoBox::~VideoBox()
{
}

void VideoBox::fullscreen(bool is)
{
    // TODO : 播放时缩放性能优化
    QPropertyAnimation *animation = new QPropertyAnimation(this,"geometry");
    //SparkMediaControler::getInstance()->setVideoSize(parentWidget()->width(),parentWidget()->height());
    
    animation->setDuration(300);
    if (is)
    {
        animation->setStartValue(this->geometry());
        animation->setEndValue(QRect(0,0,parentWidget()->width(),parentWidget()->height()));
    } else {
        animation->setStartValue(this->geometry());
        animation->setEndValue(QRect(10,parentWidget()->height()-50-5,90,50));
    }
    //media_controler->setVideoSize(90,50);
    if (!SparkMediaControler::getInstance()->getStatus()) { // 如果不在播放在重新渲染放大之后的当前帧   
        connect(animation, &QPropertyAnimation::valueChanged, this, &VideoBox::showimg);
        qDebug()<<"ww";
    }
    animation->start(QAbstractAnimation::DeleteWhenStopped);
    is_fullscreen = is;
    emit onFullscreen(is);
}

void VideoBox::mousePressEvent(QMouseEvent *ev)
{
    if (ev->button() == Qt::LeftButton)
    {
        if (!is_fullscreen)
        {
            fullscreen(true);
        }
        
    }
}

void VideoBox::resizeEvent()
{
    if (is_fullscreen)
    {
        this->resize(parentWidget()->size());
        if (!SparkMediaControler::getInstance()->getStatus())
        {
            showimg();
        }
    } else {
        this->move(10,parentWidget()->height()-50-5);
    }
}

void VideoBox::showimg()
{
    if (is_fullscreen)
    {
        setPixmap(ImageTools::toPixmap(*(SparkMediaControler::getInstance()->getImg()),QSize(this->width(),this->height())));
    } else {
        setPixmap(ImageTools::toPixmap(*(SparkMediaControler::getInstance()->getImg()),QSize(this->width(),this->height()),5));
    }

}