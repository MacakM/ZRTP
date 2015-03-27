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
     * Creates array of bytes from class attributes according to RFC.
     *
     * @return  array of bytes
     */
    virtual uint8_t *toBytes();

    /**
     * Parses received data into PacketConfirm class.
     *
     * @param data  received data
     * @return      true = successful, false = otherwise
     */
    virtual bool parse(uint8_t *data);

    /**
     * Gets initialization vector.
     *
     * @return  IV
     */
    uint8_t *getVector();

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
    void setConfirmMac(uint8_t *mac);

    /**
     * Sets initialization vector.
     *
     * @param vector    IV
     */
    void setInitVector(uint8_t *vector);

    /**
     * Sets hash image H0
     *
     * @param hash  H0
     */
    void setH0(uint8_t *hash);

    /**
     * Sets signature length.
     *
     * @param length    signature length
     */
    void setSigLen(uint16_t length);

    /**
     * Sets PBX Enrollment flag.
     */
    void setFlagE();

    /**
     * Sets SAS Verified flag.
     */
    void setFlagV();

    /**
     * Sets Allow Clear flag.
     */
    void setFlagA();

    /**
     * Sets Disclosure flag.
     */
    void setFlagD();

    /**
     * Sets expiration interval.
     *
     * @param interval  expiration interval
     */
    void setExpInterval(uint32_t interval);

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
