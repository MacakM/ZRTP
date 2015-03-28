#ifndef PACKETERRORACK_H
#define PACKETERRORACK_H

#include "packet.h"

class PacketErrorAck : public Packet
{
public:
    /**
     * Constructor of ErrorAck packet.
     */
    PacketErrorAck();

    /**
     * Destructor of ErrorAck packet.
     */
    virtual ~PacketErrorAck();

    /**
     * Creates array of bytes from class attributes according to RFC.
     *
     * @return  array of bytes
     */
    virtual uint8_t *toBytes();

    /**
     * Parses received data into PacketErrorAck class.
     *
     * @param data  received data
     * @return      true = successful, false = otherwise
     */
    virtual bool parse(uint8_t *data);

private:
    uint8_t data[MAX_ERRORACK_LENGTH];
};

#endif // PACKETERRORACK_H
