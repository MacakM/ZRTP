#ifndef PACKETHELLOACK_H
#define PACKETHELLOACK_H

#include "packet.h"

class PacketHelloAck : public Packet
{
public:
    PacketHelloAck();

    virtual uint8_t *toBytes();

private:
    uint8_t data[MAX_HELLOACK_LENGTH];
};

#endif // PACKETHELLOACK_H
