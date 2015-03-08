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
    memcpy(version,"1.10",VERSION_SIZE);
    flags = 0;

    counts.hc = 2;
    counts.cc = 1;
    counts.ac = 1;
    counts.kc = 1;
    counts.sc = 1;

    hashTypes = (uint8_t*)"S256S386";
    cipherTypes = (uint8_t*)"AES1";
    authTagTypes = (uint8_t*)"HS32";
    keyAgreementTypes = (uint8_t*)"DH3K";
    sasTypes = (uint8_t*)"B32 ";
    setLength(getLength() + (4 / WORD_SIZE) * (counts.hc + counts.cc + counts.ac + counts.kc + counts.sc));
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
    *(++pos) = '\0';
    return data;
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

void PacketHello::setMac(uint8_t mac[])
{
    memcpy(this->mac, mac, MAC_SIZE);
}
