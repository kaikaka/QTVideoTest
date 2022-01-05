#ifndef MYPUSHBUTTON_H
#define MYPUSHBUTTON_H

#include <QPushButton>

class myPushButton: public QPushButton
{
public:
    //explicit 禁止隐式构造
    explicit myPushButton(QWidget *parent = nullptr);
    ~myPushButton();
};

#endif // MYPUSHBUTTON_H
