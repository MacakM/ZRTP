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

public:
    uint8_t* getMyZid() {return myZid;}

private:
    QUdpSocket *socket;
    QTimer timer;

    Zrtp *zrtp;
    ZrtpCallback *callbacks;
    uint8_t *myZid;
};

#endif // NETWORKMANAGER_H
