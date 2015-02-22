#include "zrtp.h"

Zrtp::Zrtp(uint8_t *zid, ZrtpCallback *cb)
{
    myZID = zid;
    callback = cb;
    uint8_t* msg = (uint8_t*)"LOL";
    sendData(msg,3);
}

bool Zrtp::sendData(const uint8_t *data, int32_t length)
{
    return callback->sendData(data, length);
}
