#include "packethello.h"

PacketHello::PacketHello()
{
    packetHeader = new Header();
    setZrtpIdentifier();
    setType((uint8_t*)"Hello   ");
    setLength(((sizeof(Header) + sizeof(uint8_t[CLIENTID_SIZE]) +
                sizeof(uint8_t[HASHIMAGE_SIZE]) + sizeof(uint8_t[ZID_SIZE]) +
                sizeof(uint8_t) + sizeof(uint8_t[VERSION_SIZE]) + sizeof(Counts)) / WORD_SIZE) - 1);
    memcpy(version,"1.10",VERSION_SIZE);
    flags = 0;
    counts.hc = 0;
    counts.cc = 1;
    counts.ac = 5;
    counts.kc = 0;
    counts.sc = 10;
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
