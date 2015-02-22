#ifndef CALLBACK_H
#define CALLBACK_H

#include "Integers.h"

class ZrtpCallback {

    typedef bool (*function)(const uint8_t* data, int32_t length);

public:
    ZrtpCallback(function pFunc);

protected:
    function sendData;

};

#endif // CALLBACK_H
