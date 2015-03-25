#include "packet.h"

uint16_t Packet::getLength()
{
    return packetHeader->length;
}

uint8_t *Packet::getType()
{
    return packetHeader->type;
}

void Packet::setLength(uint16_t length)
{
    packetHeader->length = length;
}

void Packet::setType(uint8_t *type)
{
    memcpy(packetHeader->type, type, TYPE_SIZE);
}

void Packet::setZrtpIdentifier()
{
    packetHeader->identifier = (uint16_t)ZRTP_IDENTIFIER;
}
