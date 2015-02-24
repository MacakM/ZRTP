#ifndef NETWORKMANAGER_H
#define NETWORKMANAGER_H

#include <QObject>
#include <QUdpSocket>
#include <QTimer>
#include "zrtp.h"
#include "mycallbacks.h"
#include "parser.h"

enum Role
{
    Initiator,
    Responder
};

class NetworkManager : public QObject
{
    Q_OBJECT

    friend class MyCallbacks;

public:
    explicit NetworkManager(int argc, char *argv[], QObject *parent = 0);

signals:

public slots:
    void processPendingDatagram();

public:
    uint8_t getMyZid();

private:
    QUdpSocket *sendSocket;
    QUdpSocket *readSocket;
    QTimer timer;

    Role myRole;
    QHostAddress sendIp;
    quint16 sendPort;
    QHostAddress receiveIp;
    quint16 receivePort;

    Zrtp *zrtp;
    ZrtpCallback *callbacks;
    uint8_t myZid;

    void setArguments(Arguments args);
};

#endif // NETWORKMANAGER_H
