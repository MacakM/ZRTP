#ifndef PACKETHELLO_H
#define PACKETHELLO_H

#include "packet.h"
#include "userinfo.h"

//bit field
typedef struct
{
    uint8_t hc : 4, cc : 4, ac : 4, kc : 4, sc : 4;
}Counts;

class PacketHello : public Packet
{
public:
    /**
     * Constructor of Hello packet.
     */
    PacketHello();

    /**
     * Creates array of bytes from class attributes according to RFC.
     *
     * @return  array of bytes
     */
    virtual uint8_t *toBytes();

    /**
     * Parses received data into PacketHelloAck class.
     *
     * @param data  received data
     * @return      true = successful, false = otherwise
     */
    virtual bool parse(uint8_t *data);

    /**
     * Returns user's ZID.
     *
     * @return ZID
     */
    uint8_t *getZid();

    /**
     * Returns protocol version.
     *
     * @return      version
     */
    uint8_t *getVersion();

    /**
     * Sets protocol version.
     *
     * @param version   version
     */
    void setVersion(uint8_t *version);

    /**
     * Sets clientID.
     *
     * @param id   clientID
     */
    void setClientId(std::string id);

    /**
     * Sets hash image H3.
     *
     * @param hash   H3
     */
    void setH3(uint8_t *hash);

    /**
     * Sets ZID.
     *
     * @param zid   ZID
     */
    void setZid(uint8_t *zid);

    /**
     * Sets Signature-capable flag
     */
    void setFlagS();

    /**
     * Sets MiTM flag
     */
    void setFlagM();

    /**
     * Sets Passive flag
     */
    void setFlagP();

    /**
     * Adds all user supported algorithms and types.
     *
     * @param info  user's info
     */
    void AddSupportedTypes(UserInfo *info);

    /**
     * Sets MAC of packet.
     */
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


    /**
     * Adds hash algorithm option.
     *
     * @param hash  hash algorithm
     */
    void addHash(uint8_t hash[4]);

    /**
     * Adds cipher algorithm option.
     *
     * @param cipher  cipher algorithm
     */
    void addCipher(uint8_t cipher[4]);

    /**
     * Adds authTag type option.
     *
     * @param authTag  authTag type
     */
    void addAuthTag(uint8_t authTag[4]);

    /**
     * Adds keyAgreement type option.
     *
     * @param keyAgreement  keyAgreement type
     */
    void addKeyAgreement(uint8_t keyAgreement[4]);

    /**
     * Adds sas type option.
     *
     * @param sas  sas type
     */
    void addSas(uint8_t sas[4]);
};

#endif // PACKETHELLO_H
