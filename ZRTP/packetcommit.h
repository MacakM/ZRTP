#ifndef PACKETCOMMIT_H
#define PACKETCOMMIT_H

#include "packet.h"

class PacketCommit : public Packet
{
public:
    PacketCommit();

    virtual uint8_t *toBytes();
    virtual void parse(uint8_t *data);

    uint8_t *getHashImage();

    void setH2(uint8_t *hash);
    void setZid(uint8_t *zid);
    void setHash(uint8_t hash[4]);
    void setCipher(uint8_t cipher[4]);
    void setAuthTag(uint8_t authTag[4]);
    void setKeyAgreement(uint8_t keyAgreement[4]);
    void setSas(uint8_t sas[4]);

private:
    uint8_t h2[HASHIMAGE_SIZE];
    uint8_t zid[ZID_SIZE];

    uint8_t hash[WORD_SIZE];
    uint8_t cipher[WORD_SIZE];
    uint8_t authTag[WORD_SIZE];
    uint8_t keyAgreement[WORD_SIZE];
    uint8_t sas[WORD_SIZE];

    uint8_t hvi[HVI_SIZE];
    uint8_t mac[MAC_SIZE];

    uint8_t data[MAX_COMMIT_LENGTH];
};

#endif // PACKETCOMMIT_H
