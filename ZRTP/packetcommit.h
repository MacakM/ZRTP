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
     * Returns hvi.
     *
     * @return  hvi
     */
    uint8_t *getHvi();

    /**
     * Returns hash image H2.
     *
     * @return  h2
     */
    uint8_t *getH2();

    /**
     * Returns chosen hash algorithm.
     *
     * @return  hash algorithm
     */
    uint8_t *getHash();

    /**
     * Returns chosen cipher algorithm.
     *
     * @return  cipher algorithm
     */
    uint8_t *getCipher();

    /**
     * Returns chosen authTag type.
     *
     * @return  authTag type
     */
    uint8_t *getAuthTag();

    /**
     * Returns chosen key agreement type.
     *
     * @return  key agreement type
     */
    uint8_t *getKeyAgreement();

    /**
     * Returns chosen SAS type.
     *
     * @return  SAS type
     */
    uint8_t *getSas();

    /**
     * Returns mac of packet.
     *
     * @return  mac
     */
    uint8_t *getMac();

    /**
     * Sets hash image H2.
     *
     * @param hash  H2
     */
    void setH2(uint8_t *hash);

    /**
     * Sets user's ZID.
     *
     * @param zid   ZID
     */
    void setZid(uint8_t *zid);

    /**
     * Sets chosen hash algorithm.
     *
     * @param hash  hash algorithm
     */
    void setHash(uint8_t *hash);

    /**
     * Sets chosen cipher algorithm.
     *
     * @param cipher  cipher algorithm
     */
    void setCipher(uint8_t *cipher);

    /**
     * Sets chosen authTag type.
     *
     * @param authTag  authTag type
     */
    void setAuthTag(uint8_t *authTag);

    /**
     * Sets chosen keyAgreement type.
     *
     * @param keyAgreement  keyAgreement type
     */
    void setKeyAgreement(uint8_t *keyAgreement);

    /**
     * Sets chosen SAS type.
     *
     * @param sas  SAS type
     */
    void setSas(uint8_t *sas);

    /**
     * Sets calculated hvi.
     * @param hvi   hvi
     */
    void setHvi(uint8_t *hvi);

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
     * Sets MAC of packet.
     */
    virtual void setMac(uint8_t *mac);
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
