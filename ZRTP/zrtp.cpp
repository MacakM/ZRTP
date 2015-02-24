#include "zrtp.h"

Zrtp::Zrtp(uint8_t *zid, ZrtpCallback *cb)
{
    myZID = zid;
    callback = cb;

    uint8_t* msg = (uint8_t*)"Hello";
    sendData(msg,5);
    activateTimer(2000);
}

void Zrtp::processTimeout()
{
    /*Event event;
    event.type = Timeout;
    engine->processEvent(&event);*/
    uint8_t* msg = (uint8_t*)"Hello";
    sendData(msg,5);
    activateTimer(2000);
}

bool Zrtp::sendData(const uint8_t *data, int32_t length)
{
    return callback->sendData(data, length);
}

bool Zrtp::activateTimer(int32_t time)
{
    return callback->activateTimer(time);
}
