# include "ImageTools.h"



QPixmap ImageTools::toPixmap(const QString &url, QSize size, int radius)
{
    QImageReader raw(url);
    QPixmap image;
    bool isHeight = raw.size().height() >= raw.size().width();
    if (isHeight) // 竖图
    {
        raw.setScaledSize(QSize(size.width(),size.height()*raw.size().height()/raw.size().width()));
        image = QPixmap::fromImage(raw.read().copy(0,(raw.scaledSize().height()-size.height())/2,size.width(),size.height()));
    } else
    {
        raw.setScaledSize(QSize(size.width()*raw.size().width()/raw.size().height(),size.height()));
        image = QPixmap::fromImage(raw.read().copy((raw.scaledSize().width()-size.width())/2,0,size.width(),size.height()));
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

QPixmap ImageTools::toPixmap(const QImage &raw, QSize size, int radius)
{
    QPixmap background = QPixmap(size);
    background.fill(Qt::transparent);
    QPainter painter(&background);
    painter.setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
    QPainterPath path;
    path.addRoundedRect(0,0,size.width(),size.height(), radius, radius);
    painter.setClipPath(path);
    QPixmap scaledPixmap = QPixmap::fromImage(raw.scaled(size, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    int x = (size.width() - scaledPixmap.width()) / 2;
    int y = (size.height() - scaledPixmap.height()) / 2;
    painter.drawPixmap(x, y, scaledPixmap);
    QPainterPath path2;
    path2.addRect(x,y,scaledPixmap.width(),scaledPixmap.height());
    path2 = path.subtracted(path2);
    painter.fillPath(path2,Qt::black);
    return background;
}
