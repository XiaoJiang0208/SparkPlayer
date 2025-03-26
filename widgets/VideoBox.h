#ifndef VIDEOBOX_H
#define VIDEOBOX_H

#include "../include.h"
#include "../SparkMediaControler.h"
#include "../utils/ImageTools.h"

class Sparkplayer;

class VideoBox : public QLabel
{
    Q_OBJECT
private:
    bool is_fullscreen = false;
    QImage *img;
public:
    VideoBox(QWidget *parent = nullptr);
    ~VideoBox();


    void fullscreen(bool is);

    void mousePressEvent(QMouseEvent *ev);

    void resizeEvent();

signals:
    void onFullscreen(bool t);

public slots:
    void showimg();
};


#endif