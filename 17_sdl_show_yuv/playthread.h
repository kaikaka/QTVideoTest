#ifndef PLAYTHREAD_H
#define PLAYTHREAD_H


class PlayThread : public QObject {
    Q_OBJECT
private:
    void run();
    void *_winid;

public:
    explicit PlayThread(void *winid, QObject *parent = nullptr);
    ~PlayThread();
signals:
};
#endif // PLAYTHREAD_H
