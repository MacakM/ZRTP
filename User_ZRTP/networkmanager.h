#ifndef NETWORKMANAGER_H
#define NETWORKMANAGER_H

#include <QObject>
#include <QUdpSocket>
#include <QTimer>
#include <QMutex>
#include "zrtp.h"
#include "mycallbacks.h"
#include "parser.h"
#include "zrtpmessage.h"
#include "zrtptimeout.h"

//just for testing
#include <windows.h>
#include <conio.h>
#include <fstream>
#include <stdlib.h>
#include <time.h>

class NetworkManager : public QObject
{
    Q_OBJECT

    friend class MyCallbacks;

public:
    explicit NetworkManager(int argc, char *argv[], QObject *parent = 0);

signals:

public slots:
    void processPendingDatagram();
    void processZrtpTimeout();
    void processZrtpMessage(uint8_t *msg, int32_t length);

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

    std::ofstream myFile;
    QMutex *mutex;

    void setArguments(Arguments args);
};

#endif // NETWORKMANAGER_H
