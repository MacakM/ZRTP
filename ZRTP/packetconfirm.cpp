#include "packetconfirm.h"

PacketConfirm::PacketConfirm()
{
    packetHeader = new Header();
    setZrtpIdentifier();
    setLength(19);
    flags = 0;
    encryptionDone = false;
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
    else
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

bool PacketConfirm::parse(uint8_t *data)
{
    uint8_t *pos = data;

    packetHeader->identifier = *pos << 8 | *(pos + 1);
    pos += 2;

    uint16_t zrtpId = (uint16_t)ZRTP_IDENTIFIER;
    if(memcmp(&packetHeader->identifier,&zrtpId,WORD_SIZE) != 0)
    {
        std::cout << "CHYBA";
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
    return true;
}

uint8_t *PacketConfirm::getVector()
{
    return initVector;
}

void PacketConfirm::setEncryptedPart(uint8_t data[])
{
    memcpy(encryptedPart,data,40);
    encryptionDone = true;
}

void PacketConfirm::setConfirmMac(uint8_t mac[])
{
    memcpy(confirmMac,mac,MAC_SIZE);
}

void PacketConfirm::setInitVector(uint8_t vector[])
{
    memcpy(initVector,vector,VECTOR_SIZE);
}

void PacketConfirm::setH0(uint8_t *hash)
{
    memcpy(h0,hash,HASHIMAGE_SIZE);
}

void PacketConfirm::setSigLen(uint16_t length)
{
    sigLen = length;
}

void PacketConfirm::setFlagE()
{
    flags |= 8;
}

void PacketConfirm::setFlagV()
{
    flags |= 4;
}

void PacketConfirm::setFlagA()
{
    flags |= 2;
}

void PacketConfirm::setFlagD()
{
    flags |= 1;
}

void PacketConfirm::setExpInterval(uint32_t interval)
{
    expInterval = interval;
}
