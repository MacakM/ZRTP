#ifndef CALLBACK_H
#define CALLBACK_H

#define AES1_KEY_LENGTH     16
#define SALT_KEY_LENGTH     14
#define SAS_LENGTH          4

#include "Integers.h"

typedef struct{
    char sas[SAS_LENGTH];
    uint8_t srtpKeyI[AES1_KEY_LENGTH];
    uint8_t srtpSaltI[SALT_KEY_LENGTH];
    uint8_t srtpKeyR[AES1_KEY_LENGTH];
    uint8_t srtpSaltR[SALT_KEY_LENGTH];
}SrtpMaterial;

/**
 * Class that defines which methods user has to implement and give to Zrtp class.
 *
 * @see Zrtp
 */
class ZrtpCallback {

public:
    ZrtpCallback() {}

    virtual ~ZrtpCallback() {}

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
     *
     * @param material  SRTP material
     */
    virtual void keyAgreed(SrtpMaterial *material) = 0;

    /**
     * Tells user that key agreement has failed.
     */
    virtual void agreementFailed() = 0;

};

#endif // CALLBACK_H
