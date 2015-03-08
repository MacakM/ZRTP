#include "packetconfirm.h"

PacketConfirm::PacketConfirm()
{
    packetHeader = new Header();
    setZrtpIdentifier();
    setLength((sizeof(Header) / WORD_SIZE) - 1);
}

uint8_t *PacketConfirm::toBytes()
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

void PacketConfirm::parse(uint8_t *data)
{

}
