#ifndef PACKETERROR_H
#define PACKETERROR_H

#include "packet.h"

class PacketError : public Packet
{
public:
    PacketError(uint8_t code[]);

    virtual uint8_t *toBytes();
    virtual void parse(uint8_t *data);

    uint8_t errorCode[WORD_SIZE];

    uint8_t data[MAX_ERROR_LENGTH];
};

#endif // PACKETERROR_H
