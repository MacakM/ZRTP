#ifndef CALLBACK_H
#define CALLBACK_H

#include "Integers.h"

/**
 * Class that defines which methods user has to implement and give to Zrtp class.
 *
 * @see Zrtp
 */
class ZrtpCallback {

public:
    ZrtpCallback() {}

    /**
     * Sends data to other endpoint.
     *
     * @param data      sending data
     * @param length    data length
     *
     * @return          true = successful, false = otherwise
     */
    virtual bool sendData (const uint8_t* data, int32_t length) = 0;

    /**
     * Activates user's timer.
     *
     * @param time  timeout interval in milliseconds
     *
     * @return      true = successful, false = otherwise
     */
    virtual bool activateTimer(int32_t time) = 0;

    /**
     * Cancels user's timer.
     *
     * @return true = successful, false = otherwise
     */
    virtual bool cancelTimer() = 0;

    /**
     * Enters user's critical section.
     */
    virtual void enterCriticalSection() = 0;

    /**
     * Leaves user's critical section.
     */
    virtual void leaveCriticalSection() = 0;

    /**
     * Tells user that key agreement has ended successfuly.
     */
    virtual void keyAgreed() = 0;

};

#endif // CALLBACK_H
