#ifndef PACKETCOMMIT_H
#define PACKETCOMMIT_H

#include "packet.h"

class PacketCommit : public Packet
{
public:
    PacketCommit();

    virtual uint8_t *toBytes();

private:
    uint8_t data[20];
};

#endif // PACKETCOMMIT_H
