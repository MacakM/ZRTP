#ifndef PACKETDHPART_H
#define PACKETDHPART_H

#define DH3K_LENGTH 96 * WORD_SIZE

#include "packet.h"

class PacketDHPart : public Packet
{
public:
    /**
     * Constructor of DHPart packet.
     */
    PacketDHPart();

    /**
     * Destructor of DHPart packet.
     */
    virtual ~PacketDHPart();

    /**
     * Creates array of bytes from class attributes according to RFC.
     *
     * @return  array of bytes
     */
    virtual uint8_t *toBytes();

    /**
     * Parses received data into PacketDHPart class.
     *
     * @param data          received data
     * @param errorCode     code of occurred error in parsing
     *
     * @return              true = successful, false = error occurred
     */
    virtual bool parse(uint8_t *data, uint32_t *errorCode);

    /**
     * Returns public key.
     *
     * @return  public key
     */
    uint8_t *getPv() { return pv; }

    /**
     * Returns hash image H1.
     *
     * @return  h1
     */
    uint8_t *getH1() { return h1; }

    /**
     * Returns mac of packet.
     *
     * @return  mac
     */
    uint8_t *getMac() { return mac; }

    /**
     * Sets hash image H1.
     *
     * @param hash  h1
     */
    void setH1(uint8_t *hash) { memcpy(h1,hash,HASHIMAGE_SIZE); }

    /**
     * Sets rs1ID.
     *
     * @param rs1ID  rs1ID
     */
    void setRs1Id(uint8_t *rs1Id) { memcpy(this->rs1Id,rs1Id,ID_SIZE); }

    /**
     * Sets rs2Id.
     *
     * @param rs2Id  rs2Id
     */
    void setRs2Id(uint8_t *rs2Id) { memcpy(this->rs2Id,rs2Id,ID_SIZE); }

    /**
     * Sets auxsecretId.
     *
     * @param auxsecretId  auxsecretId
     */
    void setAuxsecretId(uint8_t *auxsecretId) { memcpy(this->auxsecretId,auxsecretId,ID_SIZE); }

    /**
     * Sets pbxsecretId.
     *
     * @param pbxsecretId  pbxsecretId
     */
    void setPbxsecretId(uint8_t *pbxsecretId) { memcpy(this->pbxsecretId,pbxsecretId,ID_SIZE); }

    /**
     * Sets public key.
     *
     * @param publicValue   public key
     */
    void setPv(uint8_t *publicValue) { memcpy(pv,publicValue,DH3K_LENGTH); }

    /**
     * Sets MAC of packet.
     */
    void setMac(uint8_t *mac) { memcpy(this->mac, mac, MAC_SIZE); }
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
