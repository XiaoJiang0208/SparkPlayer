#ifndef IMAGEBOX_H
#define IMAGEBOX_H

#include "../include.h"

DWIDGET_USE_NAMESPACE

class ImageBox : public DWidget
{
    Q_OBJECT
public:
    ImageBox(QWidget *parent = nullptr);

    void setImage(const QString &url);
    void getPixmap();

    void resize(int width, int height);
    void resize(int size);
    void resize(QSize size);

    void setStyleSheet(const QString &styleSheet);

private: 
    DLabel *main;
    QPixmap image;
    QPixmap background;

    QSize size;
};

#endif // IMAGEBOX_H