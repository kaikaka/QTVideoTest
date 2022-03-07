#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QTime>

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

void MainWindow::onTimeChanged(unsigned long long ms) {
    QTime time(0, 0, 0, 0);
    QString text = time.addMSecs(ms).toString("mm:ss.z");
    ui->timeLabel->setText(text.left(7));
}

void MainWindow::on_audioButton_clicked()
{
    _audioThread = new AudioThread(this);
    _audioThread->start();
}

