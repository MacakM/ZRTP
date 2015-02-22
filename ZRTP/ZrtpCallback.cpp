#include "zrtp.h"

ZrtpCallback::ZrtpCallback(function pFunc)
{
    sendData = pFunc;
}
