#ifndef PACKETCONFIRM_H
#define PACKETCONFIRM_H

#include "packet.h"

class PacketConfirm : public Packet
{
public:
    PacketConfirm();

    virtual uint8_t *toBytes();
    virtual void parse(uint8_t *data);

    void setConfirmMac(uint8_t mac[MAC_SIZE]);
    void setInitVector(uint8_t vector[VECTOR_SIZE]);
    void setH0(uint8_t *hash);
    void setSigLen(uint16_t length);
    void setFlagE();
    void setFlagV();
    void setFlagA();
    void setFlagD();
    void setExpInterval(uint32_t interval);

private:
    uint8_t confirmMac[MAC_SIZE];
    uint8_t initVector[VECTOR_SIZE];
    uint8_t h0[HASHIMAGE_SIZE];
    uint16_t sigLen;
    uint8_t flags;
    uint32_t expInterval;

    uint8_t data[MAX_CONFIRM_LENGTH];
};

#endif // PACKETCONFIRM_H
