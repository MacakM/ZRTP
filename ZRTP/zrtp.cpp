#include "zrtp.h"

Zrtp::Zrtp(uint8_t *zid, ZrtpCallback *cb)
{
    myZID = zid;
    callback = cb;

    //testing
    uint8_t* msg = (uint8_t*)"Hello";
    sendData(msg,5);
}

bool Zrtp::sendData(const uint8_t *data, int32_t length)
{
    return callback->sendData(data, length);
}
