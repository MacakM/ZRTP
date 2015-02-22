#ifndef ZRTP_H
#define ZRTP_H

#include "Integers.h"
#include "ZrtpCallback.h"
#include <iostream>

class Zrtp
{
public:
    Zrtp(uint8_t *zid, ZrtpCallback *cb);

private:
    bool sendData(const uint8_t *data, int32_t length);

    uint8_t *myZID;
    ZrtpCallback *callback;
};

#endif // ZRTP_H
