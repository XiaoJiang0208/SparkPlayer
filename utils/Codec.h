#ifndef CODEC_H
#define CODEC_H

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/avutil.h>
#include <libswscale/swscale.h>
}

#include <queue>
#include <QDebug>

class Codec // 单例模式
{
private:
    AVFormatContext *m_pAvFormatCtx; // 格式上下文
    AVCodecContext *m_pVidCodecCtx; // 视频编解码上下文
    uint32_t m_vidStreamIndex; // 视频流索引
    AVCodecContext *m_pAudCodecCtx; // 音频编解码上下文
    uint32_t m_audStreamIndex; // 音频流索引

    std::queue<AVFrame *> m_vidBuffer; // 视屏缓冲区
    std::queue<AVFrame *> m_aioBuffer; // 视屏缓冲区

public:
    Codec(/* args */);
    ~Codec();
    
    int32_t openFile(const char *path); // 打开文件
    void closeFile(); // 关闭文件
    int32_t readFrame(); // 读取帧
    int32_t decodePacketToFrame(AVCodecContext *pCodecCtx, const AVPacket *pPacket, AVFrame **ppFrame); // 解码视频帧
    int32_t videoConvert(const AVFrame *pInFrame, /*AVPixelFormat eOutFormat, */int32_t out_width, int32_t out_height, uint8_t* data[1],int linesize[1]); // 输出视屏帧
    AVFrame *getVidFrame();
    void popVidFrame();
    int32_t getVidBufferCount();
    
};


#endif // CODEC_H