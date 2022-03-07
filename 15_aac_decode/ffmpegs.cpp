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

#define IN_DATA_SIZE 20480
#define REFILL_THRESH 4096

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

static int decode (AVCodecContext *ctx,AVPacket *pkt,AVFrame *frame,QFile &outFile) {
    //发送压缩数据到解码器
    int ret = avcodec_send_packet(ctx,pkt);
    if (ret < 0) {
        qDebug() << "avcodec_send_packet  error";
        return ret;
    }
    while (true) {
        ret = avcodec_receive_frame(ctx,frame);
        if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF) {
            return 0;
        } else if (ret < 0) {
            qDebug() << "avcodec_receive_frame  error";
            return ret;
        }
        outFile.write((char *) frame->data[0],frame->linesize[0]);
    }

}

void FFmpegs::aacDecode(AudioDecodeSpc &out,const char *inFilename) {
    //记录返回结果
    int ret = 0;

    //用来存放读取的aac输入数据
    //AV_INPUT_BUFFER_PADDING_SIZE 是有些优化过的reader会一次性读取过多数据(防止越界）
    char inDataArray[IN_DATA_SIZE + AV_INPUT_BUFFER_PADDING_SIZE];
    char *inData = inDataArray;
    int inLen;//长度

    QFile inFile(inFilename);
    QFile outFile(out.filename);
    //解码器
    AVCodec *codec = nullptr;

    AVCodecContext *ctx = nullptr;

    //解析器上下文
    AVCodecParserContext *parserCtx = nullptr;

    //存放解码器的数据(aac)
    AVPacket *pkt = nullptr;
    //存放解码后的数据(pcm)
    AVFrame *frame = nullptr;
    //获取解码器
    codec = avcodec_find_decoder_by_name("libfdk_aac");
    if (!codec) {
        qDebug() << "decode not found";
        return;
    }

    //初始化解析器上下文
    parserCtx = av_parser_init(codec->id);
    if (!parserCtx) {
        qDebug() << "av_parser_init error";
        return;
    }
    //创建上下文
    ctx = avcodec_alloc_context3(codec);
    if (!ctx) {
        qDebug() << "avcodec_alloc_context3 error";
        goto end;
    }
    //创建avpacket
    pkt = av_packet_alloc();
    if (!pkt) {
        qDebug() << "av_packet_alloc error";
        goto end;
    }

    frame = av_frame_alloc();
    if (!frame) {
        qDebug() << "av_frame_alloc error";
        goto end;
    }

    ret = avcodec_open2(ctx,codec,nullptr);
    if (ret < 0) {
        qDebug() << "av_frame_alloc error";
        goto end;
    }

    if (!inFile.open(QFile::ReadOnly)) {
        qDebug() << "inFile.open error" << inFilename;
        goto end;
    }

    if (!outFile.open(QFile::WriteOnly)) {
        qDebug() << "outFile.open error";
        goto end;
    }
    while ((inLen = inFile.read(inDataArray,IN_DATA_SIZE)) > 0) {
        inData = inDataArray;
        while (inLen > 0) {
            //经过解析器解析 内部调用ff_aac_ac3_parse
            ret = av_parser_parse2(parserCtx,ctx,&pkt->data,&pkt->size,(uint8_t *)inData,inLen,AV_NOPTS_VALUE,AV_NOPTS_VALUE,0);
            if (ret < 0) {
                qDebug() << "av_parser_parse2 error";
                goto end;
            }
            //下次解析时，跳过已经解析过的数据
            inData += ret;
            inLen -= ret;
            //解码
            if (pkt->size > 0 && decode(ctx,pkt,frame,outFile) < 0) {
                goto end;
            }
        }
    }
    //刷新缓冲区
    decode(ctx,pkt,frame,outFile);
    out.sampleRate = ctx->sample_rate;
    out.sampleFmt = ctx->sample_fmt;
    out.chLayout = ctx->channel_layout;

    end:
    inFile.close();
    outFile.close();

    av_frame_free(&frame);
    av_packet_free(&pkt);
    avcodec_free_context(&ctx);
    av_parser_close(parserCtx);
    qDebug() << "正常结束";
}
