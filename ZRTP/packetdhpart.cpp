#include "packetdhpart.h"

PacketDHPart::PacketDHPart()
{
    packetHeader = new Header();
    setZrtpIdentifier();
    setLength(((sizeof(Header) + sizeof(uint8_t[HASHIMAGE_SIZE]) + 4 * sizeof(uint8_t[ID_SIZE]) +
                sizeof(uint8_t[DH3K_LENGTH]) + sizeof(uint8_t[MAC_SIZE])) / WORD_SIZE) - 1);
}

uint8_t *PacketDHPart::toBytes()
{
    uint8_t *pos = data;

    *pos = packetHeader->identifier >> 8;
    pos++;
    *pos = packetHeader->identifier;
    pos++;

    *pos = packetHeader->length >> 8;
    pos++;
    *pos = packetHeader->length;

    for (uint8_t i = 0; i < TYPE_SIZE; i++)
    {
        *(++pos) = packetHeader->type[i];
    }
    for (uint8_t i = 0; i < HASHIMAGE_SIZE; i++)
    {
        *(++pos) = h1[i];
    }
    for (uint8_t i = 0; i < ID_SIZE; i++)
    {
        *(++pos) = rs1Id[i];
    }
    for (uint8_t i = 0; i < ID_SIZE; i++)
    {
        *(++pos) = rs2Id[i];
    }
    for (uint8_t i = 0; i < ID_SIZE; i++)
    {
        *(++pos) = auxsecretId[i];
    }
    for (uint8_t i = 0; i < ID_SIZE; i++)
    {
        *(++pos) = pbxsecretId[i];
    }
    for (uint16_t i = 0; i < DH3K_LENGTH; i++)
    {
        *(++pos) = pv[i];
    }
    for(uint8_t i = 0; i < MAC_SIZE; i++)
    {
        *(++pos) = mac[i];
    }
    return data;
}

void PacketDHPart::parse(uint8_t *data)
{

}

uint8_t *PacketDHPart::getHashImage()
{
    return h1;
}

void PacketDHPart::setH1(uint8_t *hash)
{
    memcpy(h1,hash,HASHIMAGE_SIZE);
}

void PacketDHPart::setRs1Id(uint8_t *rs1Id)
{
    memcpy(this->rs1Id,rs1Id,ID_SIZE);
}

void PacketDHPart::setRs2Id(uint8_t *rs2Id)
{
    memcpy(this->rs2Id,rs2Id,ID_SIZE);
}

void PacketDHPart::setAuxsecretId(uint8_t *auxsecretId)
{
    memcpy(this->auxsecretId,auxsecretId,ID_SIZE);
}

void PacketDHPart::setPbxsecretId(uint8_t *pbxsecretId)
{
    memcpy(this->pbxsecretId,pbxsecretId,ID_SIZE);
}

void PacketDHPart::setPv(uint8_t publicValue[])
{
    memcpy(pv,publicValue,DH3K_LENGTH);
}

void PacketDHPart::setMac(uint8_t mac[])
{
    memcpy(this->mac, mac, MAC_SIZE);
}
