#ifndef PLAYTHREAD_H
#define PLAYTHREAD_H

#include <QThread>

class playThread : public QThread
{
    Q_OBJECT
public:
    explicit playThread(QObject *parent = nullptr);
    ~playThread();
signals:
private:
    void run();
};

#endif // PLAYTHREAD_H
