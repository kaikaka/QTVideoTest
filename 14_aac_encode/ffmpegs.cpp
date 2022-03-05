#include "ffmpegs.h"

#include <QDebug>
#include <QFile>

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavutil/avutil.h>
}

FFmpegs::FFmpegs()
{

}

static int check_sample_fmt(const AVCodec *codec, enum AVSampleFormat sample_fmt) {
    const enum AVSampleFormat *p = codec->sample_fmts;
    while (*p != AV_SAMPLE_FMT_NONE) {
        if (*p == sample_fmt) return 1;
        p++;
    }
    return 0;
}

static int encode(AVCodecContext *ctx,AVFrame *frame,AVPacket *pkt,QFile &outFile) {
    //发送数据到编码器
    int ret = avcodec_send_frame(ctx,frame);
    if (ret < 0) {
        return ret;
    }
    //不断从编码器中取出编码后的数据
    while (true) {
        ret = avcodec_receive_packet(ctx,pkt);
        if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF) {
            return 0;
        } else if (ret < 0) {
            return ret;
        }
        //成功拿到编码后的数据，将编码后的数据写入文件
        outFile.write((char *)pkt->data,pkt->size);
        av_packet_unref(pkt);
    }
}

void FFmpegs::aacEncode(AudioEncodeSpc &in,const char *outFilename) {
    QFile inFile(in.filename);
    QFile outFile(outFilename);

    //记录返回结果
    int ret = 0;
    AVCodec *codec = nullptr;
    AVCodecContext *ctx = nullptr;
    //存放编码前的pcm数据
    AVFrame *frame = nullptr;
    //存放编码后的aac数据
    AVPacket *pkt = nullptr;
    codec = avcodec_find_encoder_by_name("libfdk_aac");
    if (!codec) {
        qDebug() << "encoder not found";
        return;
    }
    //libfdk_aac对输入数据的要求：采样格式必须是16位整数
    //检查输入数据的采样格式
    if (!check_sample_fmt(codec,in.sampleFmt)) {
        qDebug() << "unsupported sample format"
                         << av_get_sample_fmt_name(in.sampleFmt);
                return;
    }
    //创建编码上下文
    ctx = avcodec_alloc_context3(codec);
    if (!ctx) {
        qDebug() << "avcodec_alloc_context3 error";
        return;
    }
    //设置pcm参数
    ctx->sample_rate = in.sampleRate;
    ctx->sample_fmt = in.sampleFmt;
    ctx->channel_layout = in.chLayout;
    //比特率
    ctx->bit_rate = 32000;
    //规格
    ctx->profile = FF_PROFILE_AAC_HE_V2;
         //打卡编码器
    ret = avcodec_open2(ctx,codec,nullptr);
    if (ret < 0) {
        goto end;
    }

    //创建AVFrame
    frame = av_frame_alloc();
    if (!frame) {
        qDebug() << "av_frame_alloc error";
                    goto end;
    }
    frame->nb_samples = ctx->frame_size;
    frame->format = ctx->sample_fmt;
    frame->channel_layout = ctx->channel_layout;

    //创建缓冲区
    ret = av_frame_get_buffer(frame,0);
    if (ret < 0) {
        goto end;
    }

    //创建AVPacket
    pkt = av_packet_alloc();
    if (!pkt) {
        goto end;
    }

    if (!inFile.open(QFile::ReadOnly)) {
        goto end;
    }
    if (!outFile.open(QFile::WriteOnly)) {
        goto end;
    }

    //读取数据到frame中
    while ((ret = inFile.read((char *)frame->data[0],frame->linesize[0])) > 0) {
        if (ret < frame->linesize[0]) {
            int bytes = av_get_bytes_per_sample((AVSampleFormat) frame->format);
            int ch = av_get_channel_layout_nb_channels(frame->channel_layout);
            //设置样本帧数量
            frame->nb_samples = ret / (bytes * ch);
        }
        //编码
        if (encode(ctx,frame,pkt,outFile) < 0) {
            goto end;
        }
    }
    //刷新缓冲区
    encode(ctx,nullptr,pkt,outFile);

    end:
    inFile.close();
    outFile.close();

    av_frame_free(&frame);
    av_packet_free(&pkt);
    avcodec_free_context(&ctx);

    qDebug() << "正常结束";
}
