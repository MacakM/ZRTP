#ifndef PACKETCONF2ACK_H
#define PACKETCONF2ACK_H

#include "packet.h"

class PacketConf2Ack : public Packet
{
public:
    /**
     * Constructor of Conf2Ack packet.
     */
    PacketConf2Ack();

    /**
     * Destructor of Conf2Ack packet.
     */
    virtual ~PacketConf2Ack();

    /**
     * Creates array of bytes from class attributes according to RFC.
     *
     * @return  array of bytes
     */
    virtual uint8_t *toBytes();

    /**
     * Parses received data into PacketConf2Ack class.
     *
     * @param data          received data
     * @param errorCode     code of occurred error in parsing
     *
     * @return              true = successful, false = error occurred
     */
    virtual bool parse(uint8_t *data, uint32_t *errorCode);

private:
    uint8_t data[MAX_CONF2ACK_LENGTH];
};

#endif // PACKETCONF2ACK_H
