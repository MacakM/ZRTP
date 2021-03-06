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
#define MAX_ERRORACK_LENGTH 12

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

typedef enum
{
    MalformedPacket             = 0x10,
    CriticalSoftwareError       = 0x20,
    UnsupportedVersion          = 0x30,
    HelloComponentsMismatch     = 0x40,
    HashTypeNotSupported        = 0x51,
    CipherTypeNotSupported      = 0x52,
    KeyExchangeNotSupported     = 0x53,
    AuthTagNotSupported         = 0x54,
    SasNotSupported             = 0x55,
    NoSharedSecret              = 0x56,
    BadDhPublicValue            = 0x61,
    DhHviError                  = 0x62,
    ReceivedUntrustedSas        = 0x63,
    BadConfirm                  = 0x70,
    NonceReuse                  = 0x80,
    EqualZid                    = 0x90,
    ColissionSSRC               = 0x91,
    ServiceUnavailable          = 0xA0,
    ProtocolTimeout             = 0xB0,
    ReceivedNotAllowedGoClear   = 0x100
}ErrorCode;

typedef struct
{
    uint16_t identifier;
    uint16_t length;
    uint8_t type[TYPE_SIZE];
}Header;

class Packet {

public:
    Packet() {}

    virtual~Packet() {}

    /**
     * Returns length of packet in WORDs. WORD = 4 bytes.
     *
     * @return  packet length
     */
    uint16_t getLength() { return packetHeader->length; }

    /**
     * Returns type of packet.
     *
     * @return  packet type
     */
    uint8_t *getType() { return packetHeader->type; }

    /**
     * Sets length of packet in WORDs.
     *
     * @param length    packet length
     */
    void setLength(uint16_t length) { packetHeader->length = length; }

    /**
     * Sets type of packet.
     *
     * @param type      packet type
     */
    void setType(uint8_t *type) { memcpy(packetHeader->type, type, TYPE_SIZE); }

    /**
     * Sets zrtp identifier 0x505a.
     */
    void setZrtpIdentifier() { packetHeader->identifier = (uint16_t)ZRTP_IDENTIFIER; }

    /**
     * Sets MAC of packet. Only implemented for Hello, Commit and DHPart packets.
     */
    virtual void setMac(uint8_t *) {}

    /**
     * Creates array of bytes from class attributes according to RFC.
     *
     * @return  array of bytes
     */
    virtual uint8_t *toBytes() = 0;

    /**
     * Parses received data into Packet class.
     *
     * @param data          received data
     * @param errorCode     code of occurred error in parsing
     *
     * @return              true = successful, false = error occurred
     */
    virtual bool parse(uint8_t *data, uint32_t *errorCode) = 0;

protected:
    Header *packetHeader;
};

#endif // PACKET_H
