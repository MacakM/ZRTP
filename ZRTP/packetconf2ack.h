#ifndef PACKETCONF2ACK_H
#define PACKETCONF2ACK_H

#include "packet.h"

class PacketConf2Ack : public Packet
{
public:
    PacketConf2Ack();

    virtual uint8_t *toBytes();

private:
    uint8_t data[20];
};

#endif // PACKETCONF2ACK_H
