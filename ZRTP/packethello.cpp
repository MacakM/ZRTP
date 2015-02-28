#include "packethello.h"

PacketHello::PacketHello()
{
    packetHeader = new Header();
    setZrtpIdentifier();
    uint8_t *type = (uint8_t*)"Hello   ";
    setType(type);
    setLength((sizeof(Header) / WORD_SIZE) - 1);
}

uint8_t *PacketHello::toBytes()
{
    uint8_t *pos = data;


    uint8_t first = packetHeader->identifier & 0xFF;
    uint8_t second = (packetHeader->identifier >> 8) & 0xFF;

    *pos = first;
    pos++;
    *pos = second;

    return data;
    //TODO
}
