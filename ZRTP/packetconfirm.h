#ifndef PACKETCONFIRM_H
#define PACKETCONFIRM_H

#include "packet.h"

class PacketConfirm : public Packet
{
public:
    PacketConfirm();

    virtual uint8_t *toBytes();
    virtual void parse(uint8_t *data);

private:
    uint8_t mac[MAC_SIZE];
    uint8_t iVector[VECTOR_SIZE];
    uint8_t h0[HASHIMAGE_SIZE];
    uint16_t sigLen;
    uint8_t flags;

    //optional signature type block?

    uint8_t data[20];
};

#endif // PACKETCONFIRM_H
