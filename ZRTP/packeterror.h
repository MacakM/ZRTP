#ifndef PACKETERROR_H
#define PACKETERROR_H

#include "packet.h"

class PacketError : public Packet
{
public:
    /**
     * Constructor of Error packet specific for parsing.
     */
    PacketError();

    /**
     * Constructor of Error packet.
     *
     * @param code  error code
     */
    PacketError(uint32_t code);

    /**
     * Destructor of Error packet.
     */
    virtual ~PacketError();

    /**
     * Creates array of bytes from class attributes according to RFC.
     *
     * @return  array of bytes
     */
    virtual uint8_t *toBytes();

    /**
     * Parses received data into PacketError class.
     *
     * @param data          received data
     * @param errorCode     code of occurred error in parsing
     *
     * @return              true = successful, false = error occurred
     */
    virtual bool parse(uint8_t *data, uint32_t *errorCode);

private:
    ErrorCode errorCode;

    uint8_t data[MAX_ERROR_LENGTH];
};

#endif // PACKETERROR_H
