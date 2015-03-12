#ifndef ZRTP_H
#define ZRTP_H

#include "Integers.h"
#include "ZrtpCallback.h"
#include "stateengine.h"

#include "packethello.h"
#include "packethelloack.h"
#include "packetcommit.h"
#include "packetdhpart.h"
#include "packetconfirm.h"
#include "packetconf2ack.h"

#include <iostream>

#include <openssl/sha.h>
#include <openssl/hmac.h>
#include <openssl/rand.h>
#include <openssl/dh.h>

typedef enum
{
    Initiator,
    Responder
}Role;

class StateEngine;

class Zrtp
{
    friend class StateEngine;

public:
    Zrtp(ZrtpCallback *cb, Role role, std::string clientId);

    void processMessage(uint8_t *msg, int32_t length);
    void processTimeout();

    uint8_t getZid();

private:
    bool sendData(const uint8_t *data, int32_t length);
    bool activateTimer(int32_t time);
    bool cancelTimer();

    void createHelloPacket(std::string clientId);
    void createCommitPacket();
    void createDHPart1Packet();
    void createDHPart2Packet();

    void createHashImages();
    void generateIds(PacketDHPart *packet);
    void createMac(Packet *packet);
    void diffieHellman(PacketDHPart *packet);
    void generateHvi();

    uint8_t myZID[ZID_SIZE];
    ZrtpCallback *callback;
    Role myRole;
    StateEngine *engine;

    PacketHello *hello;
    PacketHello *peerHello;
    PacketHelloAck *helloAck;
    PacketCommit *commit;
    PacketDHPart *dhPart1;
    PacketDHPart *dhPart2;
    PacketConfirm *confirm1;
    PacketConfirm *confirm2;
    PacketConf2Ack *conf2Ack;

    uint8_t h0[HASHIMAGE_SIZE];
    uint8_t h1[HASHIMAGE_SIZE];
    uint8_t h2[HASHIMAGE_SIZE];
    uint8_t h3[HASHIMAGE_SIZE];

    uint8_t hash[WORD_SIZE];
    uint8_t cipher[WORD_SIZE];
    uint8_t authTag[WORD_SIZE];
    uint8_t keyAgreement[WORD_SIZE];
    uint8_t sas[WORD_SIZE];

    //I don't know length...
    uint8_t rs1[ID_SIZE];
    uint8_t rs2[ID_SIZE];
    uint8_t auxsecret[ID_SIZE];
    uint8_t pbxsecret[ID_SIZE];

    BIGNUM *privateKey;
    BIGNUM *publicKey;
};

#endif // ZRTP_H
