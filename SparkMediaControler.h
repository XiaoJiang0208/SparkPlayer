#ifndef SPARKMEDIACONTROLER_H
#define SPARKMEDIACONTROLER_H

#include <thread>
#include <mutex>
#include <ctime>
#include <filesystem>

#include <qt5/QtCore/QObject>
#include <qt5/QtGui/QImage>
#include <qt5/QtCore/QSize>
#include <qt5/QtMultimedia/QAudioFormat>
#include <qt5/QtMultimedia/QAudioOutput>
#include <qt5/QtCore/QIODevice>
#include "utils/Codec.h"
#include "utils/ImageTools.h"

#include <SDL.h>

namespace fs = std::filesystem;

QT_USE_NAMESPACE

class SparkMediaControler : public QObject
{
    Q_OBJECT
private:
    Codec m_codec;
    fs::path m_path;

    QImage *image_frame; // 图像帧
    QSize size; // 大小
    QImage::Format format; // 色彩格式
    std::thread *codec_thead; // 解码线程

    QAudioFormat audio_format;
    QAudioOutput *audio_output;
    QIODevice * audio_device;

    SDL_AudioSpec audio_spec;
    SDL_AudioDeviceID audio_device_id;
    int volume;

    std::deque<fs::path> play_list;
    std::deque<fs::path>::iterator play_point;

    bool isPlay;
    bool haveFile;

    SparkMediaControler(/* args */);

    std::mutex play_mutex;
    void playThead(int step);

public:
    fs::path getPath(){return m_path;}

    void setVideoSize(QSize size);
    void setVideoSize(int widht, int height);
    QSize getVideoSize(){return size;}

    double getTime();

    int setAudioDevice();

    /**
     * @brief 开始或恢复媒体播放
     *
     * 此方法用于开始或继续播放媒体文件。参数 step 用于指定向后播放的整数步数，
     * 例如传入 5 表示向后播放 5 帧或相关的步长。使用正整数来控制播放进度。
     *
     * @param step 向后播放的步数
     */
    void play(int step = 0);
    /**
     * @brief 暂停媒体播放
     *
     * 此方法用于暂停媒体播放。
     */
    void pause();

    friend void audioCallback(void* userData, Uint8* stream, int len);

    /**
     * @brief 打开媒体文件
     *
     * 此方法用于打开媒体文件，参数 path 为媒体文件的路径。
     *
     * @param path 媒体文件的路径
     */
    void openMedia(fs::path path);
    // 关闭媒体文件    
    void closeMedia();
    // 是否有媒体文件
    bool isHaveFile();

    /**
     * @brief 添加媒体文件到播放列表
     *
     * 此方法用于将媒体文件添加到播放列表中，参数 path 为媒体文件的路径。
     *
     * @param path 媒体文件的路径
     * @param index 要添加的位置
     */
    void addMedia(fs::path path, int index = 0);
    /**
     * @brief 从播放列表中移除媒体文件
     *
     * 此方法用于从播放列表中移除媒体文件，参数 path 为媒体文件的路径。
     *
     * @param path 媒体文件的路径
     */
    void removeMedia(fs::path path);
    // 下一个
    void nextMedia();
    // 上一个
    void previousmedia();

    QImage *getImg();

    /**
     * @brief 获取播放状态
     *
     * 此方法用于获取当前播放状态，返回值为 true 表示正在播放，否则表示未播放。
     *
     * @return bool 播放状态
     */
    bool getStatus();
    /**
     * @brief 设置播放进度
     *
     * 此方法用于设置播放进度，参数 time 为播放进度的时间。
     *
     * @param time 播放进度的时间
     */
    void setSeekTime(double time);
    double getSeekTime(); // 获取播放进度

    void setVolume(int v);

    ~SparkMediaControler();

    static SparkMediaControler *getInstance();

signals:
    void onImageDone();
    void onStatusChange();
    void onTimeChange();
    void onFileOpen();
};



#endif // SPARKMEDIACONTROLER_H