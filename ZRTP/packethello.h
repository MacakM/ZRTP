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
     * Destructor of Hello packet.
     */
    virtual ~PacketHello();

    /**
     * Creates array of bytes from class attributes according to RFC.
     *
     * @return  array of bytes
     */
    virtual uint8_t *toBytes();

    /**
     * Parses received data into PacketHello class.
     *
     * @param data          received data
     * @param errorCode     code of occurred error in parsing
     *
     * @return              true = successful, false = error occurred
     */
    virtual bool parse(uint8_t *data, uint32_t *errorCode);

    /**
     * Returns user's ZID.
     *
     * @return ZID
     */
    uint8_t *getZid() { return zid; }

    /**
     * Returns protocol version.
     *
     * @return      version
     */
    uint8_t *getVersion() { return version; }

    /**
     * Returns client identifier.
     *
     * @return  client identifier
     */
    uint8_t *getClientId() { return clientId; }

    /**
     * Returns hash image H3.
     *
     * @return  h3
     */
    uint8_t *getH3() { return h3; }

    /**
     * Returns all supported key agreement types from hello packet.
     *
     * @return  supported key agreement types
     */
    uint8_t *getKeyAgreementTypes() { return keyAgreementTypes; }

    /**
     * Returns number of supported key agreement types.
     *
     * @return  key agreement count
     */
    uint8_t getKeyCount() { return counts.kc; }

    /**
     * Returns mac of packet.
     *
     * @return  mac
     */
    uint8_t *getMac() { return mac; }

    /**
     * Sets protocol version.
     *
     * @param version   version
     */
    void setVersion(uint8_t *version) { memcpy(this->version,version,VERSION_SIZE); }

    /**
     * Sets clientID.
     *
     * @param id   clientID
     */
    void setClientId(uint8_t *id) { memcpy(clientId,id,CLIENTID_SIZE); }

    /**
     * Sets hash image H3.
     *
     * @param hash   H3
     */
    void setH3(uint8_t *hash) { memcpy(h3,hash,HASHIMAGE_SIZE); }

    /**
     * Sets ZID.
     *
     * @param zid   ZID
     */
    void setZid(uint8_t *zid) { memcpy(this->zid,zid,ZID_SIZE); }

    /**
     * Sets Signature-capable flag
     */
    void setFlagS() { flags |= 64; }

    /**
     * Sets MiTM flag
     */
    void setFlagM() { flags |= 32; }

    /**
     * Sets Passive flag
     */
    void setFlagP() { flags |= 16; }

    /**
     * Adds all user supported algorithms and types.
     *
     * @param info  user's info
     */
    void addSupportedTypes(UserInfo *info);

    /**
     * Sets MAC of packet.
     */
    void setMac(uint8_t *mac) { memcpy(this->mac, mac, MAC_SIZE); }

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
    void addHash(uint8_t *hash);

    /**
     * Adds cipher algorithm option.
     *
     * @param cipher  cipher algorithm
     */
    void addCipher(uint8_t *cipher);

    /**
     * Adds authTag type option.
     *
     * @param authTag  authTag type
     */
    void addAuthTag(uint8_t *authTag);

    /**
     * Adds keyAgreement type option.
     *
     * @param keyAgreement  keyAgreement type
     */
    void addKeyAgreement(uint8_t *keyAgreement);

    /**
     * Adds sas type option.
     *
     * @param sas  sas type
     */
    void addSas(uint8_t *sas);
};

#endif // PACKETHELLO_H
