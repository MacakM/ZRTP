#ifndef PACKETHELLO_H
#define PACKETHELLO_H

#include "packet.h"

//bit field
typedef struct
{
    uint8_t hc : 4, cc : 4, ac : 4, kc : 4, sc : 4;
}Counts;

class PacketHello : public Packet
{
public:
    PacketHello();

    virtual uint8_t *toBytes();

    void setClientId(std::string id);
    void setH3(uint8_t *hash);
    void setZid(uint8_t *zid);
    void setFlagS();
    void setFlagM();
    void setFlagP();

private:
    uint8_t version[VERSION_SIZE];
    uint8_t clientId[CLIENTID_SIZE];
    uint8_t h3[HASHIMAGE_SIZE];
    uint8_t zid[ZID_SIZE];
    uint8_t flags;
    Counts counts;

    uint8_t data[80];
};

#endif // PACKETHELLO_H
