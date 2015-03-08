#include "packetcommit.h"

PacketCommit::PacketCommit()
{
    packetHeader = new Header();
    setZrtpIdentifier();
    setType((uint8_t*)"Commit  ");
    setLength((sizeof(Header) / WORD_SIZE) - 1);
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

    return data;
}

void PacketCommit::parse(uint8_t *data)
{

}
