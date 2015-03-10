#ifndef PACKETDHPART_H
#define PACKETDHPART_H

#include "packet.h"

class PacketDHPart : public Packet
{
public:
    PacketDHPart();

    virtual uint8_t *toBytes();
    virtual void parse(uint8_t *data);

    uint8_t *getHashImage();

    void setH1(uint8_t *hash);
    void setRs1Id(uint8_t *rs1Id);
    void setRs2Id(uint8_t *rs2Id);
    void setAuxsecretId(uint8_t *auxsecretId);
    void setPbxsecretId(uint8_t *pbxsecretId);

private:
    uint8_t h1[HASHIMAGE_SIZE];
    uint8_t rs1Id[ID_SIZE];
    uint8_t rs2Id[ID_SIZE];
    uint8_t auxsecretId[ID_SIZE];
    uint8_t pbxsecretId[ID_SIZE];

    uint8_t *pv;

    uint8_t mac[MAC_SIZE];

    uint8_t data[20];
};

#endif // PACKETDHPART_H
