#ifndef ZRTP_H
#define ZRTP_H

#define KDF_CONTEXT_LENGTH  2 * ZID_SIZE + SHA256_DIGEST_LENGTH
#define AES1_KEY_LENGTH     16
#define SALT_KEY_LENGTH     14

#include <iostream>
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

/**
 * Zrtp class is responsible for processing messages and timeouts.
 * It also does all cryptographical functions and stores important packets.
 *
 * It cooperates with StateEngine class, giving it received messages and timeouts.
 *
 * User have to implement functions defined in ZrtpCallback class, so Zrtp can use them.
 *
 * @author Martin Macak
 *
 * @see ZrtpCallback
 */
class Zrtp
{
    friend class StateEngine;

public:
    /**
     * Zrtp costructor.
     *
     * @param cb        callback class
     * @param role      role of user
     * @param clientId  identifies the vendor and release of the ZRTP software
     */
    Zrtp(ZrtpCallback *cb, Role role, std::string clientId);

    /**
     * Method called by user when ZRTP message arrives.
     *
     * @param msg       received message
     * @param length    length of message
     */
    void processMessage(uint8_t *msg, int32_t length);

    /**
     * Method called by user when the Timer ends.
     */
    void processTimeout();

    /**
     * At the first run of ZRTP it generates random ZID which can user store for later purposes.
     *
     * @return ZID
     */
    uint8_t getZid();

private:
    /**
     * Function that Zrtp calls when it wants to send message throught ZrtpCallback.
     *
     * @param data      data that is going to be send
     * @param length    length of data
     * @return          true = successful, false = otherwise
     */
    bool sendData(const uint8_t *data, int32_t length);

    /**
     * Activate timer throught ZrtpCallback.
     *
     * @param time  timeout interval in milliseconds
     * @return      true = successful, false = otherwise
     */
    bool activateTimer(int32_t time);

    /**
     * Cancel timer throught ZrtpCallback.
     *
     * @return true = successful, false = otherwise
     */
    bool cancelTimer();

    /**
     * Enter critical section throught ZrtpCallback.
     */
    void enterCriticalSection();

    /**
     * Leave critical section throught ZrtpCallback.
     */
    void leaveCriticalSection();

    /**
     * Prepares Hello packet for sending.
     *
     * @param clientId  obtained from user in constructor
     */
    void createHelloPacket(std::string clientId);

    /**
     * Prepares HelloAck packet for sending.
     */
    void createHelloAckPacket();

    /**
     * Prepares Commit packet for sending.
     */
    void createCommitPacket();

    /**
     * Prepares DHPart1 packet for sending.
     */
    void createDHPart1Packet();

    /**
     * Prepares DHPart2 packet for sending.
     */
    void createDHPart2Packet();

    /**
     * Prepares Confirm1 packet for sending.
     */
    void createConfirm1Packet();

    /**
     * Prepares Confirm2 packet for sending.
     */
    void createConfirm2Packet();

    /**
     * Generates h0, h1, h2, h3.
     */
    void generateHashImages();

    /**
     * Generates rs1ID, rs2ID, auxsecretID, pbxsecretID and sets it to DHPart packet.
     *
     * @param packet    DHPart packet
     */
    void generateIds(PacketDHPart *packet);

    /**
     * Creates MAC hash at the end of the Hello, Commit and DHPart packets
     *
     * @param packet    packet which is going to be hashed
     */
    void createMac(Packet *packet);

    /**
     * Creates MAC hash for Confirm packets
     *
     * @param packet    confirm packet
     */
    void createConfirmMac(PacketConfirm *packet);

    /**
     * Computes the DH and sets public key, private key and prime.
     */
    void diffieHellman();

    /**
     * Generates hvi in commit packet. Have to create DHPart2 packet first.
     * hvi = hash(initiator's DHPart2 message || responder's Hello message)
     */
    void generateHvi();

    /**
     * Sets public value to DHPart packet
     *
     * @param packet    DHPart packet
     */
    void setPv(PacketDHPart *packet);

    /**
     * Creates total_hash.
     * total_hash = hash(Hello of responder || Commit || DHPart1 || DHPart2)
     */
    void createTotalHash();

    /**
     * Creates DHResult.
     * DHResult = (peer's publicValue ^ my secretValue) mod prime
     */
    void createDHResult();

    /**
     * Creates KDF_Context.
     * KDF_Context = (ZIDi || ZIDr || total_hash)
     */
    void createKDFContext();

    /**
     * Function that creates total_hash, DHResult and KDF_Context.
     */
    void sharedSecretCalculation();

    /**
     * Creates s0.
     * s0 = hash(counter || DHResult || "ZRTP-HMAC-KDF" || ZIDi ||
     *           ZIDr || total_hash || len(s1) || s1 || len(s2) ||
     *           s2 || len(s3) || s3)
     */
    void createS0();

    /**
     * Key derivation function.
     *
     * @param key           key that is being derived
     * @param label         identifies the purpose for derived keying material
     * @param labelLength   length of label
     * @param context       key derivation context
     * @param lengthL       length in bits of output
     * @param derivedKey    key after derivation
     */
    void kdf(uint8_t *key, uint8_t *label, int32_t labelLength, uint8_t *context, int32_t lengthL, uint8_t *derivedKey);

    /**
     * Generates ZRTPSess, sashash, sasvalue, exportedKey, srtpKeys, strpSalts, macKeys, zrtpKeys.
     */
    void keyDerivation();

    /**
     * Encrypt the part of Confirm packet.
     */
    void encryptConfirmData();

    /**
     * Decrypt the part of Confirm packet and sets attributes to that packet.
     *
     * @param data  encrypted data
     */
    void decryptConfirmData(uint8_t *data);

    uint8_t myZID[ZID_SIZE];
    ZrtpCallback *callback;
    Role myRole;
    StateEngine *engine;

    //important packets
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
