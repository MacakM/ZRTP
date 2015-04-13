#include "packetcommit.h"

PacketCommit::PacketCommit()
{
    packetHeader = new Header();
    setZrtpIdentifier();
    setType((uint8_t*)"Commit  ");
    setLength(29);
}

PacketCommit::~PacketCommit()
{
    std::cout << "Commit destructing" << std::endl;
    delete (packetHeader);
    memset(data,0,MAX_COMMIT_LENGTH);
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
    for(uint8_t i = 0; i < WORD_SIZE; i++)
    {
        *(++pos) = hash[i];
    }
    for(uint8_t i = 0; i < WORD_SIZE; i++)
    {
        *(++pos) = cipher[i];
    }
    for(uint8_t i = 0; i < WORD_SIZE; i++)
    {
        *(++pos) = authTag[i];
    }
    for(uint8_t i = 0; i < WORD_SIZE; i++)
    {
        *(++pos) = keyAgreement[i];
    }
    for(uint8_t i = 0; i < WORD_SIZE; i++)
    {
        *(++pos) = sas[i];
    }
    for(uint8_t i = 0; i < HVI_SIZE; i++)
    {
        *(++pos) = hvi[i];
    }
    for(uint8_t i = 0; i < MAC_SIZE; i++)
    {
        *(++pos) = mac[i];
    }

    return data;
}

bool PacketCommit::parse(uint8_t *data, uint32_t *errorCode)
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
    //type has been checked in StateEngine
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
    for(uint8_t i = 0; i < WORD_SIZE; i++)
    {
        hash[i] = *(pos++);
    }
    for(uint8_t i = 0; i < WORD_SIZE; i++)
    {
        cipher[i] = *(pos++);
    }
    for(uint8_t i = 0; i < WORD_SIZE; i++)
    {
        authTag[i] = *(pos++);
    }
    for(uint8_t i = 0; i < WORD_SIZE; i++)
    {
        keyAgreement[i] = *(pos++);
    }
    for(uint8_t i = 0; i < WORD_SIZE; i++)
    {
        sas[i] = *(pos++);
    }
    for(uint8_t i = 0; i < HVI_SIZE; i++)
    {
        hvi[i] = *(pos++);
    }
    for(uint8_t i = 0; i < MAC_SIZE; i++)
    {
        mac[i] = *(pos++);
    }
    return true;
}

uint8_t *PacketCommit::getHvi()
{
    return hvi;
}

uint8_t *PacketCommit::getH2()
{
    return h2;
}

uint8_t *PacketCommit::getHash()
{
    return hash;
}

uint8_t *PacketCommit::getCipher()
{
    return cipher;
}

uint8_t *PacketCommit::getAuthTag()
{
    return authTag;
}

uint8_t *PacketCommit::getKeyAgreement()
{
    return keyAgreement;
}

uint8_t *PacketCommit::getSas()
{
    return sas;
}

uint8_t *PacketCommit::getMac()
{
    return mac;
}

void PacketCommit::setH2(uint8_t *hash)
{
    memcpy(h2,hash,HASHIMAGE_SIZE);
}

void PacketCommit::setZid(uint8_t *zid)
{
    memcpy(this->zid,zid,ZID_SIZE);
}

void PacketCommit::setHash(uint8_t *hash)
{
    memcpy(this->hash,hash, WORD_SIZE);
}

void PacketCommit::setCipher(uint8_t *cipher)
{
    memcpy(this->cipher, cipher, WORD_SIZE);
}

void PacketCommit::setAuthTag(uint8_t *authTag)
{
    memcpy(this->authTag, authTag, WORD_SIZE);
}

void PacketCommit::setKeyAgreement(uint8_t *keyAgreement)
{
    memcpy(this->keyAgreement, keyAgreement, WORD_SIZE);
}

void PacketCommit::setSas(uint8_t *sas)
{
    memcpy(this->sas, sas, WORD_SIZE);
}

void PacketCommit::setHvi(uint8_t *hvi)
{
    memcpy(this->hvi, hvi, HVI_SIZE);
}

bool PacketCommit::hasGreaterHvi(PacketCommit *packet)
{
    if(memcmp(this->hvi,packet->getHvi(),HVI_SIZE) > 0)
    {
        return true;
    }
    return false;
}

void PacketCommit::setMac(uint8_t *mac)
{
    memcpy(this->mac, mac, MAC_SIZE);
}
