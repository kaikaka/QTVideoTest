#include "audiothread.h"
#include <QDebug>
#include <QFile>
#include <QDateTime>

extern "C" {
    #include <libavformat/avformat.h>
    #include <libavutil/avutil.h>
}

// 格式名称、设备名称目前暂时使用宏定义固定死
#define FMT_NAME "avfoundation"
#define DEVICE_NAME ":0"

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
    AVInputFormat *fmt = av_find_input_format("avfoundation");
    if (!fmt) {
        // 如果找不到输入格式
        qDebug() << "找不到输入格式" << FMT_NAME;
        return;
    }
    //打开设备
    AVFormatContext *ctx = nullptr;

    int ret = avformat_open_input(&ctx,DEVICE_NAME,fmt,nullptr);
    if (ret < 0) {
        char errbuf[1024] = {0};
        av_strerror(ret,errbuf,sizeof(errbuf));
        qDebug() << "打卡设备失败" << errbuf;
        return;
    }

    //采集数据
    const char *fileName = "/Users/yinxiangkai/Desktop/test/13.wav";
    QFile file(fileName);
    if(!file.open(QFile::WriteOnly)) {
        qDebug() << "文件打开失败";
        avformat_close_input(&ctx);
        return;
    }

    //获取输入流
    AVStream *stream = ctx->streams[0];
    AVCodecParameters *params = stream->codecpar;

    //wav 需要将wav头先写入到文件中
    WAVHeader header;
    header.sampleRate = params->sample_rate;
    header.bitsPerSample = av_get_bits_per_sample(params->codec_id);
    header.numChannels = params->channels;
    if (params->codec_id >= AV_CODEC_ID_PCM_F32BE) {
        header.audioFormat = AUDIO_FORMAT_FLOAT;
    }
    header.blockAlign = header.bitsPerSample * header.numChannels >> 3;
    header.byteRate = header.sampleRate * header.blockAlign;
    file.write((char *)&header,sizeof (WAVHeader));
    // 数据包
    AVPacket *pkt = av_packet_alloc();
    while (!isInterruptionRequested()) {
        // 从设备中采集数据，返回值为0，代表采集数据成功
        ret = av_read_frame(ctx, pkt);

        if (ret == 0) { // 读取成功
            // 将数据写入文件
            file.write((const char *) pkt->data, pkt->size);
    //计算录音时长
               header.dataChunkDataSize += pkt->size;
               unsigned long long ms = 1000.0 * header.dataChunkDataSize / header.byteRate;
               emit timeChanged(ms); //发送信号
            // 释放资源
            av_packet_unref(pkt);
        } else if (ret == AVERROR(EAGAIN)) { // 资源临时不可用
            continue;
        } else { // 其他错误
            char errbuf[1024];
            av_strerror(ret, errbuf, sizeof (errbuf));
            qDebug() << "av_read_frame error" << errbuf << ret;
            break;
        }
    }

    file.seek(sizeof(WAVHeader) - sizeof (header.dataChunkDataSize));
    file.write((char *) &header.dataChunkDataSize,sizeof (header.dataChunkDataSize));

    //写入riff
    header.riffChunkDataSize = file.size() - sizeof (header.riffChunkId)
            - sizeof (header.riffChunkDataSize);
    file.seek(sizeof (header.riffChunkId));
    file.write((char *)&header.riffChunkDataSize ,sizeof(header.riffChunkDataSize));

    av_packet_free(&pkt);
    file.close();

    avformat_close_input(&ctx);
}

void AudioThread::setStop(bool stop) {
    _stop = stop;
}
