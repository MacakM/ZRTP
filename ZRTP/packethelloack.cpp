#include "packethelloack.h"

PacketHelloAck::PacketHelloAck()
{
    packetHeader = new Header();
    setZrtpIdentifier();
    uint8_t *type = (uint8_t*)"HelloACK";
    setType(type);
    setLength((sizeof(Header) / WORD_SIZE) - 1);
}

uint8_t *PacketHelloAck::toBytes()
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
    return data;

}
