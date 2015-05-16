#ifndef PACKETCOMMIT_H
#define PACKETCOMMIT_H

#include "packet.h"

class PacketCommit : public Packet
{
public:
    /**
     * Constructor of Commit packet.
     */
    PacketCommit();

    /**
     * Destructor of Commit packet.
     */
    virtual ~PacketCommit();

    /**
     * Creates array of bytes from class attributes according to RFC.
     *
     * @return  array of bytes
     */
    virtual uint8_t *toBytes();

    /**
     * Parses received data into PacketCommit class.
     *
     * @param data          received data
     * @param errorCode     code of occurred error in parsing
     *
     * @return              true = successful, false = error occurred
     */
    virtual bool parse(uint8_t *data, uint32_t *errorCode);

    /**
     * Returns whether this packet has greater hvi than peer commit packet.
     * Important when choosing Initiator from two Initiator endpoints.
     *
     * @param packet    peer commit packet
     *
     * @return          true = this packet has greater hvi, false = lower hvi
     */
    bool hasGreaterHvi(PacketCommit *packet);

    /**
     * Returns hvi.
     *
     * @return  hvi
     */
    uint8_t *getHvi() { return hvi; }

    /**
     * Returns hash image H2.
     *
     * @return  h2
     */
    uint8_t *getH2() { return h2; }

    /**
     * Returns chosen hash algorithm.
     *
     * @return  hash algorithm
     */
    uint8_t *getHash() { return hash; }

    /**
     * Returns chosen cipher algorithm.
     *
     * @return  cipher algorithm
     */
    uint8_t *getCipher() { return cipher; }

    /**
     * Returns chosen authTag type.
     *
     * @return  authTag type
     */
    uint8_t *getAuthTag() { return authTag; }

    /**
     * Returns chosen key agreement type.
     *
     * @return  key agreement type
     */
    uint8_t *getKeyAgreement() { return keyAgreement; }

    /**
     * Returns chosen SAS type.
     *
     * @return  SAS type
     */
    uint8_t *getSas() { return sas; }

    /**
     * Returns mac of packet.
     *
     * @return  mac
     */
    uint8_t *getMac() { return mac; }

    /**
     * Sets hash image H2.
     *
     * @param hash  H2
     */
    void setH2(uint8_t *hash) { memcpy(h2,hash,HASHIMAGE_SIZE); }

    /**
     * Sets user's ZID.
     *
     * @param zid   ZID
     */
    void setZid(uint8_t *zid) { memcpy(this->zid,zid,ZID_SIZE); }

    /**
     * Sets chosen hash algorithm.
     *
     * @param hash  hash algorithm
     */
    void setHash(uint8_t *hash) { memcpy(this->hash,hash, WORD_SIZE); }

    /**
     * Sets chosen cipher algorithm.
     *
     * @param cipher  cipher algorithm
     */
    void setCipher(uint8_t *cipher) { memcpy(this->cipher, cipher, WORD_SIZE); }

    /**
     * Sets chosen authTag type.
     *
     * @param authTag  authTag type
     */
    void setAuthTag(uint8_t *authTag) { memcpy(this->authTag, authTag, WORD_SIZE); }

    /**
     * Sets chosen keyAgreement type.
     *
     * @param keyAgreement  keyAgreement type
     */
    void setKeyAgreement(uint8_t *keyAgreement) { memcpy(this->keyAgreement, keyAgreement, WORD_SIZE); }

    /**
     * Sets chosen SAS type.
     *
     * @param sas  SAS type
     */
    void setSas(uint8_t *sas) { memcpy(this->sas, sas, WORD_SIZE); }

    /**
     * Sets calculated hvi.
     * @param hvi   hvi
     */
    void setHvi(uint8_t *hvi) { memcpy(this->hvi, hvi, HVI_SIZE); }

    /**
     * Sets MAC of packet.
     */
    virtual void setMac(uint8_t *mac) { memcpy(this->mac, mac, MAC_SIZE); }
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
