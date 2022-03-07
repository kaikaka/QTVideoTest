#ifndef AUDIOTHREAD_H
#define AUDIOTHREAD_H
#include <QThread.h>


class AudioThread : public QThread {
    Q_OBJECT
private:
    void run();
    bool _stop = false;

public:
    explicit AudioThread(QObject *parent = nullptr);
    ~AudioThread();
signals:
};

#endif // AUDIOTHREAD_H
