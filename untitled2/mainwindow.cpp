#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "untitled.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
        Untitled *ynd = new Untitled();
        int s = ynd->add(3,5);
        qDebug() << "相加的值是" << s << "。";
}

MainWindow::~MainWindow()
{
    delete ui;
}

