#include "VideoBox.h"

VideoBox::VideoBox(QWidget *parent) : QLabel(parent), hideMouseTimer(new QTimer(this))
{
    
    resize(90,50);
    move(10,parent->height()-this->height()-10);
    //SparkMediaControler::getInstance()->setVideoSize(90,50);
    connect(SparkMediaControler::getInstance(),&SparkMediaControler::onImageDone,this,&VideoBox::showimg);
    hideMouseTimer->setSingleShot(true);
    hideMouseTimer->setInterval(3000); // 3秒无操作后隐藏鼠标
    connect(hideMouseTimer, &QTimer::timeout, this, [this](){
        this->setCursor(Qt::BlankCursor);
    });
    raise();
    //setPixmap(ImageTools::toPixmap(Path::applicationPath("images/icon.png").toString(),size,6));
}

VideoBox::~VideoBox()
{
}

void VideoBox::fullscreen(bool is)
{
    QPropertyAnimation *animation = new QPropertyAnimation(this,"geometry");
    //SparkMediaControler::getInstance()->setVideoSize(parentWidget()->width(),parentWidget()->height());
    
    if (is)
    {
        animation->setStartValue(this->geometry());
        animation->setEndValue(QRect(0,0,parentWidget()->width(),parentWidget()->height()));
        animation->setEndValue(QRect(0,0,parentWidget()->width(),parentWidget()->height()));
    } else {
        animation->setStartValue(this->geometry());
        animation->setEndValue(QRect(10,parentWidget()->height()-50-5,90,50));
    }
    // TODO : 优化了显示效果没有优化播放时缩放性能
    //media_controler->setVideoSize(90,50);
    //if (!SparkMediaControler::getInstance()->getStatus()) { // 如果不在播放在重新渲染放大之后的当前帧   
    //    
    //}
    connect(animation, &QPropertyAnimation::valueChanged, this, &VideoBox::showimg);
    qDebug()<<"ww";
    animation->start(QAbstractAnimation::DeleteWhenStopped);
    is_fullscreen = is;
    this->setMouseTracking(is_fullscreen);
    hideMouseTimer->stop();
    this->setCursor(Qt::ArrowCursor);
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

void VideoBox::mouseMoveEvent(QMouseEvent *ev)
{
    this->setCursor(Qt::ArrowCursor);
    hideMouseTimer->start(); // 重启计时器
    QPoint pos = ev->pos();
    if (pos.y()<50 || pos.y()>window()->height()-80) {
        emit onNeedShow();
    } else {
        emit onNeedHide();
    }
    
}

void VideoBox::resizeEvent()
{
    if (is_fullscreen)
    {
        this->resize(parentWidget()->size());
        showimg();
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