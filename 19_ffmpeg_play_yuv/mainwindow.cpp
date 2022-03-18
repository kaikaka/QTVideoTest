#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include "yuvplayer.h"

static int yuvIdx = 0;
static Yuv yuvs[] = {
    {   "/Users/yinxiangkai/Desktop/test/seven_out.yuv",1280,720,AV_PIX_FMT_YUV420P,30 },
     {   "/Users/yinxiangkai/Desktop/test/318.yuv",1080,720,AV_PIX_FMT_YUV420P,30 }
                    };


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    _player = new YuvPlayer(this);
    connect(_player,&YuvPlayer::stateChanged,this,&MainWindow::onPlayerStateChanged);

    //设置播放器的位置
    int w = 500;
    int h = 500;
    int x = (width() - w) >> 1;
    int y = (height() - h) >> 1;
    _player->setGeometry(x,y,w,h);
    //设置需要播放的文件
    _player->setYuv(yuvs[yuvIdx]);
    _player->play();
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::onPlayerStateChanged() {
    if (_player->getState() == YuvPlayer::Playing) {
        ui->startButton->setText("pause");
    } else {
        ui->startButton->setText("play");
    }
}

void MainWindow::on_startButton_clicked()
{
    qDebug() << "on_startButton_clicked";
    if (_player->isPlaying()) {
        _player->pause();
    } else {
        _player->play();
    }
}


void MainWindow::on_stopButton_clicked()
{
    _player->stop();
}


void MainWindow::on_playButton_clicked()
{
    int yuvCount = sizeof (yuvs) / sizeof(Yuv);
    yuvIdx = ++yuvIdx % yuvCount;
    _player->stop();
    _player->setYuv(yuvs[yuvIdx]);
    _player->play();
}

