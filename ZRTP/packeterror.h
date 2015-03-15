#ifndef PACKETERROR_H
#define PACKETERROR_H

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

#include "packet.h"

class PacketError : public Packet
{
public:
    PacketError(ErrorCode code);

    virtual uint8_t *toBytes();
    virtual void parse(uint8_t *data);

    ErrorCode *errorCode;

    uint8_t data[MAX_ERROR_LENGTH];
};

#endif // PACKETERROR_H
