#ifndef SPARKMEDIACONTROLER_H
#define SPARKMEDIACONTROLER_H

#include <thread>
#include <ctime>
#include <qt5/QtCore/QObject>
#include <qt5/QtGui/QImage>
#include <qt5/QtCore/QSize>
#include <qt5/QtMultimedia/QAudioFormat>
#include <qt5/QtMultimedia/QAudioOutput>
#include <qt5/QtCore/QIODevice>
#include "utils/Codec.h"

#include <SDL.h>

QT_USE_NAMESPACE

class SparkMediaControler : public QObject
{
    Q_OBJECT
private:
    Codec m_codec;

    QImage *image_frame; // 图像帧
    QSize size; // 大小
    QImage::Format format; // 色彩格式
    std::thread *codec_thead; // 解码线程
    
    QAudioFormat audio_format;
    QAudioOutput *audio_output;
    QIODevice * audio_device;

    SDL_AudioSpec audio_spec;
    SDL_AudioDeviceID audio_device_id;

    bool isPlay;
    bool haveFile;

public:

    void reSize(QSize size);
    void reSize(int widht, int height);

    int setAudioDevice();

    void play();
    void pause();

    friend void audioCallback(void* userData, Uint8* stream, int len);

    void openMedia(QString path);
    void closeMedia();
    QImage *getImg();
    void playAudio();
    void playImg();

    bool getStatus();
    void setSeekTime(double time){m_codec.setSeekTime(time);}

    SparkMediaControler(/* args */);
    ~SparkMediaControler();


signals:
    void onImageDone();
    void onStatusChange();
};



#endif // SPARKMEDIACONTROLER_H