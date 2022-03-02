#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <SDL2/SDL.h>

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


void MainWindow::showSDLVersion() {
    SDL_version version;
    SDL_VERSION(&version);
    qDebug() << "SDL 的版本 " << version.major << version.minor << version.patch;
}


void MainWindow::on_audioButton_clicked()
{
//    showSDLVersion();

    if (_playThread) {
        //停止播放
        _playThread->requestInterruption();
        _playThread = nullptr;
        ui->audioButton->setText("开始播放");
    } else {
        _playThread = new playThread(this);
        _playThread->start();
        connect(_playThread,&playThread::finished,[this]{
            _playThread = nullptr;
            ui->audioButton->setText("开始播放");
        });
        ui->audioButton->setText("停止播放");
    }
}
