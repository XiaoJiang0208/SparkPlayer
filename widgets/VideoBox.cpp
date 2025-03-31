#include "VideoBox.h"

VideoBox::VideoBox(QWidget *parent) : QLabel(parent)
{
    
    resize(90,50);
    move(10,parent->height()-this->height()-10);
    background = new QLabel(parent);
    background->setStyleSheet(".QLabel{ background-color: rgb(56, 56, 56); border-radius: 5px; }");
    background->resize(this->size());
    background->move(this->pos());
    //SparkMediaControler::getInstance()->setVideoSize(90,50);
    connect(SparkMediaControler::getInstance(),&SparkMediaControler::onImageDone,this,&VideoBox::showimg);
    raise();
    //setPixmap(ImageTools::toPixmap(Path::applicationPath("images/icon.png").toString(),size,6));
}

VideoBox::~VideoBox()
{
}

void VideoBox::fullscreen(bool is)
{
    QPropertyAnimation *animation = new QPropertyAnimation(this,"geometry");
    QPropertyAnimation *animation1 = new QPropertyAnimation(background,"geometry");
    //SparkMediaControler::getInstance()->setVideoSize(parentWidget()->width(),parentWidget()->height());
    
    if (is)
    {
        animation->setStartValue(this->geometry());
        animation1->setStartValue(this->geometry());
        animation->setEndValue(QRect(0,0,parentWidget()->width(),parentWidget()->height()));
        animation->setEndValue(QRect(0,0,parentWidget()->width(),parentWidget()->height()));
    } else {
        animation->setStartValue(this->geometry());
        animation1->setStartValue(this->geometry());
        animation->setEndValue(QRect(10,parentWidget()->height()-50-5,90,50));
        animation1->setEndValue(QRect(10,parentWidget()->height()-50-5,90,50));
    }
    // TODO : 优化了显示效果没有优化播放时缩放性能
    //media_controler->setVideoSize(90,50);
    //if (!SparkMediaControler::getInstance()->getStatus()) { // 如果不在播放在重新渲染放大之后的当前帧   
    //    
    //}
    connect(animation, &QPropertyAnimation::valueChanged, this, &VideoBox::showimg);
    qDebug()<<"ww";
    animation->start(QAbstractAnimation::DeleteWhenStopped);
    animation1->start(QAbstractAnimation::DeleteWhenStopped);
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
        background->resize(parentWidget()->size());
        this->resize(parentWidget()->size());
        // if (!SparkMediaControler::getInstance()->getStatus())
        // {
        //     showimg();
        // }
    } else {
        background->move(10,parentWidget()->height()-50-5);
        this->move(10,parentWidget()->height()-50-5);
    }
}

void VideoBox::raise()
{
    background->raise();
    QLabel::raise();

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