#ifndef ZRTPMESSAGE_H
#define ZRTPMESSAGE_H

#include <QDebug>
#include <QThread>
#include <QMutex>
#include "networkmanager.h"

class NetworkManager;

class ZrtpMessage : public QThread
{
    Q_OBJECT
public:
    explicit ZrtpMessage(NetworkManager *manager, uint8_t *msg, int32_t length);

private:
    NetworkManager *manager;
    uint8_t *msg;
    int32_t length;

    void run();
};

#endif // ZRTPMESSAGE_H
