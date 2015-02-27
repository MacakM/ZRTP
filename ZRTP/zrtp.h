#ifndef ZRTP_H
#define ZRTP_H

#include "Integers.h"
#include "ZrtpCallback.h"
#include "stateengine.h"
#include "packethello.h"
#include <iostream>

class StateEngine;

class Zrtp
{
    friend class StateEngine;

public:
    Zrtp(uint8_t *zid, ZrtpCallback *cb);

    void processMessage(uint8_t *msg);
    void processTimeout();

private:
    bool sendData(const uint8_t *data, int32_t length);
    bool activateTimer(int32_t time);
    bool cancelTimer();

    uint8_t *myZID;
    ZrtpCallback *callback;
    StateEngine *engine;

    PacketHello *hello;
};

#endif // ZRTP_H
