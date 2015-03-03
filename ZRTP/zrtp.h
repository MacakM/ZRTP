#ifndef ZRTP_H
#define ZRTP_H

#include "Integers.h"
#include "ZrtpCallback.h"
#include "stateengine.h"

#include "packethello.h"
#include "packethelloack.h"
#include "packetcommit.h"
#include "packetdhpart.h"
#include "packetconfirm.h"
#include "packetconf2ack.h"

#include <iostream>

typedef enum
{
    Initiator,
    Responder
}Role;

class StateEngine;

class Zrtp
{
    friend class StateEngine;

public:
    Zrtp(uint8_t *zid, ZrtpCallback *cb, Role role, std::string clientId);

    void processMessage(uint8_t *msg, int32_t length);
    void processTimeout();

private:
    bool sendData(const uint8_t *data, int32_t length);
    bool activateTimer(int32_t time);
    bool cancelTimer();

    uint8_t *myZID;
    ZrtpCallback *callback;
    Role myRole;
    StateEngine *engine;

    PacketHello *hello;
    PacketHelloAck *helloAck;
};

#endif // ZRTP_H
