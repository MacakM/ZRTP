#include "packetconfirm.h"

PacketConfirm::PacketConfirm()
{
    packetHeader = new Header();
    setZrtpIdentifier();
    setLength(3);//19
    flags = 0;
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

void PacketConfirm::setInitVector(uint8_t vector[])
{
    memcpy(initVector,vector,VECTOR_SIZE);
}

void PacketConfirm::setH0(uint8_t *hash)
{
    memcpy(h0,hash,HASHIMAGE_SIZE);
}

void PacketConfirm::setSigLen(uint16_t length)
{
    sigLen = length;
}

void PacketConfirm::setFlagE()
{
    flags |= 8;
}

void PacketConfirm::setFlagV()
{
    flags |= 4;
}

void PacketConfirm::setFlagA()
{
    flags |= 2;
}

void PacketConfirm::setFlagD()
{
    flags |= 1;
}

void PacketConfirm::setExpInterval(uint32_t interval)
{
    expInterval = interval;
}
