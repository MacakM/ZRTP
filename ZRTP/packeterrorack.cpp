#include "packeterrorack.h"

PacketErrorAck::PacketErrorAck()
{
    packetHeader = new Header();
    setZrtpIdentifier();
    setType((uint8_t*)"ErrorACK");
    setLength((sizeof(Header) / WORD_SIZE) - 1);
}

uint8_t *PacketErrorAck::toBytes()
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

bool PacketErrorAck::parse(uint8_t *data)
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
    //type has been checked in StateEngine
    setType((uint8_t*)"Conf2ACK");
    return true;
}
