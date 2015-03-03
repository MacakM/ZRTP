#include "packetconf2ack.h"

PacketConf2Ack::PacketConf2Ack()
{
    packetHeader = new Header();
    setZrtpIdentifier();
    setType((uint8_t*)"Conf2ACK");
    setLength((sizeof(Header) / WORD_SIZE) - 1);
}

uint8_t *PacketConf2Ack::toBytes()
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
