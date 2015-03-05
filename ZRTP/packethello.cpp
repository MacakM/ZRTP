#include "packethello.h"

PacketHello::PacketHello()
{
    packetHeader = new Header();
    setZrtpIdentifier();
    setType((uint8_t*)"Hello   ");
    setLength(((sizeof(Header) + sizeof(uint8_t[CLIENTID_SIZE])) / WORD_SIZE) - 1);
    memset(clientId,0,CLIENTID_SIZE);
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

void PacketHello::setClientId(std::string id)
{
    memcpy(clientId,id.c_str(),id.length());
}
