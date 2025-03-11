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
}

#include <thread>
#include <chrono>
#include <ctime>
#include <math.h>
#include <queue>
#include <QDebug>

enum MediaType
{
    video,
    audio
};

struct OutVideoFrameSetting
{
    int32_t width = 1920;
    int32_t height = 1080;
    
};

struct OutAudioFrameSetting
{
    int sample_rate = 48000;
    int channel_count = 2;
    int sample_fmt = 16;
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

class Codec // 单例模式
{
private:
    AVSync m_sync;
    AVFormatContext *m_pAvFormatCtx; // 格式上下文
    AVCodecContext *m_pVidCodecCtx; // 视频编解码上下文
    uint32_t m_vidStreamIndex; // 视频流索引

    AVCodecContext *m_pAudCodecCtx; // 音频编解码上下文
    uint32_t m_audStreamIndex; // 音频流索引
    double m_audDelayCorrection;

    double m_audioClock;

    std::queue<AVPacket *> m_vidPacketQueue;
    std::queue<AVPacket *> m_audPacketQueue;

    std::queue<AVFrame *> m_vidBuffer; // 视屏缓冲区
    std::queue<AVFrame *> m_audBuffer; // 视屏缓冲区
    OutVideoFrameSetting m_outVidSetting;
    OutAudioFrameSetting m_outAudSetting;

    bool m_isDecoding; // 是否在解码
    bool m_isEnd;
    
    double m_audDelay;

public:
    Codec(/* args */);
    ~Codec();

    int32_t openFile(const char *path); // 打开文件
    void closeFile(); // 关闭文件
    
    int32_t readPacket(); // 读取包
    int32_t packetDecoder(std::queue<AVPacket *> &packetQueue);
    int32_t decodePacketToFrame(AVCodecContext *pCodecCtx, const AVPacket *pPacket, AVFrame **ppFrame); // 解码视频帧
    int32_t videoFrameConvert(const AVFrame *pInFrame, OutVideoFrameSetting &settings, uint8_t* data[1],int linesize[1]); // 转换视屏帧
    int32_t audioFrameConvert(const AVFrame *pInFrame, OutAudioFrameSetting &settings, uint8_t* data[1],int linesize[1]);

    void threadReadPacket();
    void threadDecodeVideo();
    void threadDecodeAudio();
    void startDecoding();
    void stopDecoding();
    bool isEnd();

    int32_t getFinalVidFrame(uint8_t* data[1],int linesize[1]);
    int32_t getFinalAudFrame(uint8_t* data[1],int linesize[1]);

    void setOutVideo(int width, int height);
    void setOutAudio(int sample_rate, int channel_count, int sample_fmt);
};


#endif // CODEC_H