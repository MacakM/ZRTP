#include "packetconfirm.h"

PacketConfirm::PacketConfirm()
{
    packetHeader = new Header();
    setZrtpIdentifier();
    setLength(19);  //Confirm length without signature
    flags = 0;
    encryptionDone = false;
}

PacketConfirm::~PacketConfirm()
{
    delete (packetHeader);
    memset(data,0,MAX_CONFIRM_LENGTH);
}

uint8_t *PacketConfirm::toBytes()
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
    for (uint8_t i = 0; i < MAC_SIZE; i++)
    {
        *(++pos) = confirmMac[i];
    }
    for (uint8_t i = 0; i < VECTOR_SIZE; i++)
    {
        *(++pos) = initVector[i];
    }
    if(encryptionDone)
    {
        for(uint8_t i = 0; i < 40; i++)
        {
            *(++pos) = encryptedPart[i];
        }
    }
    else    //not encrypted yet
    {
        for (uint8_t i = 0; i < HASHIMAGE_SIZE; i++)
        {
            *(++pos) = h0[i];
        }
        *(++pos) = 0;

        *(++pos) = sigLen >> 8;
        *(++pos) = sigLen >> 0;

        *(++pos) = flags;

        *(++pos) = expInterval >> 24;
        *(++pos) = expInterval >> 16;
        *(++pos) = expInterval >> 8;
        *(++pos) = expInterval;
    }

    return data;
}

bool PacketConfirm::parse(uint8_t *data, uint32_t *errorCode)
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
    for(uint8_t i = 0; i < MAC_SIZE; i++)
    {
        confirmMac[i] = *(pos++);
    }
    for(uint8_t i = 0; i < VECTOR_SIZE; i++)
    {
        initVector[i] = *(pos++);
    }
    for(uint8_t i = 0; i < ENCRYPTED_PART_LENGTH; i++)
    {
        encryptedPart[i] = *(pos++);
    }
    encryptionDone = true;

    return true;
}

void PacketConfirm::setEncryptedPart(uint8_t *data)
{
    memcpy(encryptedPart,data,ENCRYPTED_PART_LENGTH);
    encryptionDone = true;
}
