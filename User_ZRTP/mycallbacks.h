#ifndef MYCALLBACKS_H
#define MYCALLBACKS_H

#include <QObject>
#include <QUdpSocket>
#include "networkmanager.h"

class NetworkManager;

/**
 * Class that implements defined methods from ZRTP library.
 */
class MyCallbacks : public ZrtpCallback
{

public:
    /**
     * Constructor of MyCallbacks.
     *
     * @param manager   NetworkManager that created this class
     */
    MyCallbacks(NetworkManager *manager);

    /**
     * Sends data to other endpoint.
     *
     * @param data      sending data
     * @param length    data length
     *
     * @return          true = successful, false = otherwise
     */
    virtual bool sendData (const uint8_t* data, int32_t length);

    /**
     * Activates user's timer.
     *
     * @param time  timeout interval in milliseconds
     *
     * @return      true = successful, false = otherwise
     */
    virtual bool activateTimer(int32_t time);

    /**
     * Cancels user's timer.
     *
     * @return true = successful, false = otherwise
     */
    virtual bool cancelTimer();

    /**
     * Enters user's critical section.
     */
    virtual void enterCriticalSection();

    /**
     * Leaves user's critical section.
     */
    virtual void leaveCriticalSection();

    /**
     * Tells user that key agreement has ended successfuly.
     */
    virtual void keyAgreed();

private:
    NetworkManager *manager;
};

#endif // MYCALLBACKS_H
