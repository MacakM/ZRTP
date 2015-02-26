#ifndef PACKET_H
#define PACKET_H

#include "Integers.h"

#define WORD                4

#define TYPE_SIZE           2 * WORD
#define VERSION_SIZE        1 * WORD
#define CLIENTID_SIZE       4 * WORD
#define HASHIMAGE_SIZE      8 * WORD
#define ZID_SIZE            3 * WORD
#define MAC_SIZE            2 * WORD
#define HVI_SIZE            8 * WORD
#define ID_SIZE             2 * WORD
#define VECTOR_SIZE         4 * WORD

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

protected:
    Header *packetHeader;
};

#endif // PACKET_H
