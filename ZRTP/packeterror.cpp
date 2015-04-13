#include "packeterror.h"

PacketError::PacketError()
{
    packetHeader = new Header();
    setZrtpIdentifier();
    setType((uint8_t*)"Error   ");
    setLength(4);
}

PacketError::PacketError(uint32_t code)
{
    packetHeader = new Header();
    setZrtpIdentifier();
    setType((uint8_t*)"Error   ");
    setLength(4);
    memcpy(&errorCode,&code,WORD_SIZE);
}

PacketError::~PacketError()
{
    std::cout << "Error destructing" << std::endl;
    delete (packetHeader);
    memset(data,0,MAX_ERROR_LENGTH);
}

uint8_t *PacketError::toBytes()
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
    pos++;
    memcpy(pos,&errorCode,WORD_SIZE);
    return data;
}

bool PacketError::parse(uint8_t *data, uint32_t *errorCode)
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
    setType((uint8_t*)"Error   ");
    pos += 10;
    memcpy(&(this->errorCode),pos,WORD_SIZE);
    return true;
}
