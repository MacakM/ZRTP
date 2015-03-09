#include "packetcommit.h"

PacketCommit::PacketCommit()
{
    packetHeader = new Header();
    setZrtpIdentifier();
    setType((uint8_t*)"Commit  ");
    setLength(29);

}

uint8_t *PacketCommit::toBytes()
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

void PacketCommit::parse(uint8_t *data)
{

}

void PacketCommit::setH2(uint8_t *hash)
{
    memcpy(h2,hash,HASHIMAGE_SIZE);
}

void PacketCommit::setZid(uint8_t *zid)
{
    memcpy(this->zid,zid,ZID_SIZE);
}

void PacketCommit::setHash(uint8_t hash[])
{
    memcpy(this->hash,hash,HASHIMAGE_SIZE);
}

void PacketCommit::setCipher(uint8_t cipher[])
{
    memcpy(this->cipher, cipher, WORD_SIZE);
}

void PacketCommit::setAuthTag(uint8_t authTag[])
{
    memcpy(this->authTag, authTag, WORD_SIZE);
}

void PacketCommit::setKeyAgreement(uint8_t keyAgreement[])
{
    memcpy(this->keyAgreement, keyAgreement, WORD_SIZE);
}

void PacketCommit::setSas(uint8_t sas[])
{
    memcpy(this->sas, sas, WORD_SIZE);
}
