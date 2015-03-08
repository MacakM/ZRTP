#include "packethello.h"

PacketHello::PacketHello()
{
    packetHeader = new Header();
    setZrtpIdentifier();
    setType((uint8_t*)"Hello   ");
    setLength(((sizeof(Header) + sizeof(uint8_t[CLIENTID_SIZE]) +
                sizeof(uint8_t[HASHIMAGE_SIZE]) + sizeof(uint8_t[ZID_SIZE]) +
                sizeof(uint8_t) + sizeof(uint8_t[VERSION_SIZE]) +
                sizeof(Counts) + sizeof(uint8_t[MAC_SIZE]))/ WORD_SIZE) - 1);
    flags = 0;
    counts.hc = 0;
    counts.cc = 0;
    counts.ac = 0;
    counts.kc = 0;
    counts.sc = 0;
}

uint8_t *PacketHello::toBytes()
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
    for (uint8_t i = 0; i < VERSION_SIZE; i++)
    {
        *(++pos) = version[i];
    }

    for(uint8_t i = 0; i < CLIENTID_SIZE; i++)
    {
        *(++pos) = clientId[i];
    }
    for(uint8_t i = 0; i < HASHIMAGE_SIZE; i++)
    {
        *(++pos) = h3[i];
    }
    for(uint8_t i = 0; i < ZID_SIZE; i++)
    {
        *(++pos) = zid[i];
    }
    *(++pos) = flags;

    *(++pos) = 0x0000 | counts.hc;
    *(++pos) = 0x0000 | counts.cc << 4 | counts.ac;
    *(++pos) = 0x0000 | counts.kc << 4 | counts.sc;

    uint8_t *typePos = hashTypes;
    for(uint8_t i = 0; i < counts.hc; i++)
    {
        for(uint8_t j = 0; j < 4; j++)
        {
            *(++pos) = *(typePos++);
        }
    }
    typePos = cipherTypes;
    for(uint8_t i = 0; i < counts.cc; i++)
    {
        for(uint8_t j = 0; j < 4; j++)
        {
            *(++pos) = *(typePos++);
        }
    }
    typePos = authTagTypes;
    for(uint8_t i = 0; i < counts.ac; i++)
    {
        for(uint8_t j = 0; j < 4; j++)
        {
            *(++pos) = *(typePos++);
        }
    }
    typePos = keyAgreementTypes;
    for(uint8_t i = 0; i < counts.kc; i++)
    {
        for(uint8_t j = 0; j < 4; j++)
        {
            *(++pos) = *(typePos++);
        }
    }
    typePos = sasTypes;
    for(uint8_t i = 0; i < counts.sc; i++)
    {
        for(uint8_t j = 0; j < 4; j++)
        {
            *(++pos) = *(typePos++);
        }
    }
    for(uint8_t i = 0; i < MAC_SIZE; i++)
    {
        *(++pos) = mac[i];
    }
    return data;
}

void PacketHello::parse(uint8_t *data)
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
    pos += 10;

    for(uint8_t i = 0; i < VERSION_SIZE; i++)
    {
        version[i] = *(pos++);
    }
    for(uint8_t i = 0; i < CLIENTID_SIZE; i++)
    {
        clientId[i] = *(pos++);
    }
    for(uint8_t i = 0; i < HASHIMAGE_SIZE; i++)
    {
        h3[i] = *(pos++);
    }
    for(uint8_t i = 0; i < ZID_SIZE; i++)
    {
        zid[i] = *(pos++);
    }
    flags = *(pos++);

    counts.hc = *(pos++) | 0x00;
    counts.cc = *(pos) >> 4 | 0x00;
    counts.ac = *(pos++) | 0x00;
    counts.kc = *(pos) >> 4 | 0x00;
    counts.sc = *(pos++) | 0x00;

    uint8_t *typePos = hashTypes;
    for(uint8_t i = 0; i < counts.hc; i++)
    {
        for(uint8_t j = 0; j < 4; j++)
        {
            *(typePos++) = *(pos++);
        }
    }
    typePos = cipherTypes;
    for(uint8_t i = 0; i < counts.cc; i++)
    {
        for(uint8_t j = 0; j < 4; j++)
        {
            *(typePos++) = *(pos++);
        }
    }
    typePos = authTagTypes;
    for(uint8_t i = 0; i < counts.ac; i++)
    {
        for(uint8_t j = 0; j < 4; j++)
        {
            *(typePos++) = *(pos++);
        }
    }
    typePos = keyAgreementTypes;
    for(uint8_t i = 0; i < counts.kc; i++)
    {
        {
            *(typePos++) = *(pos++);
        }
    }
    typePos = sasTypes;
    for(uint8_t i = 0; i < counts.sc; i++)
    {
        {
            *(typePos++) = *(pos++);
        }
    }
    for(uint8_t i = 0; i < MAC_SIZE; i++)
    {
        mac[i] = *(pos++);
    }
}

void PacketHello::setVersion(uint8_t *version)
{
    memcpy(this->version,version,VERSION_SIZE);
}

void PacketHello::setClientId(std::string id)
{
    memset(clientId,0,CLIENTID_SIZE);
    memcpy(clientId,id.c_str(),id.length());
}

void PacketHello::setH3(uint8_t *hash)
{
    memcpy(h3,hash,HASHIMAGE_SIZE);
}

void PacketHello::setZid(uint8_t *zid)
{
    memcpy(this->zid,zid,sizeof(uint8_t[ZID_SIZE]));
}

void PacketHello::setFlagS()
{
    flags |= 64;
}

void PacketHello::setFlagM()
{
    flags |= 32;
}

void PacketHello::setFlagP()
{
    flags |= 16;
}

void PacketHello::addHash(uint8_t hash[])
{
    counts.hc++;
    memcpy(&hashTypes[(counts.hc - 1) * WORD_SIZE],hash,WORD_SIZE);
    setLength(getLength() + 1);
}

void PacketHello::addCipher(uint8_t cipher[])
{
    counts.cc++;
    memcpy(&cipherTypes[(counts.cc - 1) * WORD_SIZE], cipher, WORD_SIZE);
    setLength(getLength() + 1);
}

void PacketHello::addAuthTag(uint8_t authTag[])
{
    counts.ac++;
    memcpy(&authTagTypes[(counts.ac - 1) * WORD_SIZE], authTag, WORD_SIZE);
    setLength(getLength() + 1);
}

void PacketHello::addKeyAgreement(uint8_t keyAgreement[])
{
    counts.kc++;
    memcpy(&keyAgreementTypes[(counts.kc - 1) * WORD_SIZE], keyAgreement, WORD_SIZE);
    setLength(getLength() + 1);
}

void PacketHello::addSas(uint8_t sas[])
{
    counts.sc++;
    memcpy(&sasTypes[(counts.sc - 1) * WORD_SIZE], sas, WORD_SIZE);
    setLength(getLength() + 1);
}

void PacketHello::setMac(uint8_t mac[])
{
    memcpy(this->mac, mac, MAC_SIZE);
}
