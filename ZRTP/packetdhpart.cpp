#include "packetdhpart.h"

PacketDHPart::PacketDHPart()
{
    packetHeader = new Header();
    setZrtpIdentifier();
    setLength((sizeof(Header) / WORD_SIZE) - 1);
}

uint8_t *PacketDHPart::toBytes()
{
    uint8_t *pos = data;

    *pos = packetHeader->identifier >> 8;
    pos++;
    *pos = packetHeader->identifier;
    pos++;

    *pos = packetHeader->length >> 8;
    pos++;
    *pos = packetHeader->length;

    for (uint8_t i = 0; i < TYPE_SIZE; i++)
    {
        *(++pos) = packetHeader->type[i];
    }

    return data;
}

void PacketDHPart::parse(uint8_t *data)
{

}

uint8_t *PacketDHPart::getHashImage()
{
    return h1;
}

void PacketDHPart::setH1(uint8_t *hash)
{
    memcpy(h1,hash,HASHIMAGE_SIZE);
}

void PacketDHPart::setRs1Id(uint8_t *rs1Id)
{
    memcpy(this->rs1Id,rs1Id,ID_SIZE);
}

void PacketDHPart::setRs2Id(uint8_t *rs2Id)
{
    memcpy(this->rs2Id,rs2Id,ID_SIZE);
}

void PacketDHPart::setAuxsecretId(uint8_t *auxsecretId)
{
    memcpy(this->auxsecretId,auxsecretId,ID_SIZE);
}

void PacketDHPart::setPbxsecretId(uint8_t *pbxsecretId)
{
    memcpy(this->pbxsecretId,pbxsecretId,ID_SIZE);
}

void PacketDHPart::setPv(uint8_t publicValue[])
{
    memcpy(pv,publicValue,96*4);
}

void PacketDHPart::setMac(uint8_t mac[])
{
    memcpy(this->mac, mac, MAC_SIZE);
}
