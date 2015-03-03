#ifndef CALLBACK_H
#define CALLBACK_H

#include "Integers.h"


class ZrtpCallback {

public:
    ZrtpCallback() {}

    virtual bool sendData (const uint8_t* data, int32_t length) = 0;
    virtual bool activateTimer(int32_t timer) = 0;
    virtual bool cancelTimer() = 0;

};

#endif // CALLBACK_H
