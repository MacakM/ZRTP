#include "packethello.h"

PacketHello::PacketHello()
{
    packetHeader = new Header();
    setZrtpIdentifier();
    uint8_t *type = (uint8_t*)"Hello   ";
    setType(type);
    setLength(((sizeof(Header) + sizeof(uint8_t[CLIENTID_SIZE])) / WORD_SIZE) - 1);
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
    for(uint8_t i = 0; i < CLIENTID_SIZE; i++)
    {
        *(++pos) = clientId[i];
    }

    return data;
}
