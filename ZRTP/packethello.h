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
    virtual void parse(uint8_t *data);

    uint8_t *getHashImage();

    void setVersion(uint8_t *version);
    void setClientId(std::string id);
    void setH3(uint8_t *hash);
    void setZid(uint8_t *zid);
    void setFlagS();
    void setFlagM();
    void setFlagP();

    void addHash(uint8_t hash[4]);
    void addCipher(uint8_t cipher[4]);
    void addAuthTag(uint8_t authTag[4]);
    void addKeyAgreement(uint8_t keyAgreement[4]);
    void addSas(uint8_t sas[4]);

    void setMac(uint8_t mac[MAC_SIZE]);

private:
    uint8_t version[VERSION_SIZE];
    uint8_t clientId[CLIENTID_SIZE];
    uint8_t h3[HASHIMAGE_SIZE];
    uint8_t zid[ZID_SIZE];
    uint8_t flags;
    Counts counts;
    uint8_t mac[MAC_SIZE];

    uint8_t hashTypes[WORD_SIZE * 7];
    uint8_t cipherTypes[WORD_SIZE * 7];
    uint8_t authTagTypes[WORD_SIZE * 7];
    uint8_t keyAgreementTypes[WORD_SIZE * 7];
    uint8_t sasTypes[WORD_SIZE * 7];

    uint8_t data[MAX_HELLO_LENGTH];
};

#endif // PACKETHELLO_H
