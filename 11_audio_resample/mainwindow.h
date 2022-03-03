#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <stdint.h>
#include <QMainWindow>

extern "C" {
    #include <libavformat/avformat.h>
    #include <libswresample/swresample.h>
    #include <libavutil/avutil.h>
}

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
private slots:
    void on_audioButton_clicked();

private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
