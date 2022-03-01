#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFile>
#include <QDebug>

extern "C" {
    #include <libavformat/avformat.h>
    #include <libavutil/avutil.h>
}

// 格式名称、设备名称目前暂时使用宏定义固定死
#define FMT_NAME "avfoundation"
#define DEVICE_NAME ":0"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{

    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}
void MainWindow::on_audioButton_clicked()
{
    //根据格式名称获取输入格式对象，后面需要利用输入格式对象打开设备
    AVInputFormat *fmt = av_find_input_format("avfoundation");
    if (!fmt) {
        // 如果找不到输入格式
        qDebug() << "找不到输入格式" << FMT_NAME;
        return;
    }

    AVDictionary *options = nullptr;
    //打开设备
    AVFormatContext *ctx = nullptr;//格式上下文
    av_dict_set(&options,"freq","48000",0);
//    av_dict_set(&options,"sample_size","16",0);
    av_dict_set(&options,"channels","2",0);

    int ret = avformat_open_input(&ctx,DEVICE_NAME,fmt,&options);
    if (ret < 0) {
        char errbuf[1024] = {0};
        av_strerror(ret,errbuf,sizeof(errbuf));
        qDebug() << "打卡设备失败" << errbuf;
        return;
    }

    //采集数据
    const char *fileName = "/Users/yinxiangkai/Desktop/test/out.pcm";
    QFile file(fileName);
    if(!file.open(QFile::WriteOnly)) {
        qDebug() << "文件打开失败";
        avformat_close_input(&ctx);
        return;
    }

    int count = 50;
    // 数据包
    AVPacket *pkt = av_packet_alloc();
    while (count-- > 0) {
        qDebug() << count;
        // 从设备中采集数据，返回值为0，代表采集数据成功
        ret = av_read_frame(ctx, pkt);

        if (ret == 0) { // 读取成功
            // 将数据写入文件
            file.write((const char *) pkt->data, pkt->size);
//            avformat_flush(ctx);
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
    file.close();
    av_packet_free(&pkt);
    avformat_close_input(&ctx);
}

