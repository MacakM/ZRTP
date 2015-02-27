#include "packethello.h"

PacketHello::PacketHello()
{
    packetHeader = new Header();
    setZrtpIdentifier();
    uint8_t *type = (uint8_t*)"Hello   ";
    setType(type);
    setLength((sizeof(Header) / WORD_SIZE) - 1);
}
