#include "packet.h"

Packet::Packet()
{
}


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
    //TODO
}

void Packet::setType(uint8_t *type)
{
    //TODO
}
