#ifndef PACKETDHPART_H
#define PACKETDHPART_H

#define DH3K_LENGTH 96 * WORD_SIZE

#include "packet.h"

class PacketDHPart : public Packet
{
public:
    PacketDHPart();

    virtual uint8_t *toBytes();
    virtual void parse(uint8_t *data);

    void setH1(uint8_t *hash);
    void setRs1Id(uint8_t *rs1Id);
    void setRs2Id(uint8_t *rs2Id);
    void setAuxsecretId(uint8_t *auxsecretId);
    void setPbxsecretId(uint8_t *pbxsecretId);
    void setPv(uint8_t publicValue[]);

    void setMac(uint8_t mac[MAC_SIZE]);
private:
    uint8_t h1[HASHIMAGE_SIZE];
    uint8_t rs1Id[ID_SIZE];
    uint8_t rs2Id[ID_SIZE];
    uint8_t auxsecretId[ID_SIZE];
    uint8_t pbxsecretId[ID_SIZE];

    uint8_t pv[DH3K_LENGTH];

    uint8_t mac[MAC_SIZE];

    uint8_t data[MAX_DHPART_LENGTH];
};

#endif // PACKETDHPART_H
