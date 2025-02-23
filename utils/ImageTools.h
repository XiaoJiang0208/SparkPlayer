#ifndef IMAGETOOLS_H
#define IMAGETOOLS_H

#include <QString>
#include <QSize>
#include <QPixmap>
#include <QImageReader>
#include <QPainter>
#include <QPainterPath>
#include <QDir>

class ImageTools
{
public:
    //ImageTools(/* args */);
    //~ImageTools();
    static QPixmap toPixmap(const QString &url, QSize size = QSize(30,30), int radius = 0);
};

#endif // IMAGETOOLS_H