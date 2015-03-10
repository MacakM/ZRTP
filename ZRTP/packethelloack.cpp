#include "packethelloack.h"

PacketHelloAck::PacketHelloAck()
{
    packetHeader = new Header();
    setZrtpIdentifier();
    setType((uint8_t*)"HelloACK");
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

void PacketHelloAck::parse(uint8_t *data)
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
    setType((uint8_t*)"HelloACK");
}
