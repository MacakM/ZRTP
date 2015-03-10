#include "packetcommit.h"

PacketCommit::PacketCommit()
{
    packetHeader = new Header();
    setZrtpIdentifier();
    setType((uint8_t*)"Commit  ");
    setLength(29);

}

uint8_t *PacketCommit::toBytes()
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
    for(uint8_t i = 0; i < HASHIMAGE_SIZE; i++)
    {
        *(++pos) = h2[i];
    }
    for(uint8_t i = 0; i < ZID_SIZE; i++)
    {
        *(++pos) = zid[i];
    }
    for(uint8_t i = 0; i < 4; i++)
    {
        *(++pos) = hash[i];
    }
    for(uint8_t i = 0; i < 4; i++)
    {
        *(++pos) = cipher[i];
    }
    for(uint8_t i = 0; i < 4; i++)
    {
        *(++pos) = authTag[i];
    }
    for(uint8_t i = 0; i < 4; i++)
    {
        *(++pos) = keyAgreement[i];
    }
    for(uint8_t i = 0; i < 4; i++)
    {
        *(++pos) = sas[i];
    }

    return data;
}

void PacketCommit::parse(uint8_t *data)
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
    setType((uint8_t*)"Commit  ");
    pos += 10;
    for(uint8_t i = 0; i < HASHIMAGE_SIZE; i++)
    {
        h2[i] = *(pos++);
    }
    for(uint8_t i = 0; i < ZID_SIZE; i++)
    {
        zid[i] = *(pos++);
    }
    for(uint8_t i = 0; i < 4; i++)
    {
        hash[i] = *(pos++);
    }
    for(uint8_t i = 0; i < 4; i++)
    {
        cipher[i] = *(pos++);
    }
    for(uint8_t i = 0; i < 4; i++)
    {
        authTag[i] = *(pos++);
    }
    for(uint8_t i = 0; i < 4; i++)
    {
        keyAgreement[i] = *(pos++);
    }
    for(uint8_t i = 0; i < 4; i++)
    {
        sas[i] = *(pos++);
    }
}

uint8_t *PacketCommit::getHashImage()
{
    return h2;
}

void PacketCommit::setH2(uint8_t *hash)
{
    memcpy(h2,hash,HASHIMAGE_SIZE);
}

void PacketCommit::setZid(uint8_t *zid)
{
    memcpy(this->zid,zid,ZID_SIZE);
}

void PacketCommit::setHash(uint8_t hash[])
{
    memcpy(this->hash,hash,HASHIMAGE_SIZE);
}

void PacketCommit::setCipher(uint8_t cipher[])
{
    memcpy(this->cipher, cipher, WORD_SIZE);
}

void PacketCommit::setAuthTag(uint8_t authTag[])
{
    memcpy(this->authTag, authTag, WORD_SIZE);
}

void PacketCommit::setKeyAgreement(uint8_t keyAgreement[])
{
    memcpy(this->keyAgreement, keyAgreement, WORD_SIZE);
}

void PacketCommit::setSas(uint8_t sas[])
{
    memcpy(this->sas, sas, WORD_SIZE);
}
