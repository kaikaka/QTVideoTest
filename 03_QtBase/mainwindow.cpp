#include "mainwindow.h"
#include <QDebug>
#include <QPushButton>
#include "mypushbutton.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setWindowTitle("主窗口");
    setFixedSize(400,400);
    //设置起始坐标
    move(100,100);

    QPushButton *pBtn = new QPushButton;
    pBtn->setText("关闭");
    pBtn->setParent(this);
    pBtn->resize(100,50);
    pBtn->move(50,50);
    connect(pBtn,&QPushButton::clicked,this,&MainWindow::close);
}

MainWindow::~MainWindow()
{
}

