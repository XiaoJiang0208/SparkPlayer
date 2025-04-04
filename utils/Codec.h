#ifndef CODEC_H
#define CODEC_H

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/avutil.h>
#include <libswscale/swscale.h>
#include <libavutil/time.h>
#include <libavutil/channel_layout.h>
#include <libswresample/swresample.h>
#include <libavutil/imgutils.h>
#include <libavutil/opt.h>
#include <libavfilter/avfilter.h>
}

#include <thread>
#include <mutex>
#include <chrono>
#include <ctime>
#include <math.h>
#include <queue>
#include <QDebug>
#include <filesystem>

namespace fs = std::filesystem;

enum MediaType
{
    video,
    audio
};

struct OutVideoFrameSetting
{
    int32_t width = 0;
    int32_t height = 0;
    AVPixelFormat pixelFormat = AV_PIX_FMT_RGB32;
};

struct OutAudioFrameSetting
{
    int sample_rate = 48000;
    int channel_count = 2;
    int sample_fmt = 16;
    int sample = 0;
};

class AVSync
{
public:
    AVSync(){
        start_ = std::chrono::steady_clock::now();
    };

    void InitClock(){
        setClock(NAN);
    }
    void setClockAt(double pts, double time){
        pts_ = pts;
        pts_drift_ = pts_ - time;
    }
    double getClock(){
        double time = GetSeconds();
        return pts_drift_ + time;
    }
    void setClock(double pts){
        double time = GetSeconds();
        setClockAt(pts,time);
    }
    double GetSeconds(){
        auto now = std::chrono::steady_clock::now();
        std::chrono::duration<double> diff = now - start_;
        return diff.count();
    }

    // ...existing code...
    double pts_ = 0;
    double pts_drift_ = 0;
private:
    std::chrono::steady_clock::time_point start_;
};

class Codec
{
private:
    AVSync m_sync;
    AVFormatContext *m_pAvFormatCtx; // 格式上下文
    AVCodecContext *m_pVidCodecCtx; // 视频编解码上下文
    int32_t m_vidStreamIndex; // 视频流索引

    int32_t m_picStreamIndex;
    int32_t m_picHeight;
    int32_t m_picWidth;

    AVCodecContext *m_pAudCodecCtx; // 音频编解码上下文
    
    // 分配 SwrContext
    SwrContext *pSwrCtx;
    int32_t m_audStreamIndex; // 音频流索引
    double m_audDelayCorrection;

    double m_audioClock;

    std::queue<AVPacket *> m_vidPacketQueue;
    std::queue<AVPacket *> m_audPacketQueue;

    std::queue<AVFrame *> m_vidBuffer; // 视频缓冲区
    std::queue<AVFrame *> m_audBuffer; // 视频缓冲区
    OutVideoFrameSetting m_outVidSetting;
    OutAudioFrameSetting m_outAudSetting;

    std::atomic_bool m_isDecoding; // 是否在解码
    std::atomic_bool m_isEnd;
    std::atomic_bool overload; // 是否过载
    
    double m_audDelay;

    MediaType m_mediaType;

    // 音频列队锁
    std::mutex m_audPacketQueueMutex;
    std::mutex m_vidPacketQueueMutex;
    std::mutex m_avFormatCtxMutex;

    std::thread *m_readPacketThread;
    std::thread *m_decodeVideoThread;
    std::thread *m_decodeAudioThread;
        
    int32_t readPacket(); // 读取包
    /**
     * @brief 解码包
     * 
     * @param packetQueue 包列队
     * @param stream_index 流索引
     * @return int32_t 
     */
    int32_t packetDecoder(std::queue<AVPacket *> &packetQueue, int stream_index); // 解码包
    int32_t decodePacketToFrame(AVCodecContext *pCodecCtx, const AVPacket *pPacket, AVFrame **ppFrame); // 解码视频帧
    int32_t videoFrameConvert(const AVFrame *pInFrame, OutVideoFrameSetting &settings, uint8_t* data[1],int linesize[1]); // 转换视频帧
    int32_t initAudioContext();
    int32_t audioFrameConvert(const AVFrame *pInFrame, OutAudioFrameSetting &settings, uint8_t* data[1],int linesize[1]);
    void threadReadPacket();
    void threadDecodeVideo();
    void threadDecodeAudio();

public:
    Codec(/* args */);
    ~Codec();

    int32_t openFile(const char *path); // 打开文件
    void closeFile(); // 关闭文件

    int32_t changeVideoStream(int32_t i);
    void changeAudioStream(int32_t i);

    void startDecoding(); // 开始解码
    void stopDecoding(); // 停止解码
    bool isEnd(); // 是否结束
    bool getOverload(){return overload;}; // 是否过载

    int32_t getFinalVidFrame(uint8_t* data[1],int linesize[1]);
    int32_t getFinalAudFrame(uint8_t* data[1],int linesize[1]);
    
    void setOutVideo(int width, int height);
    OutVideoFrameSetting getOutVideoSettings();
    OutVideoFrameSetting getRawVideoSettings();
    void setOutAudio(int sample_rate, int channel_count, int sample_fmt);
    OutAudioFrameSetting getOutAudioSettings();
    OutAudioFrameSetting getRawAudioSettings();
    int getAudioSamples();

    double getSeekTime();
    void setSeekTime(double time);

    double getTime();

    MediaType getMediaType(){return m_mediaType;}

    static int32_t getTitleImgHeight(fs::path path);
    static int32_t getTitleImgWidth(fs::path path);
    static int32_t getTitleImg(fs::path path, int width, int height,uint8_t* data[1],int linesize[1]);
};


#endif // CODEC_H