#ifndef NETWORKMANAGER_H
#define NETWORKMANAGER_H

#include <QObject>
#include <QUdpSocket>
#include <QTimer>
#include <QMutex>
#include <vector>
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

typedef std::vector<QThread*> threadVector;

class NetworkManager : public QObject
{
    Q_OBJECT
    typedef enum
    {
        activate = 1,
        stop = 2
    } Signal;

    friend class MyCallbacks;

public:
    explicit NetworkManager(int argc, char *argv[], QObject *parent = 0);

    void setActualSignal(uint8_t signalNumber, int32_t time = 0);

signals:
    void signalReceived();

public slots:
    void processPendingDatagram();
    void createTimeoutThread();
    void processSignal();

public:
    uint8_t getMyZid();
    void processZrtpTimeout();
    void processZrtpMessage(uint8_t *msg, int32_t length);

private:
    threadVector threads;

    Signal actualSignal;
    int32_t actualTime;

    uint8_t myZid;

    QUdpSocket *sendSocket;
    QUdpSocket *readSocket;
    QTimer *timer;

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
