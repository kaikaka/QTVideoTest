#include "ffmpegs.h"
#include <QFile>
#include <QDebug>

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavutil/imgutils.h>
}

#define ERROR_BUF(ret) \
    char errbuf[1024]; \
    av_strerror(ret, errbuf, sizeof (errbuf));

//输入缓冲区的大小
#define IN_DATA_SIZE 4096

FFmpegs::FFmpegs() {

}

static int frameIdx = 0;

static int decode(AVCodecContext *ctx,AVFrame *frame,AVPacket *pkt,QFile &outFile) {
   // 发送压缩数据到解码器
    int ret = avcodec_send_packet(ctx,pkt);
    if (ret < 0) {
        ERROR_BUF(ret);
        qDebug() << "avcodec_send_packet error" << errbuf;
        return ret;
    }
    while (true) {
        //获取解码后的数据
        ret = avcodec_receive_frame(ctx,frame);
        if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF) {
            return 0;
        } else if (ret < 0) {
            ERROR_BUF(ret);
            qDebug() << "avcodec_receive_frame error" << errbuf;
            return ret;
        }
        qDebug() << "解码出来" << ++ frameIdx << "帧";
        //将解码后的数据写入文件 Y:U:V ,1:0.25:0.25
        //写入y平面
        outFile.write((char *)frame->data[0],frame->linesize[0] * ctx->height);
        //写入u平面
        outFile.write((char *)frame->data[1],frame->linesize[1] * ctx->height >> 1);
        //写入v平面
        outFile.write((char *)frame->data[2],frame->linesize[2] * ctx->height >> 1);
    }
}

void FFmpegs::h264Decode(VideoDecodeSpec &out, const char *inFilename) {
    int ret = 0;
    //用来存放读取的输入文件数据（h264）
    char inDataArray[IN_DATA_SIZE + AV_INPUT_BUFFER_PADDING_SIZE];
    char *inData = inDataArray;
    //每次从输入文件中读取的文件长度（h264）输入缓冲区中，剩下的等待进行解码的有效数据长度
    int inLen;
    //是否已经读取到输入文件的尾部
    int inEnd = 0;

    QFile inFile(inFilename);
    QFile outFile(out.filename);
    //编码器
    AVCodec *codec = nullptr;
    //编码上下文
    AVCodecContext *ctx = nullptr;
    //解析器上下文
    AVCodecParserContext *parserCtx = nullptr;

    //存放解码前的h264数据
    AVPacket *pkt = nullptr;

    //存放解码后的yuv数据
    AVFrame *frame = nullptr;

    codec = avcodec_find_decoder(AV_CODEC_ID_H264);
    if (!codec) {
        qDebug() << "decoder not found";
        return;
    }
    //创建解析器上下文
    parserCtx = av_parser_init(codec->id);
    if (!parserCtx) {
        qDebug() << "av_parser_init error";
        return;
    }

    //创建编码上下文
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

    //创建avfremae
    frame = av_frame_alloc();
    if (!frame) {
        qDebug() << "av_frame_alloc error";
        goto end;
    }
    //打开解码器
    ret = avcodec_open2(ctx,codec,nullptr);
    if (ret < 0) {
        ERROR_BUF(ret);
        qDebug() << "avcodec_open2 error" << errbuf;
        goto end;
    }
    //打开文件
    if (!inFile.open(QFile::ReadOnly)) {
        qDebug() << "file open error :" << inFilename;
        goto end;
    }
    if (!outFile.open(QFile::WriteOnly)) {
        qDebug() << "file open error" << out.filename;
        goto end;
    }
    //读取文件数据
    do {

        inLen = inFile.read(inDataArray,IN_DATA_SIZE);
        //设置是否到了文件尾部
        inEnd = !inLen;
        //然indata指向数组的首元素
        inData = inDataArray;
        //只要输入缓冲区中还在等待解码的数据
        while (inLen > 0 || inEnd) {
            // 到了文件尾部（虽然没有读取任何数据，但也要调用av_parser_parse2，修复bug）
            // 经过解析器解析
            ret = av_parser_parse2(parserCtx,ctx,&pkt->data,&pkt->size,(uint8_t *)inData,inLen,AV_NOPTS_VALUE,AV_NOPTS_VALUE,0);
            if (ret < 0) {
                ERROR_BUF(ret);
                qDebug() << "av_parser_parse2 error" << errbuf;
                goto end;
            }
            //跳过已经解析过的数据
            inData += ret;
            //减去已经解析过的数据大小
            inLen -= ret;
            qDebug() << inEnd << pkt->size << ret;
            //解码
            if (pkt->size > 0 && decode(ctx,frame,pkt,outFile) < 0) {
                goto end;
            }
            //如果到了文件尾部
            if (inEnd) break;
        }
    } while (!inEnd);

    //刷新缓冲区
    decode(ctx,frame,pkt,outFile);
    //赋值输出参数
    out.width = ctx->width;
    out.height = ctx->height;
    out.pixFmt = ctx->pix_fmt;
    out.fps = ctx->framerate.num;

end:
    inFile.close();
    outFile.close();
    av_packet_free(&pkt);
    av_frame_free(&frame);
    av_parser_close(parserCtx);
    avcodec_free_context(&ctx);

}
