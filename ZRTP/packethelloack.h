#ifndef PACKETHELLOACK_H
#define PACKETHELLOACK_H

#include "packet.h"

class PacketHelloAck : public Packet
{
public:
    /**
     * Constructor of HelloAck packet.
     */
    PacketHelloAck();

    /**
     * Destructor of Hello packet.
     */
    virtual ~PacketHelloAck();

    /**
     * Creates array of bytes from class attributes according to RFC.
     *
     * @return  array of bytes
     */
    virtual uint8_t *toBytes();

    /**
     * Parses received data into PacketHelloAck class.
     *
     * @param data  received data
     * @return      true = successful, false = otherwise
     */
    virtual bool parse(uint8_t *data);

private:
    uint8_t data[MAX_HELLOACK_LENGTH];
};

#endif // PACKETHELLOACK_H
