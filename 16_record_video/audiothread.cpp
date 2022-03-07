#include "audiothread.h"
#include <QDebug>
#include <QFile>
#include <QDateTime>

extern "C" {
    #include <libavformat/avformat.h>
    #include <libavutil/avutil.h>
    #include <libavdevice/avdevice.h>
    #include <libavutil/imgutils.h>
    #include <libavcodec/avcodec.h>
}

// 格式名称、设备名称目前暂时使用宏定义固定死
#define FMT_NAME "avfoundation"
#define DEVICE_NAME "0"

AudioThread::AudioThread(QObject *parent) : QThread(parent) {
    // 当监听到线程结束时（finished），就调用deleteLater回收内存
    connect(this, &AudioThread::finished,
            this, &AudioThread::deleteLater);
}

AudioThread::~AudioThread() {
    // 断开所有的连接
    disconnect();
    // 内存回收之前，正常结束线程
    requestInterruption();
    // 安全退出
    quit();
    wait();
    qDebug() << this << "析构（内存被回收）";
}

void AudioThread::run() {
    //根据格式名称获取输入格式对象，后面需要利用输入格式对象打开设备
    AVInputFormat *fmt = av_find_input_format(FMT_NAME);
    if (!fmt) {
        // 如果找不到输入格式
        qDebug() << "找不到输入格式" << FMT_NAME;
        return;
    }
    //打开设备
    AVFormatContext *ctx = nullptr;

    AVDictionary *options = nullptr;
    av_dict_set(&options,"video_size","640x480",0);
    av_dict_set(&options,"pixel_format","yuyv422",0);
    av_dict_set(&options,"framerate","30",0);

    int ret = avformat_open_input(&ctx,DEVICE_NAME,fmt,&options);
    if (ret < 0) {
        char errbuf[1024] = {0};
        av_strerror(ret,errbuf,sizeof(errbuf));
        qDebug() << "打开设备失败" << errbuf;
        return;
    }

    //采集数据
    //播放视频：ffplay -video_size 640x480 -pixel_format yuyv422 -framerate 30 16_yuv.yuv
    const char *fileName = "/Users/yinxiangkai/Desktop/test/16_yuv.yuv";
    QFile file(fileName);
    if(!file.open(QFile::WriteOnly)) {
        qDebug() << "文件打开失败";
        avformat_close_input(&ctx);
        return;
    }

    AVCodecParameters *params = ctx->streams[0]->codecpar;
    AVPixelFormat pixFmt = (AVPixelFormat) params->format;
    int imageSize = av_image_get_buffer_size(pixFmt,params->width,params->height,1);
    //数据包
    AVPacket *pkt = av_packet_alloc();
    while (!isInterruptionRequested()) {
        //不停采集数据
        ret = av_read_frame(ctx,pkt);
        if (ret == 0) {
            //读取成功
            file.write((const char *) pkt->data,imageSize);
            av_packet_unref(pkt);
        } else if (ret == AVERROR(EAGAIN)) {
            //资源临时不可用
            continue;
        } else  {
            //其他错误
            break;
        }
    }
    av_packet_free(&pkt);
    file.close();
    avformat_close_input(&ctx);
    qDebug() << this << "the end!!!";
}
