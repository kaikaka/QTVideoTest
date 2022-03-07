#include "mainwindow.h"

#include <QApplication>
extern "C" {
//设备
    #include <libavdevice/avdevice.h>
//格式
    #include <libavformat/avformat.h>
    #include <libavutil/avutil.h>
}

int main(int argc, char *argv[])
{

    //注册设备
    avdevice_register_all();

    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}
