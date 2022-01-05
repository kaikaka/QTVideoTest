#ifndef RECEIVER_H
#define RECEIVER_H

#include <QObject>

//必须继承qobject
class Receiver: public QObject
{
    Q_OBJECT
public:
    Receiver();
public slots:
    void handelExit();

    int handelExit2(int h1,int h2);
};

#endif // RECEIVER_H
