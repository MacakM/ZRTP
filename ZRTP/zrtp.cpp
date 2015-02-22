#include "zrtp.h"

Zrtp::Zrtp(uint8_t *zid, ZrtpCallback *cb) :
    myZID(zid), callback(cb)
{

}

bool Zrtp::sendData(const uint8_t *data, int32_t length)
{
    std::cout << "sending data via callback";
    callback->sendData(data, length);
}
