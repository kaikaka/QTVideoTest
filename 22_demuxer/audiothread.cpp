#include "audiothread.h"
#include "demuxer.h"
#include <QDebug>
extern "C" {
#include <libavutil/imgutils.h>
}

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
    AudioDecodeSpec aOut;
      aOut.filename = "/Users/yinxiangkai/Desktop/test/seven13.pcm";

      VideoDecodeSpec vOut;
      vOut.filename = "/Users/yinxiangkai/Desktop/test/seven_out13.yuv";

      Demuxer().demux("/Users/yinxiangkai/Desktop/test/seven.mp4", aOut, vOut);

      qDebug() << aOut.sampleRate
               << av_get_channel_layout_nb_channels(aOut.chLayout)
               << av_get_sample_fmt_name(aOut.sampleFmt);


      qDebug() << vOut.width << vOut.height
               << vOut.fps << av_get_pix_fmt_name(vOut.pixFmt);
      //播放分离的数据
//      ffplay -ar 44100 -ac 2 -f f32le -i seven13.pcm
//      ffplay -f rawvideo -video_size 1280x720 -pix_fmt yuv420p  seven_out13.yuv
}
