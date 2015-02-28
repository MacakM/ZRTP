#ifndef PACKETHELLO_H
#define PACKETHELLO_H

#include "packet.h"

class PacketHello : public Packet
{
public:
    PacketHello();

    virtual uint8_t *toBytes();

private:
    uint8_t data[20];
};

#endif // PACKETHELLO_H
