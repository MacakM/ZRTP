#include "packetdhpart.h"

PacketDHPart::PacketDHPart()
{
    packetHeader = new Header();
    setZrtpIdentifier();
    setLength(((sizeof(Header) + sizeof(uint8_t[HASHIMAGE_SIZE]) + 4 * sizeof(uint8_t[ID_SIZE]) +
                sizeof(uint8_t[DH3K_LENGTH]) + sizeof(uint8_t[MAC_SIZE])) / WORD_SIZE) - 1);
}

PacketDHPart::~PacketDHPart()
{
    delete (packetHeader);
    memset(data,0,MAX_DHPART_LENGTH);
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

bool PacketDHPart::parse(uint8_t *data, uint32_t *errorCode)
{
    uint8_t *pos = data;

    packetHeader->identifier = *pos << 8 | *(pos + 1);
    pos += 2;

    uint16_t zrtpId = (uint16_t)ZRTP_IDENTIFIER;
    if(memcmp(&packetHeader->identifier,&zrtpId,WORD_SIZE) != 0)
    {
        *errorCode = MalformedPacket;
        return false;
    }

    packetHeader->length = *pos << 8 | *(pos + 1);

    pos += 2;
    for(uint8_t i = 0; i < TYPE_SIZE; i++)
    {
        packetHeader->type[i] = *(pos++);
    }

    for(uint8_t i = 0; i < HASHIMAGE_SIZE; i++)
    {
        h1[i] = *(pos++);
    }
    for(uint8_t i = 0; i < ID_SIZE; i++)
    {
        rs1Id[i] = *(pos++);
    }
    for(uint8_t i = 0; i < ID_SIZE; i++)
    {
        rs2Id[i] = *(pos++);
    }
    for(uint8_t i = 0; i < ID_SIZE; i++)
    {
        auxsecretId[i] = *(pos++);
    }
    for(uint8_t i = 0; i < ID_SIZE; i++)
    {
        pbxsecretId[i] = *(pos++);
    }
    for(uint16_t i = 0; i < DH3K_LENGTH; i++)
    {
        pv[i] = *(pos++);
    }

    for(uint8_t i = 0; i < MAC_SIZE; i++)
    {
        mac[i] = *(pos++);
    }
    return true;
}
