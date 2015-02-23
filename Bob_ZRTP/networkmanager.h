#ifndef NETWORKMANAGER_H
#define NETWORKMANAGER_H

#include <QObject>
#include <QUdpSocket>
#include <QTimer>
#include "zrtp.h"
#include "mycallbacks.h"

class NetworkManager : public QObject
{
    Q_OBJECT

    friend class MyCallbacks;

public:
    explicit NetworkManager(QObject *parent = 0);

signals:

public slots:
    void processPendingDatagram();

public:
    uint8_t getMyZid();

private:
    QUdpSocket *sendSocket;
    QUdpSocket *readSocket;
    QTimer timer;

    Zrtp *zrtp;
    ZrtpCallback *callbacks;
    uint8_t myZid;

};

#endif // NETWORKMANAGER_H
