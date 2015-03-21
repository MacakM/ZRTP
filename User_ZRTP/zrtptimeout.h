#ifndef ZRTPTIMEOUT_H
#define ZRTPTIMEOUT_H

#include <QDebug>
#include <QThread>
#include <QMutex>

class ZrtpTimeout : public QThread
{
    Q_OBJECT
public:
    explicit ZrtpTimeout();

private:

    void run();
};

#endif // ZRTPTIMEOUT_H
