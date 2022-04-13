#ifndef DEMUXER_H
#define DEMUXER_H

#include <QFile>

#ifdef __cplusplus
#define __STDC_CONSTANT_MACROS
#ifdef _STDINT_H
#undef _STDINT_H
#endif
extern "C" {
#include <stdint.h>
}
#endif

#ifndef   UINT64_C
#define   UINT64_C(value)   __CONCAT(value,ULL)
#endif

extern "C" {
    #include <libavcodec/avcodec.h>
    #include <libavformat/avformat.h>
    #include <libavutil/avutil.h>
}

typedef struct {
    const char *filename;
    int sampleRate;
    AVSampleFormat sampleFmt;
    int chLayout;
} AudioDecodeSpec ;

typedef struct {
    const char *filename;
    int width;
    int height;
    AVPixelFormat pixFmt;
    int fps;
} VideoDecodeSpec ;

class Demuxer
{
public:
    Demuxer();
    void demux(const char *inFilename,
                        AudioDecodeSpec &aOut,
                        VideoDecodeSpec &vOut);
private:
    //解封装上下文
    AVFormatContext *_fmtCtx = nullptr;
    //解码上下文
    AVCodecContext *_aDecodeCtx = nullptr,*_vDecodeCtx = nullptr;
    //流索引
    int _aStreamIdx = 0,_vStreamIdx = 0;
    //文件
    QFile _aOutFile,_vOutFile;
    //函数参数
    AudioDecodeSpec *_aOut = nullptr;
    VideoDecodeSpec *_vOut = nullptr;
    AVFrame *_frame = nullptr;
    //存放一帧解码图片的缓冲区
    uint8_t *_imgBuf[4] = {nullptr};
    int _imgLinesizes[4] = {0};
    int _imgSize = 0;
    //每个音频样本帧（包含所有声道）的大小
    int _sampleFrameSize = 0;
    //每一个音频样本的大小（单声道）
    int _sampleSize = 0;

    int initVideoInfo();
    int initAudioInfo();
    int initDecode(AVCodecContext **decodeCtx,int *streamIdx,AVMediaType type);
    int decode(AVCodecContext *decodeCtx,AVPacket *pkt,void(Demuxer::*func)());
    void writeVideoFrame();
    void writeAudioFrame();
};

#endif // DEMUXER_H
