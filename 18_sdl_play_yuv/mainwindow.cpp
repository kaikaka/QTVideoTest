#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>

#define RET(judge, func) \
    if (judge) { \
        qDebug() << #func << "error" << SDL_GetError(); \
        return; \
    }

#define FILENAME "/Users/yinxiangkai/Desktop/test/seven_out.yuv"
#define PIXEL_FORMAT SDL_PIXELFORMAT_IYUV
#define IMG_W 1280
#define IMG_H 720

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    //创建播放器画面
    _widget = new QWidget(this);
    _widget->setGeometry(100,50,IMG_W,IMG_H);
    //初始化子系统
    RET(SDL_Init(SDL_INIT_VIDEO),SDL_Init);
    //创建窗口
    _window = SDL_CreateWindowFrom((void *)_widget->winId());
    RET(!_window,SDL_CreateWindowFrom);
    //创建渲染上下文
    _renderer = SDL_CreateRenderer(_window,-1,SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!_renderer) {
        _renderer = SDL_CreateRenderer(_window,-1,0);
        RET(!_renderer,SDL_CreateRenderer);
    }
    //创建纹理
    _texture = SDL_CreateTexture(_renderer,PIXEL_FORMAT,SDL_TEXTUREACCESS_STREAMING,IMG_W,IMG_H);
    RET(!_texture,SDL_CreateTexture);
    _file.setFileName(FILENAME);
    if (!_file.open(QFile::ReadOnly)) {
        qDebug() << "file open error";
    }
}

MainWindow::~MainWindow()
{
    delete ui;
    _file.close();
    SDL_DestroyTexture(_texture);
    SDL_DestroyRenderer(_renderer);
    SDL_DestroyWindow(_window);
    SDL_Quit();
}


void MainWindow::on_startButton_clicked()
{
    qDebug() << "on_startButton_clicked";
    _timerid = startTimer(33);
}


void MainWindow::timerEvent(QTimerEvent *event) {
    int imgSize = IMG_W * IMG_H * 1.5;
    char data[imgSize];
    if (_file.read(data,imgSize) > 0) {
        //将yuv的像素数据填充到texture
        RET(SDL_UpdateTexture(_texture,nullptr,data,IMG_W),SDL_UpdateTexture);
        //设置绘制颜色
        RET(SDL_SetRenderDrawColor(_renderer,0,0,0,SDL_ALPHA_OPAQUE),SDL_SetRenderDrawColor);
        //用绘制颜色清除渲染目标
        RET(SDL_RenderClear(_renderer),SDL_RenderClear);
        //拷贝纹理数据到渲染目标
        RET(SDL_RenderCopy(_renderer,_texture,nullptr,nullptr),SDL_RenderCopy);
        //更新所有渲染操作到屏幕上
        SDL_RenderPresent(_renderer);

    } else {
        //文件读取完毕
        killTimer(_timerid);
    }
}

