#ifndef NETWORKMANAGER_H
#define NETWORKMANAGER_H

#include <QObject>
#include <QUdpSocket>
#include <QTime>
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

/**
 * Class that is in charge of all network events that happens on chosen port and IP address.
 *
 * @author Martin Macak
 */
class NetworkManager : public QObject
{
    Q_OBJECT
    typedef enum
    {
        activate = 1,
        stop = 2,
        restart = 3
    } Signal;

    friend class MyCallbacks;

public:
    /**
     * NetworkManager constructor.
     *
     * @param argc      command line argument argc
     * @param argv      command line argument argv
     * @param parent    parent of this class
     */
    explicit NetworkManager(int argc, char *argv[], QObject *parent = 0);

    /**
     * Set actual signal so NetworkManager can process it.
     *
     * @param signalNumber  1 = set timer with given time, 2 = cancel timer
     * @param time          given time
     */
    void setActualSignal(uint8_t signalNumber, int32_t time = 0);

signals:
    /**
     * Is emited when the signal has been set.
     */
    void signalReceived();

public slots:
    /**
     * Slot that is called when there are some pending data in readSocket.
     */
    void processPendingDatagram();

    /**
     * Slot that is called when timeout occurs
     */
    void createTimeoutThread();

    /**
     * Process set signal.
     */
    void processSignal();

    /**
     * Restarts all Zrtp process.
     */
    void restartZrtp();

public:
    /**
     * Calls ZRTP method to process timeout.
     *
     */
    void processZrtpTimeout();

    /**
     * Calls ZRTP method to process received message.
     *
     * @param msg       received message
     * @param length    length of the message
     */
    void processZrtpMessage(uint8_t *msg, int32_t length);

private:
    /**
     * Sets given arguments from parser.
     *
     * @param args  loaded arguments
     */
    void setArguments(Arguments args);

    int32_t packetDelay;
    int8_t packetLoss;

    threadVector threads;
    UserInfo info;

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

    QTimer *restartTimer;
    quint32 counter;
};

#endif // NETWORKMANAGER_H
