#ifndef PACKETHELLO_H
#define PACKETHELLO_H

#include "packet.h"

class PacketHello : public Packet
{
public:
    PacketHello();

    virtual uint8_t *toBytes();

    void setClientId(std::string id);

private:
    uint8_t clientId[CLIENTID_SIZE];

    uint8_t data[20];
};

#endif // PACKETHELLO_H
