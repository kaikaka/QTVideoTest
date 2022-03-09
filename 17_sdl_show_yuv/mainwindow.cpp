#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <SDL2/SDL.h>
#include <QFile>

//这里的yuv是一张图片
#define FILENAME "/Users/yinxiangkai/Desktop/test/zero.yuv"
#define PIXEL_FORMAT SDL_PIXELFORMAT_IYUV
#define IMG_W 512
#define IMG_H 512

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

#define END(judge,func) \
    if (judge) { \
        qDebug() << #func << "error" << SDL_GetError(); \
        goto end; \
    }

void sdlShowYuv(void *winid) {
    //窗口
    SDL_Window *window = nullptr;
    //渲染上下文
    SDL_Renderer *renderer = nullptr;
    //纹理
    SDL_Texture *texture = nullptr;
    QFile file(FILENAME);
    //初始化子系统
    END(SDL_Init(SDL_INIT_VIDEO), SDL_Init);
    //创建窗口
    window = SDL_CreateWindowFrom(winid);
    END(!window,SDL_CreateWindow);
    //创建渲染上下文
    renderer = SDL_CreateRenderer(window,-1,SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!renderer) {
        renderer = SDL_CreateRenderer(window,-1,0);
        END(!renderer,SDL_CreateRenderer);
    }
    texture = SDL_CreateTexture(renderer,PIXEL_FORMAT,SDL_TEXTUREACCESS_STREAMING,IMG_W,IMG_H);
    END(!texture,SDL_CreateTexture);
    //打开文件
    if (!file.open(QFile::ReadOnly)) {
        qDebug() << "file open error" << FILENAME;
        goto end;
    }
    //将yuv的像素数据填充到texture
    END(SDL_UpdateTexture(texture,nullptr,file.readAll().data(),IMG_W),SDL_UpdateTexture);
    //设置绘制颜色(画笔颜色)清楚渲染目标
    END(SDL_RenderClear(renderer),SDL_RenderClear);
    //拷贝纹理到渲染目标
    END(SDL_RenderCopy(renderer,texture,nullptr,nullptr),SDL_RenderCopy);
    //更新所有的渲染操作到屏幕上
    SDL_RenderPresent(renderer);
    //等待退出事件
    while (true) {
        SDL_Event event;
        SDL_WaitEvent(&event);
        switch (event.type) {
        case SDL_QUIT:
            goto end;
        }
    }
    end:
    file.close();
    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

void MainWindow::on_playButton_clicked()
{
    sdlShowYuv((void *)ui->label->winId());
}

