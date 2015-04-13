#include "packetconf2ack.h"

PacketConf2Ack::PacketConf2Ack()
{
    packetHeader = new Header();
    setZrtpIdentifier();
    setType((uint8_t*)"Conf2ACK");
    setLength((sizeof(Header) / WORD_SIZE) - 1);
}

PacketConf2Ack::~PacketConf2Ack()
{
    std::cout << "Conf2ACK destructing" << std::endl;
    delete (packetHeader);
    memset(data,0,MAX_CONF2ACK_LENGTH);
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

bool PacketConf2Ack::parse(uint8_t *data, uint32_t *errorCode)
{
    uint8_t *pos = data;

    packetHeader->identifier = *pos << 8 | *(pos + 1);
    pos += 2;

    uint16_t zrtpId = (uint16_t)ZRTP_IDENTIFIER;
    if(memcmp(&packetHeader->identifier,&zrtpId,WORD_SIZE) != 0)
    {
        *errorCode = MalformedPacket;
        return false;
    }

    packetHeader->length = *pos << 8 | *(pos + 1);
    //type has been checked in StateEngine
    setType((uint8_t*)"Conf2ACK");
    return true;
}
