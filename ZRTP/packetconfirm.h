#ifndef PACKETCONFIRM_H
#define PACKETCONFIRM_H

#include "packet.h"

#define ENCRYPTED_PART_LENGTH   40

class PacketConfirm : public Packet
{
public:
    /**
     * Constructor of Confirm packet.
     */
    PacketConfirm();

    /**
     * Destructor of Confirm packet.
     */
    virtual ~PacketConfirm();

    /**
     * Creates array of bytes from class attributes according to RFC.
     *
     * @return  array of bytes
     */
    virtual uint8_t *toBytes();

    /**
     * Parses received data into PacketConfirm class.
     *
     * @param data          received data
     * @param errorCode     code of occurred error in parsing
     *
     * @return              true = successful, false = error occurred
     */
    virtual bool parse(uint8_t *data, uint32_t *errorCode);

    /**
     * Returns confirm mac.
     *
     * @return  confirm mac
     */
    uint8_t *getConfirmMac() { return confirmMac; }

    /**
     * Gets initialization vector.
     *
     * @return  IV
     */
    uint8_t *getVector() { return initVector; }

    /**
     * Returns hash image H0.
     *
     * @return  h0
     */
    uint8_t *getH0() { return h0; }

    /**
     * Returns encrypted part of the packet.
     *
     * @return  encrypted data
     */
    uint8_t *getEncryptedPart() { return encryptedPart; }

    /**
     * Sets encrypted part of packet instead of decrypted part.
     *
     * @param data  encrypted data
     */
    void setEncryptedPart(uint8_t *data);

    /**
     * Sets confirm mac
     *
     * @param mac   MAC
     */
    void setConfirmMac(uint8_t *mac) { memcpy(confirmMac,mac,MAC_SIZE); }

    /**
     * Sets initialization vector.
     *
     * @param vector    IV
     */
    void setInitVector(uint8_t *vector) { memcpy(initVector,vector,VECTOR_SIZE); }

    /**
     * Sets hash image H0
     *
     * @param hash  H0
     */
    void setH0(uint8_t *hash) { memcpy(h0,hash,HASHIMAGE_SIZE); }

    /**
     * Sets signature length.
     *
     * @param length    signature length
     */
    void setSigLen(uint16_t length) { sigLen = length; }

    /**
     * Sets PBX Enrollment flag.
     */
    void setFlagE() { flags |= 8; }

    /**
     * Sets SAS Verified flag.
     */
    void setFlagV() { flags |= 4; }

    /**
     * Sets Allow Clear flag.
     */
    void setFlagA() { flags |= 2; }

    /**
     * Sets Disclosure flag.
     */
    void setFlagD() { flags |= 1; }

    /**
     * Sets expiration interval.
     *
     * @param interval  expiration interval
     */
    void setExpInterval(uint32_t interval) { expInterval = interval; }

private:
    bool encryptionDone;
    uint8_t confirmMac[MAC_SIZE];
    uint8_t initVector[VECTOR_SIZE];
    uint8_t h0[HASHIMAGE_SIZE];
    uint16_t sigLen;
    uint8_t flags;
    uint32_t expInterval;
    uint8_t encryptedPart[ENCRYPTED_PART_LENGTH];

    uint8_t data[MAX_CONFIRM_LENGTH];
};

#endif // PACKETCONFIRM_H
