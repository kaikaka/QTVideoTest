#include "playthread.h"
#include <QDebug>
#include <QFile>
#include <SDL2/SDL.h>

PlayThread::PlayThread(void *winid, QObject *parent) : QThread(parent),_winid(winid) {
    // 当监听到线程结束时（finished），就调用deleteLater回收内存
    connect(this, &PlayThread::finished,
            this, &PlayThread::deleteLater);
}

#define FILENAME "/Users/yinxiangkai/Desktop/test/16_yuv.yuv"
#define PIXEL_FORMAT SDL_PIXELFORMAT_IYUV
#define IMG_W 512
#define IMG_H 512

#define END(judge,func) \
    if (judge) { \
        qDebug() << #func << "error" << SDL_GetError(); \
        goto end; \
    }

PlayThread::~PlayThread() {
    // 断开所有的连接
    disconnect();
    // 内存回收之前，正常结束线程
    requestInterruption();
    // 安全退出
    quit();
    wait();
    qDebug() << this << "析构（内存被回收）";
}

void PlayThread::run() {

}
