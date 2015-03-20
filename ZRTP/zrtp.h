#ifndef ZRTP_H
#define ZRTP_H

#define KDF_CONTEXT_LENGTH  2 * ZID_SIZE + SHA256_DIGEST_LENGTH
#define AES1_KEY_LENGTH     16
#define SALT_KEY_LENGTH     14

#include <stdexcept>
#include <assert.h>
#include "Integers.h"
#include "ZrtpCallback.h"
#include "stateengine.h"

#include "packethello.h"
#include "packethelloack.h"
#include "packetcommit.h"
#include "packetdhpart.h"
#include "packetconfirm.h"
#include "packetconf2ack.h"
#include "packeterror.h"

#include <iostream>

#include <openssl/sha.h>
#include <openssl/hmac.h>
#include <openssl/rand.h>
#include <openssl/dh.h>
#include <openssl/aes.h>

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
    void createHelloAckPacket();
    void createCommitPacket();
    void createDHPart1Packet();
    void createDHPart2Packet();
    void createConfirm1Packet();
    void createConfirm2Packet();

    void createHashImages();
    void generateIds(PacketDHPart *packet);
    void createMac(Packet *packet);
    void createConfirmMac(PacketConfirm *packet);
    void diffieHellman();
    void generateHvi();
    void createTotalHash();
    void createDHResult();
    void sharedSecretCalculation();
    void createKDFContext();
    void createS0();
    void kdf(uint8_t *key, uint8_t *label, int32_t labelLength, uint8_t *context, int32_t lengthL, uint8_t *derivedKey);
    void keyDerivation();
    void encryptConfirmData();
    void decryptConfirmData(uint8_t *data);

    void setPv(PacketDHPart *packet);

    uint8_t myZID[ZID_SIZE];
    ZrtpCallback *callback;
    Role myRole;
    StateEngine *engine;

    PacketHello *hello;
    PacketHello *peerHello;
    PacketHelloAck *helloAck;
    PacketHelloAck *peerHelloAck;
    PacketCommit *commit;
    PacketDHPart *dhPart1;
    PacketDHPart *dhPart2;
    PacketConfirm *confirm1;
    PacketConfirm *confirm2;
    PacketConf2Ack *conf2Ack;
    PacketError *error;

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
    BIGNUM *p;
    BIGNUM *dhResult;

    uint8_t totalHash[SHA256_DIGEST_LENGTH];
    uint8_t s0[SHA256_DIGEST_LENGTH];
    uint8_t kdfContext[KDF_CONTEXT_LENGTH];

    uint8_t zrtpSess[SHA256_DIGEST_LENGTH];
    uint8_t sasHash[SHA256_DIGEST_LENGTH];
    uint8_t sasValue[32];
    uint8_t exportedKey[SHA256_DIGEST_LENGTH];

    uint8_t srtpKeyI[AES1_KEY_LENGTH];
    uint8_t srtpSaltI[SALT_KEY_LENGTH];
    uint8_t srtpKeyR[AES1_KEY_LENGTH];
    uint8_t srtpSaltR[SALT_KEY_LENGTH];

    uint8_t macKeyI[SHA256_DIGEST_LENGTH];
    uint8_t macKeyR[SHA256_DIGEST_LENGTH];

    uint8_t zrtpKeyI[AES1_KEY_LENGTH];
    uint8_t zrtpKeyR[AES1_KEY_LENGTH];
};

#endif // ZRTP_H
