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

//pcm 转wav
void MainWindow::pcm2wav(WAVHeader &header,
             const char *pcmFilename,
             const char *wavFilename) {
    header.blockAlign = (header.bitsPerSample * header.numChannels) >> 3;
    header.byteRate = header.sampleRate * header.blockAlign;
    QFile pcmFile(pcmFilename);
    if (!pcmFile.open(QFile::ReadOnly)) {
        qDebug() << "文件打开失败" << pcmFilename;
        return;
    }
    header.dataChunkDataSize = pcmFile.size();
    header.riffChunkDataSize = header.dataChunkDataSize + sizeof (WAVHeader) - sizeof(header.riffChunkId) - sizeof(header.riffChunkDataSize);

    QFile wavFile(wavFilename);
    if (!wavFile.open(QFile::WriteOnly)) {
        qDebug() << "文件打开失败" << wavFilename;
        return;
    }
    wavFile.write((const char *) &header,sizeof (WAVHeader));

    char buf[1024];
    int size;
    while ((size = pcmFile.read(buf,sizeof(buf))) > 0) {
        wavFile.write(buf,size);
    }
    //关闭文件
    pcmFile.close();
    wavFile.close();
}

void MainWindow::on_audioButton_clicked()
{
    WAVHeader header;
    header.sampleRate = 44100;
    header.bitsPerSample = 16;
    header.numChannels = 2;

    const char * pcmfilename = "/Users/yinxiangkai/Desktop/test/01.pcm";
    const char * wavfilename = "/Users/yinxiangkai/Desktop/test/003.wav";
    pcm2wav(header,pcmfilename,wavfilename);
}

