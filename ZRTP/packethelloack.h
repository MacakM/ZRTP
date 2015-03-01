#ifndef PACKETHELLOACK_H
#define PACKETHELLOACK_H

#include "packet.h"

class PacketHelloAck : public Packet
{
public:
    PacketHelloAck();

    virtual uint8_t *toBytes();

private:
    uint8_t data[12];
};

#endif // PACKETHELLOACK_H
