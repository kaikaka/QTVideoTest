#include "audiothread.h"

#include <QDebug>
#include "ffmpegs.h"

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
    VideoEncodeSpec in;
    in.filename = "/Users/yinxiangkai/Desktop/test/seven_out.yuv";
    in.width = 1280;
    in.height = 720;
    in.fps = 30;
    in.pixFmt = AV_PIX_FMT_YUV420P;

    FFmpegs::h264Encode(in, "/Users/yinxiangkai/Desktop/test/seven_out.h264");
}
