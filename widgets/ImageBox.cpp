#include "ImageBox.h"

ImageBox::ImageBox(QWidget *parent):DWidget(parent)
{
    size = QSize(50,50);
    main = new DLabel(this);
    main->resize(size);
}

void ImageBox::setImage(const QString &url)
{
    QImageReader raw(url);
    bool isHeight = raw.size().height() > raw.size().width();
    if (isHeight)
    {
        raw.setScaledSize(QSize(size.width(),size.height()*raw.size().height()/raw.size().width()));
        image = QPixmap::fromImage(raw.read().copy(0,(size.height()-size.width())/2,size.width(),size.height()));
    } else
    {
        raw.setScaledSize(QSize(size.width()*raw.size().width()/raw.size().height(),size.height()));
        image = QPixmap::fromImage(raw.read().copy((size.width()-size.height())/2,0,size.width(),size.height()));
    }
    background = QPixmap(size);
    background.fill(QColor(228, 232, 236,150));
    QPainter painter(&background);
    painter.setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
    QPainterPath path;
    path.addRoundedRect(0,0,size.width(),size.height(),10,10);
    painter.setClipPath(path);
    painter.drawPixmap(0,0,size.width(),size.height(),image);
    main->setPixmap(background);
}

void ImageBox::getPixmap()
{
    
}

void ImageBox::resize(int size)
{
    resize(size,size);
}
void ImageBox::resize(QSize size)
{
    resize(size.width(),size.height());
}
void ImageBox::resize(int width, int height)
{
    size = QSize(width,height);
}

void ImageBox::setStyleSheet(const QString &styleSheet)
{
}