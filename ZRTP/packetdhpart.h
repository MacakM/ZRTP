#ifndef PACKETDHPART_H
#define PACKETDHPART_H

#include "packet.h"

class PacketDHPart : public Packet
{
public:
    PacketDHPart();

    virtual uint8_t *toBytes();

private:
    uint8_t data[20];
};

#endif // PACKETDHPART_H
