#ifndef PACKET_H
#define PACKET_H

#include "Integers.h"
#include <iostream>
#include <string>
#include <cstring>

#define ZRTP_IDENTIFIER     0x505a

#define MAX_HELLO_LENGTH    220
#define MAX_HELLOACK_LENGTH 12
#define MAX_COMMIT_LENGTH   116
#define MAX_DHPART_LENGTH   468
#define MAX_CONFIRM_LENGTH  76
#define MAX_CONF2ACK_LENGTH 12
#define MAX_ERROR_LENGTH    16

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
    Packet() {}

    /**
     * Returns length of packet in WORDs. WORD = 4 bytes.
     *
     * @return  packet length
     */
    uint16_t getLength();

    /**
     * Returns type of packet.
     *
     * @return  packet type
     */
    uint8_t *getType();

    /**
     * Sets length of packet in WORDs.
     *
     * @param length    packet length
     */
    void setLength(uint16_t length);

    /**
     * Sets type of packet.
     *
     * @param type      packet type
     */
    void setType(uint8_t *type);

    /**
     * Sets zrtp identifier 0x505a.
     */
    void setZrtpIdentifier();

    /**
     * Sets MAC of packet. Only implemented for Hello, Commit and DHPart packets.
     */
    virtual void setMac(uint8_t [MAC_SIZE]) {}

    /**
     * Creates array of bytes from class attributes according to RFC.
     *
     * @return  array of bytes
     */
    virtual uint8_t *toBytes() = 0;

    /**
     * Parses received data into PacketHelloAck class.
     *
     * @param data  received data
     * @return      true = successful, false = otherwise
     */
    virtual bool parse(uint8_t *data) = 0;

protected:
    Header *packetHeader;
};

#endif // PACKET_H
