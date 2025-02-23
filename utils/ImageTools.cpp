# include "ImageTools.h"

QPixmap ImageTools::toPixmap(const QString &url, QSize size, int radius)
{
    QImageReader raw(url);
    QPixmap image;
    bool isHeight = raw.size().height() > raw.size().width();
    if (isHeight)
    {
        QSize tgsize = QSize(size.width()*raw.size().width()/raw.size().height(),size.height());
        raw.setScaledSize(QSize(size.width(),size.height()*raw.size().height()/raw.size().width()));
        image = QPixmap::fromImage(raw.read().copy(0,(tgsize.height()-tgsize.width())/2,size.width(),size.height()));
    } else
    {
        QSize tgsize = QSize(size.width()*raw.size().width()/raw.size().height(),size.height());
        raw.setScaledSize(tgsize);
        image = QPixmap::fromImage(raw.read().copy((tgsize.width()-tgsize.height())/2,0,size.width(),size.height()));
    }
    QPixmap background = QPixmap(size);
    background.fill(Qt::transparent);
    QPainter painter(&background);
    painter.setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
    QPainterPath path;
    path.addRoundedRect(0,0,size.width(),size.height(), radius, radius);
    painter.setClipPath(path);
    painter.drawPixmap(0,0,size.width(),size.height(),image);
    return background;
}