#include "packethelloack.h"

PacketHelloAck::PacketHelloAck()
{
    packetHeader = new Header();
    setZrtpIdentifier();
    setType((uint8_t*)"HelloACK");
    setLength((sizeof(Header) / WORD_SIZE) - 1);
}

PacketHelloAck::~PacketHelloAck()
{
    std::cout << "HelloAck destructing" << std::endl;
    delete(packetHeader);
    memset(data,0,MAX_HELLOACK_LENGTH);
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

bool PacketHelloAck::parse(uint8_t *data, uint32_t *errorCode)
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
    setType((uint8_t*)"HelloACK");
    return true;
}
