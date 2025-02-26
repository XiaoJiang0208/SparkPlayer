#ifndef CODEC_H
#define CODEC_H

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/avutil.h>
#include <libswscale/swscale.h>
}

#include <QDebug>

class Codec // 单例模式
{
private:
    Codec(/* args */);
    AVFormatContext *m_pAvFormatCtx; // 格式上下文
    AVCodecContext *m_pVidCodecCtx; // 视频编解码上下文
    uint32_t m_vidStreamIndex; // 视频流索引
    AVCodecContext *m_pAudCodecCtx; // 音频编解码上下文
    uint32_t m_audStreamIndex; // 音频流索引

public:
    ~Codec();
    static Codec *getInstance(); // 获取单例对象
    
    int32_t openFile(const char *path); // 打开文件
    void closeFile(); // 关闭文件

};


#endif // CODEC_H