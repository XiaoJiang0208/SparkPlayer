#ifndef VIDEOBOX_H
#define VIDEOBOX_H

#include "../include.h"
#include "../SparkMediaControler.h"
#include "../utils/ImageTools.h"
#include "../SparkAIAPI.h"

class Sparkplayer;

class VideoBox : public QLabel
{
    Q_OBJECT
private:
    bool is_fullscreen = false;
    QTimer *hideMouseTimer = nullptr;
    QList<QPair<QRect,QString>> marks;
    QPixmap *AIimg = nullptr;
public:
    VideoBox(QWidget *parent = nullptr);
    ~VideoBox();


    void fullscreen(bool is);

    void clearMark();
    void addMark(int x, int y, int w, int h, const QString &text);
    void showMark();

    //传入一个函数
    
    void SparkAI(int type);
    void SparkAIEmotion(QString res);
    void SparkAIAnimation(QString res);
    void SparkAIMusic(QString res);

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