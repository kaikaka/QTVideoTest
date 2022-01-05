#include "receiver.h"
#include <QDebug>

Receiver::Receiver()
{

}

void Receiver::handelExit() {
    qDebug() << "Receiver::handelExit()";
}

int Receiver::handelExit2(int h1,int h2) {
    return h1 + h2;
}
