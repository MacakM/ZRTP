#ifndef PACKET_H
#define PACKET_H

#include "Integers.h"
#include <iostream>
#include <string>

#define WORD_SIZE           4

#define TYPE_SIZE           2 * WORD_SIZE
#define VERSION_SIZE        1 * WORD_SIZE
#define CLIENTID_SIZE       4 * WORD_SIZE
#define HASHIMAGE_SIZE      8 * WORD_SIZE
#define ZID_SIZE            3 * WORD_SIZE
#define MAC_SIZE            2 * WORD_SIZE
#define HVI_SIZE            8 * WORD_SIZE
#define ID_SIZE             2 * WORD_SIZE
#define VECTOR_SIZE         4 * WORD_SIZE

typedef struct
{
    uint16_t identifier;
    uint16_t length;
    uint8_t type[TYPE_SIZE];
}Header;

class Packet {

public:
    Packet();

    uint16_t getLength();
    uint8_t *getType();

    void setLength(uint16_t length);
    void setType(uint8_t *type);
    void setZrtpIdentifier();

    virtual uint8_t *toBytes() = 0;

protected:
    Header *packetHeader;
};

#endif // PACKET_H
