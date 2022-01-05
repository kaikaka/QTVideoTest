#include "mainwindow.h"
#include <QDebug>
#include <QPushButton>
#include "mypushbutton.h"
#include "sender.h"
#include "receiver.h"

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

    //自定义信号，不需要实现
    Sender *sender = new Sender;
    Receiver *receiver = new Receiver;

    connect(sender,&Sender::exit,receiver,&Receiver::handelExit);
    //发出信号
    emit sender->exit();


//    connect(sender,&Sender::add,receiver,&Receiver::handelExit2);
//    int sum = emit sender->add(10,20);
//    qDebug() << sum;

    //lambda 写法 不需要写槽函数
    connect(sender,&Sender::add, [](int a1,int a2) {
        qDebug() << "sender" << a1 << a2;
    });
    int sum = emit sender->add(10,20);

    //连接两个信号时，后一个信号参数必须少于上一个信号参数
    //connect(sender,&Sender::add,sender,&Sender::exit);
    //qDebug() << emit sender->add(1,2);
}

MainWindow::~MainWindow()
{
}

