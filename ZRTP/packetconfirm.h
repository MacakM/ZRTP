#ifndef PACKETCONFIRM_H
#define PACKETCONFIRM_H

#include "packet.h"

class PacketConfirm : public Packet
{
public:
    PacketConfirm();

    virtual uint8_t *toBytes();

private:
    uint8_t data[20];
};

#endif // PACKETCONFIRM_H
