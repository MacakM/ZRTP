#ifndef NETWORKMANAGER_H
#define NETWORKMANAGER_H

#include <QObject>
#include <QUdpSocket>
#include <QTime>
#include <QTimer>
#include <QMutex>
#include <QSystemSemaphore>
#include <QTime>
#include <vector>

#include "zrtp.h"
#include "mycallbacks.h"
#include "parser.h"
#include "zrtpmessage.h"
#include "zrtptimeout.h"
#include "restarter.h"

#include <fstream>
#include <stdlib.h>
#include <time.h>

typedef std::vector<QThread*> threadVector;

/**
 * Class that is in charge of all network events that happens on chosen port and IP address.
 */
class NetworkManager : public QObject
{
    Q_OBJECT
    typedef enum
    {
        activateTimer = 1,
        stopTimer = 2,
        zrtpEnded = 3,
        zrtpFailed = 4,
        zrtpRestart = 5
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

	~NetworkManager();
	
    /**
     * Set actual signal so NetworkManager can process it.
     *
     * @param signalNumber  1 = activate timer with given time, 2 = cancel timer,
     *                      3 = zrtp ended, 4 = zrtp failed, 5 = zrtp restart
     * @param time          given time
     */
    void setActualSignal(uint8_t signalNumber, int32_t time = 0);

    /**
     * Returns whether the key agreement has ended.
     *
     * @return      true = ended, false = otherwise
     */
    bool hasEnded() { return ended; }

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
     * Slot that is called when timeout occurs.
     */
    void createTimeoutThread();

    /**
     * Process set signal.
     */
    void processSignal();

    /**
     * Restarts Zrtp process.
     */
    void restartZrtp();
	
	/**
      * Ends Zrtp process.
      */
    void endZrtp();

public:
    /**
     * Calls ZRTP method to process timeout.
     */
    void processZrtpTimeout() { zrtp->processTimeout(); }

    /**
     * Calls ZRTP method to process received message.
     *
     * @param msg       received message
     * @param length    length of the message
     */
    void processZrtpMessage(uint8_t *msg, int32_t length) { zrtp->processMessage(msg, length); }

private:
    /**
     * Sets given arguments from parser.
     *
     * @param args  arguments from command line
     */
    void setArguments(Arguments *args);

    Role myRole;
    QHostAddress sendIp;
    quint16 sendPort;
    QHostAddress receiveIp;
    quint16 receivePort;
    int32_t packetDelay;
    int8_t packetLoss;
    quint32 testCap;

    quint32 counter;

    threadVector threads;
    UserInfo info;

    Signal actualSignal;
    int32_t actualTime;

    uint8_t myZid[ZID_SIZE];

    QUdpSocket *sendSocket;
    QUdpSocket *readSocket;
    QTimer *timer;

    Zrtp *zrtp;
    ZrtpCallback *callbacks;

    std::ofstream myFile;
    QMutex *mutex;

    SrtpMaterial material;

    //tells main to quit
    bool ended;

    //important when multiple restart signals occurs
    bool restarted;

    QTime elapsedTimer;
};

#endif // NETWORKMANAGER_H
