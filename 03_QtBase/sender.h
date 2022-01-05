#ifndef SENDER_H
#define SENDER_H

#include <QObject>

class Sender:public QObject
{
    Q_OBJECT
public:
    Sender();
signals:
    void exit();
    int add(int a1,int a2);
};

#endif // SENDER_H
