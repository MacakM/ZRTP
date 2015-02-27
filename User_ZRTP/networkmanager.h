#ifndef NETWORKMANAGER_H
#define NETWORKMANAGER_H

#include <QObject>
#include <QUdpSocket>
#include <QTimer>
#include "zrtp.h"
#include "mycallbacks.h"
#include "parser.h"

//just for testing
#include <windows.h>
#include <conio.h>

class NetworkManager : public QObject
{
    Q_OBJECT

    friend class MyCallbacks;

public:
    explicit NetworkManager(int argc, char *argv[], QObject *parent = 0);

signals:

public slots:
    void processPendingDatagram();
    void sendTimeout();
    void sendMessage(uint8_t *msg);

public:
    uint8_t getMyZid();

private:
    uint8_t myZid;

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


    void setArguments(Arguments args);
};

#endif // NETWORKMANAGER_H
