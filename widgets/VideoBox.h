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
    QTimer *hideMouseTimer;
    QImage *img;
public:
    VideoBox(QWidget *parent = nullptr);
    ~VideoBox();


    void fullscreen(bool is);

    void mousePressEvent(QMouseEvent *ev);
    void mouseMoveEvent(QMouseEvent *ev);
    void resizeEvent();
signals:
    void onFullscreen(bool t);
    void onNeedShow();
    void onNeedHide();
public slots:
    void showimg();
};


#endif