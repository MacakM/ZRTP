#ifndef ZRTPTIMEOUT_H
#define ZRTPTIMEOUT_H

#include <QDebug>
#include <QThread>
#include <QMutex>
#include "networkmanager.h"

class NetworkManager;

class ZrtpTimeout : public QThread
{
    Q_OBJECT
public:
    explicit ZrtpTimeout(NetworkManager *manager);

private:
    NetworkManager *manager;

    void run();
};

#endif // ZRTPTIMEOUT_H
