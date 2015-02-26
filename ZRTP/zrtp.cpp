#include "zrtp.h"

Zrtp::Zrtp(uint8_t *zid, ZrtpCallback *cb)
{
    myZID = zid;
    callback = cb;

    engine = new StateEngine(this);

    Event event;
    event.type = Start;
    engine->processEvent(&event);
}

void Zrtp::processTimeout()
{
    Event event;
    event.type = Timeout;
    engine->processEvent(&event);
}

bool Zrtp::sendData(const uint8_t *data, int32_t length)
{
    return callback->sendData(data, length);
}

bool Zrtp::activateTimer(int32_t time)
{
    return callback->activateTimer(time);
}

bool Zrtp::cancelTimer()
{
    return callback->cancelTimer();
}
