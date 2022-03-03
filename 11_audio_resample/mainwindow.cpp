#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFile>
#include <QDebug>

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

void resampleAudio(const char *inFilename,int inSampleRate,AVSampleFormat inSampleFmt, int inChLayout,
                   const char *outFilename,int outSampleRate,AVSampleFormat outSampleFmt, int outChLayout) {
    QFile inFile(inFilename);
    QFile outFile(outFilename);

    //输入缓冲区
    uint8_t **inData = nullptr;
    //缓冲区的大小
    int inLinessize = 0;
    //声道数
    int inChs = av_get_channel_layout_nb_channels(inChLayout);
    //样本大小
    int inBytesPerSample = inChs * av_get_bytes_per_sample(inSampleFmt);
    //缓冲区的样本数量
    int inSamples = 1024;
    // 读取文件数据的大小
    int len = 0;


    uint8_t **outData = nullptr;
    int outLinesize = 0;
    int outChs = av_get_channel_layout_nb_channels(outChLayout);
    int outBytesPerSample = outChs * av_get_bytes_per_sample(outSampleFmt);

    int outSamples = av_rescale_rnd(outSampleRate,inSamples,inSampleRate,AV_ROUND_UP);
    /*
         inSampleRate     inSamples
         ------------- = -----------
         outSampleRate    outSamples

         outSamples = outSampleRate * inSamples / inSampleRate
         */
    int ret = 0;
    //创建重采样上下文
    SwrContext *ctx = swr_alloc_set_opts(nullptr,outChLayout,outSampleFmt,outSampleRate,inChLayout,inSampleFmt,inSampleRate,0,nullptr);
    if (!ctx) {
        qDebug() << "swr_alloc_set_opts error";
        goto end;
    }
    //初始化重采样上下文
    ret = swr_init(ctx);
    if (ret < 0) {
        qDebug() << "swr_init error";
        goto end;
    }
    //输入缓冲区
    ret = av_samples_alloc_array_and_samples(&inData,&inLinessize,inChs,inSamples,inSampleFmt,1);
    if (ret < 0) {
        qDebug() << "av_samples_alloc_array_and_samples error";
        goto end;
    }

    ret = av_samples_alloc_array_and_samples(&outData,&outLinesize,outChs,outSamples,outSampleFmt,2);
    if (ret < 0) {
        qDebug() << "av_samples_alloc_array_and_samples error";
        goto end;
    }

    if (!inFile.open(QFile::ReadOnly)) {
        qDebug() << "file open error";
        goto end;
    }

    if (!outFile.open(QFile::WriteOnly)) {
        qDebug() << "file open error";
        goto end;
    }

    //读取文件数据
    while ((len = inFile.read((char *)inData[0],inLinessize)) > 0) {
        inSamples = len / inBytesPerSample;
        //重采样
        ret = swr_convert(ctx,outData,outSamples,(const uint8_t **)inData,inSamples);
        if (ret < 0) {
            qDebug() << "swr_convert error";
            goto end;
        }
        //将转换后的数据写入到输出文件
        outFile.write((char *) outData[0],ret * outBytesPerSample);
    }
    //检查一下输出缓冲区是否还有残留的样本（已经重采样过的，转换过的）
    while ((ret = swr_convert(ctx,outData,outSamples,nullptr,0)) > 0) {
        outFile.write((char *) outData[0],ret * outBytesPerSample);
    }
    end:
        inFile.close();
        outFile.close();

        if(inData) {
            av_freep(&inData[0]);
        }
        av_freep(&inData);

        if (outData) {
            av_freep(&outData[0]);
        }
        av_freep(&outData);

        swr_free(&ctx);
}

void MainWindow::on_audioButton_clicked()
{

    const char * inFilename = "/Users/yinxiangkai/Desktop/test/44100_s16le_2.pcm";
    const char * outfilename = "/Users/yinxiangkai/Desktop/test/44100_f32le_1.pcm";
    resampleAudio(inFilename,44100,AV_SAMPLE_FMT_S16,AV_CH_LAYOUT_STEREO,outfilename,48000,AV_SAMPLE_FMT_FLT,AV_CH_LAYOUT_MONO);
}

