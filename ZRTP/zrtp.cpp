#include "zrtp.h"

Zrtp::Zrtp(ZrtpCallback *cb, Role role, std::string clientId)
{
    if(!cb)
    {
        throw std::invalid_argument("NULL callback");
    }
    callback = cb;
    myRole = role;

    RAND_bytes(myZID,ZID_SIZE);
    createHashImages();

    RAND_bytes(rs1,ID_SIZE);
    RAND_bytes(rs2,ID_SIZE);
    RAND_bytes(auxsecret,ID_SIZE);
    RAND_bytes(pbxsecret,ID_SIZE);

    diffieHellman();

    assert(publicKey);
    assert(privateKey);
    assert(p);

    engine = new StateEngine(this);

    assert(engine);

    createHelloPacket(clientId);

    assert(hello);

    conf2Ack = new PacketConf2Ack();

    Event event;
    event.type = Start;
    engine->processEvent(&event);
}

void Zrtp::processMessage(uint8_t *msg, int32_t length)
{
    //ignore packet with shorter length than it should be
    if(length < (int32_t)sizeof(Header) - WORD_SIZE)
    {
        return;
    }
    Event event;
    event.type = Message;
    event.message = msg;
    event.messageLength = length;
    engine->processEvent(&event);
}

void Zrtp::processTimeout()
{
    Event event;
    event.type = Timeout;
    engine->processEvent(&event);
}

uint8_t Zrtp::getZid()
{
    return *myZID;
}

bool Zrtp::sendData(const uint8_t *data, int32_t length)
{
    return callback->sendData(data, length);
}

bool Zrtp::activateTimer(int32_t time)
{
    return callback->activateTimer(time);
}

bool Zrtp::cancelTimer()
{
    return callback->cancelTimer();
}

void Zrtp::createHelloPacket(std::string clientId)
{
    hello = new PacketHello();
    hello->setVersion((uint8_t*)"1.10");
    hello->setClientId(clientId);
    hello->setH3(h3);
    hello->setZid(myZID);
    hello->addHash((uint8_t*)"S256");
    hello->addHash((uint8_t*)"S386");
    hello->addCipher((uint8_t*)"AES1");
    hello->addAuthTag((uint8_t*)"HS32");
    hello->addKeyAgreement((uint8_t*)"DH3K");
    hello->addSas((uint8_t*)"B32 ");
    createMac(hello);
}

void Zrtp::createHelloAckPacket()
{
    helloAck = new PacketHelloAck();
}

void Zrtp::createCommitPacket()
{
    commit = new PacketCommit();
    commit->setH2(h2);
    commit->setZid(myZID);
    commit->setHash((uint8_t*)"S256");
    memcpy(hash,(uint8_t*)"S256",WORD_SIZE);
    commit->setCipher((uint8_t*)"AES1");
    memcpy(cipher,(uint8_t*)"AES1",WORD_SIZE);
    commit->setAuthTag((uint8_t*)"HS32");
    memcpy(authTag,(uint8_t*)"HS32",WORD_SIZE);
    commit->setKeyAgreement((uint8_t*)"DH3K");
    memcpy(keyAgreement,(uint8_t*)"DH3K",WORD_SIZE);
    commit->setSas((uint8_t*)"B32 ");
    memcpy(sas,(uint8_t*)"B32 ",WORD_SIZE);
    generateHvi();
    createMac(commit);
}

void Zrtp::createDHPart1Packet()
{
    dhPart1 = new PacketDHPart();
    dhPart1->setType((uint8_t*)"DHPart1 ");
    dhPart1->setH1(h1);
    generateIds(dhPart1);
    setPv(dhPart1);
    createMac(dhPart1);
}

void Zrtp::createDHPart2Packet()
{
    dhPart2 = new PacketDHPart();
    dhPart2->setType((uint8_t*)"DHPart2 ");
    dhPart2->setH1(h1);
    generateIds(dhPart2);
    setPv(dhPart2);
    createMac(dhPart2);
}

void Zrtp::createConfirm1Packet()
{
    confirm1 = new PacketConfirm();
    confirm1->setType((uint8_t*)"Confirm1");
    confirm1->setH0(h0);
    confirm1->setSigLen(0);
    confirm1->setExpInterval(0);

    uint8_t vector[VECTOR_SIZE];
    RAND_bytes(vector,VECTOR_SIZE);
    confirm1->setInitVector(vector);
    encryptConfirmData();

    createConfirmMac(confirm1);
}

void Zrtp::createConfirm2Packet()
{
    confirm2 = new PacketConfirm();
    confirm2->setType((uint8_t*)"Confirm2");
    confirm2->setH0(h0);
    confirm2->setSigLen(0);
    confirm2->setExpInterval(0);

    uint8_t vector[VECTOR_SIZE];
    RAND_bytes(vector,VECTOR_SIZE);
    confirm2->setInitVector(vector);
    encryptConfirmData();

    createConfirmMac(confirm2);
}

void Zrtp::createHashImages()
{
    RAND_bytes(h0,HASHIMAGE_SIZE);

    SHA256(h0,HASHIMAGE_SIZE,h1);
    SHA256(h1,HASHIMAGE_SIZE,h2);
    SHA256(h2,HASHIMAGE_SIZE,h3);
}

void Zrtp::generateIds(PacketDHPart *packet)
{
    if(memcmp(hash,"S256",WORD_SIZE) == 0)
    {
        uint8_t id[SHA256_DIGEST_LENGTH];
        SHA256(rs1,ID_SIZE,id);
        packet->setRs1Id(id);
        SHA256(rs2,ID_SIZE,id);
        packet->setRs1Id(id);
        SHA256(auxsecret,ID_SIZE,id);
        packet->setAuxsecretId(id);
        SHA256(pbxsecret,ID_SIZE,id);
        packet->setPbxsecretId(id);
    }
}

void Zrtp::createMac(Packet *packet)
{
    uint8_t key[HASHIMAGE_SIZE];

    switch(packet->getType()[0])
    {
    //Hello
    case 'H':
        memcpy(key,h2,HASHIMAGE_SIZE);
        break;
    //Commit
    case 'C':
        memcpy(key,h1,HASHIMAGE_SIZE);
        break;
    //DHPart1 or DHPart2
    case 'D':
        memcpy(key,h0,HASHIMAGE_SIZE);
        break;
    }

    uint8_t *buffer = packet->toBytes();

    assert(buffer);

    uint16_t length = packet->getLength() - 2;

    buffer[(length) * WORD_SIZE] = '\0';

    uint8_t* digest;
    digest = HMAC(EVP_sha256(), key, HASHIMAGE_SIZE,
                  buffer, length, NULL, NULL);

    assert(digest);

    uint8_t computedMac[MAC_SIZE + 1];
    for(int i = 0; i < 4; i++)
    {
        sprintf((char*)&computedMac[i*2], "%02x", (unsigned int)digest[i]);
    }
    packet->setMac(computedMac);
}

void Zrtp::createConfirmMac(PacketConfirm *packet)
{
    uint8_t key[SHA256_DIGEST_LENGTH];
    (myRole == Initiator) ? memcpy(&key,macKeyI,MAC_SIZE) : memcpy(&key,macKeyR,MAC_SIZE);
    uint8_t *buffer = packet->toBytes();
    uint16_t bufferLength = packet->getLength() * WORD_SIZE;

    uint8_t *startHash = &(buffer[9*WORD_SIZE]);

    uint8_t* digest;
    digest = HMAC(EVP_sha256(), key, HASHIMAGE_SIZE,
                  startHash, bufferLength - 9*WORD_SIZE, NULL, NULL);

    assert(digest);

    uint8_t computedMac[MAC_SIZE + 1];
    for(int i = 0; i < 4; i++)
    {
        sprintf((char*)&computedMac[i*2], "%02x", (unsigned int)digest[i]);
    }
    packet->setConfirmMac(computedMac);
}

void Zrtp::diffieHellman()
{
    const char* modpGroup3072 =
            "FFFFFFFFFFFFFFFFC90FDAA22168C234C4C6628B80DC1CD1"
            "29024E088A67CC74020BBEA63B139B22514A08798E3404DD"
            "EF9519B3CD3A431B302B0A6DF25F14374FE1356D6D51C245"
            "E485B576625E7EC6F44C42E9A637ED6B0BFF5CB6F406B7ED"
            "EE386BFB5A899FA5AE9F24117C4B1FE649286651ECE45B3D"
            "C2007CB8A163BF0598DA48361C55D39A69163FA8FD24CF5F"
            "83655D23DCA3AD961C62F356208552BB9ED529077096966D"
            "670C354E4ABC9804F1746C08CA18217C32905E462E36CE3B"
            "E39E772C180E86039B2783A2EC07A28FB5C55DF06F4C52C9"
            "DE2BCBF6955817183995497CEA956AE515D2261898FA0510"
            "15728E5A8AAAC42DAD33170D04507A33A85521ABDF1CBA64"
            "ECFB850458DBEF0A8AEA71575D060C7DB3970F85A6E1E4C7"
            "ABF5AE8CDB0933D71E8C94E04A25619DCEE3D2261AD2EE6B"
            "F12FFA06D98A0864D87602733EC86A64521F2B18177B200C"
            "BBE117577A615D6C770988C0BAD946E208E24FA074E5AB31"
            "43DB5BFCE0FD108E4B82D120A93AD2CAFFFFFFFFFFFFFFFF";

    BIGNUM *prime = BN_new();
    BN_hex2bn(&prime,modpGroup3072);

    BIGNUM *generator = BN_new();
    BN_hex2bn(&generator,"2");

    DH *dh = DH_new();
    dh->p = prime;
    dh->g = generator;

    DH_generate_key(dh);

    privateKey = BN_new();
    publicKey = BN_new();
    p = BN_new();

    BN_copy(privateKey,dh->priv_key);
    BN_copy(publicKey,dh->pub_key);
    BN_copy(p, dh->p);

    /*BN_free(prime);
    BN_free(&generator);*/

    DH_free(dh);
}

void Zrtp::generateHvi()
{
    createDHPart2Packet();
    assert(dhPart2);
    uint8_t *buffer = (uint8_t*)malloc((dhPart2->getLength() + peerHello->getLength()) * WORD_SIZE);

    memcpy(buffer, dhPart2->toBytes(), dhPart2->getLength() * WORD_SIZE);
    uint8_t *secondHalf = &(buffer[dhPart2->getLength() * WORD_SIZE]);
    memcpy(secondHalf, peerHello->toBytes(), peerHello->getLength() * WORD_SIZE);

    uint8_t hash[SHA256_DIGEST_LENGTH];
    assert(buffer);
    SHA256(buffer, (dhPart2->getLength() + peerHello->getLength()) * WORD_SIZE, hash);
    commit->setHvi(hash);
    free(buffer);
}

void Zrtp::createTotalHash()
{
    PacketHello *helloR;
    (myRole == Initiator) ? helloR = peerHello : helloR = hello;

    uint8_t *buffer = (uint8_t*)malloc((helloR->getLength() + commit->getLength() +
                                        dhPart1->getLength() + dhPart2->getLength()) * WORD_SIZE);

    memcpy(buffer, helloR->toBytes(), helloR->getLength() * WORD_SIZE);
    uint8_t *nextPos = &(buffer[helloR->getLength() * WORD_SIZE]);
    memcpy(nextPos, commit->toBytes(), commit->getLength() * WORD_SIZE);
    nextPos = &(buffer[(helloR->getLength() + commit->getLength()) * WORD_SIZE]);
    memcpy(nextPos, dhPart1->toBytes(), dhPart1->getLength() * WORD_SIZE);
    nextPos = &(buffer[(helloR->getLength() + commit->getLength() + dhPart1->getLength()) * WORD_SIZE]);
    memcpy(nextPos, dhPart2->toBytes(), dhPart2->getLength() * WORD_SIZE);

    uint8_t hash[SHA256_DIGEST_LENGTH];
    assert(buffer);
    SHA256(buffer, (helloR->getLength() + commit->getLength() +
                    dhPart1->getLength() + dhPart2->getLength()) * WORD_SIZE, hash);
    memcpy(totalHash,hash,SHA256_DIGEST_LENGTH);
    free(buffer);
/*
    std::cout << "Total hash:" << std::endl;
    for(int i = 0; i < SHA256_DIGEST_LENGTH; i++)
    {
        std::cout << totalHash[i];
    }
    std::cout << std::endl;*/
}

void Zrtp::createDHResult()
{
    PacketDHPart *dhPartR;

    BIGNUM *peerPublicKey = BN_new();
    BIGNUM *result = BN_new();

    BN_CTX* ctx = BN_CTX_new();

    dhResult = BN_new();
    (myRole == Initiator) ? dhPartR = dhPart1 : dhPartR = dhPart2;

    BN_bin2bn(dhPartR->getPv(),DH3K_LENGTH,peerPublicKey);
    BN_mod_exp(result,peerPublicKey,privateKey, p, ctx);

    BN_copy(dhResult,result);

    BN_free(peerPublicKey);
    BN_free(result);
    BN_CTX_free(ctx);

    uint8_t *buffer;
    buffer = (uint8_t*)calloc(BN_num_bytes(dhResult), sizeof(uint8_t));

    BN_bn2bin(dhResult,buffer);
/*
    std::cout << std::endl;
    std::cout << "DHResult:" << std::endl;
    for(int i = 0; i < BN_num_bytes(dhResult); i++)
    {
        std::cout << buffer[i];
    }
    std::cout << std::endl;*/
    free(buffer);
}

void Zrtp::sharedSecretCalculation()
{
    createTotalHash();
    createS0();
    createKDFContext();
}

void Zrtp::createKDFContext()
{
    bool initiator = (myRole == Initiator);

    assert(peerHello);

    uint8_t context[KDF_CONTEXT_LENGTH];
    uint8_t *pos;
    pos = &context[0];
    if(initiator)
    {
        memcpy(pos,myZID,ZID_SIZE);
        pos = &context[ZID_SIZE];
        memcpy(pos,peerHello->getZid(),ZID_SIZE);
    }
    else
    {
        memcpy(pos,peerHello->getZid(),ZID_SIZE);
        pos = &context[ZID_SIZE];
        memcpy(pos,myZID,ZID_SIZE);
    }
    pos = &context[2*ZID_SIZE];
    memcpy(pos,totalHash,SHA256_DIGEST_LENGTH);
    memcpy(kdfContext,context,KDF_CONTEXT_LENGTH);
/*
    std::cout << std::endl;
    std::cout << "KDF_Context:" << std::endl;
    for(int i = 0; i < KDF_CONTEXT_LENGTH; i++)
    {
        std::cout << kdfContext[i];
    }
    std::cout << std::endl;*/
}

void Zrtp::createS0()
{
    int32_t counter = 1;
    bool initiator = (myRole == Initiator);

    uint8_t *buffer = (uint8_t*)malloc(sizeof(int32_t) + BN_num_bytes(dhResult)
                                       + 13*sizeof(uint8_t) + 2*ZID_SIZE + SHA256_DIGEST_LENGTH
                                       + 3*ID_SIZE + 3*sizeof(int32_t));
    uint8_t *pos = buffer;
    memcpy(pos, &counter, sizeof(int32_t));
    pos += sizeof(int32_t);
    BN_bn2bin(dhResult,pos);
    pos += BN_num_bytes(dhResult);
    memcpy(pos, "ZRTP-HMAC-KDF",13);
    pos += 13*sizeof(uint8_t);
    if(initiator)
    {
        memcpy(pos,myZID,ZID_SIZE);
        pos += ZID_SIZE;
        memcpy(pos,peerHello->getZid(),ZID_SIZE);
    }
    else
    {
        memcpy(pos,peerHello->getZid(),ZID_SIZE);
        pos += ZID_SIZE;
        memcpy(pos,myZID,ZID_SIZE);
    }
    pos += ZID_SIZE;
    memcpy(pos,totalHash,SHA256_DIGEST_LENGTH);
    pos += SHA256_DIGEST_LENGTH;

    //s1,s2,s3 = NULL
    memset(pos,0,3*sizeof(int32_t));
    pos += 3*sizeof(int32_t);

    int32_t bufferLength = pos - buffer;

    uint8_t hash[SHA256_DIGEST_LENGTH];

    SHA256(buffer,bufferLength,hash);

    memcpy(s0,hash,SHA256_DIGEST_LENGTH);
    BN_clear(dhResult);

    /*std::cout << std::endl;
    std::cout << "S0:" << std::endl;
    for(int i = 0; i < SHA256_DIGEST_LENGTH; i++)
    {
        std::cout << s0[i];
    }
    std::cout << std::endl;*/
}

void Zrtp::kdf(uint8_t *key, uint8_t *label, int32_t labelLength, uint8_t *context, int32_t lengthL, uint8_t *derivedKey)
{
    int32_t counter = 1;
    uint8_t ki[SHA256_DIGEST_LENGTH];
    memcpy(ki,key,SHA256_DIGEST_LENGTH);

    int32_t bufferLength = sizeof(int32_t) + labelLength + sizeof(uint8_t) + KDF_CONTEXT_LENGTH + sizeof(int32_t);
    uint8_t *buffer = (uint8_t*)malloc(bufferLength);
    uint8_t *pos = buffer;
    memcpy(pos,&counter,sizeof(int32_t));
    pos += sizeof(int32_t);
    memcpy(pos,label,labelLength);
    pos += labelLength;
    memset(pos,0,sizeof(uint8_t));
    pos += sizeof(uint8_t);
    memcpy(pos,context,KDF_CONTEXT_LENGTH);
    pos += KDF_CONTEXT_LENGTH;
    memcpy(pos,&lengthL,sizeof(int32_t));

    uint8_t* digest;
    digest = HMAC(EVP_sha256(), ki, lengthL,
                  buffer, bufferLength, NULL, NULL);

    assert(digest);

    uint8_t computedMac[lengthL + 1];
    for(int i = 0; i < lengthL; i++)
    {
        sprintf((char*)&computedMac[i*2], "%02x", (unsigned int)digest[i]);
    }
    memcpy(derivedKey,computedMac,lengthL);
    free(buffer);
}

void Zrtp::keyDerivation()
{
    //generate ZRTPSess
    kdf(s0,(uint8_t*)"ZRTP Session Key",16, kdfContext,SHA256_DIGEST_LENGTH, zrtpSess);

    //generate sashash and sasvalue
    kdf(s0,(uint8_t*)"SAS",3,kdfContext,SHA256_DIGEST_LENGTH, sasHash);
    memcpy(sasValue,sasHash,32);

    //generate ExportedKey
    kdf(s0,(uint8_t*)"Exported key",12,kdfContext,SHA256_DIGEST_LENGTH, exportedKey);

    //generate srtpkeys
    kdf(s0,(uint8_t*)"Initiator SRTP master key",25,kdfContext,AES1_KEY_LENGTH,srtpKeyI);
    kdf(s0,(uint8_t*)"Responder SRTP master key",25,kdfContext,AES1_KEY_LENGTH,srtpKeyR);

    //generate srtpsalts
    kdf(s0,(uint8_t*)"Initiator SRTP master salt",26,kdfContext,SALT_KEY_LENGTH,srtpSaltI);
    kdf(s0,(uint8_t*)"Responder SRTP master salt",26,kdfContext,SALT_KEY_LENGTH,srtpSaltR);

    //generate mackeys
    kdf(s0,(uint8_t*)"Initiator HMAC key",18,kdfContext,SHA256_DIGEST_LENGTH,macKeyI);
    kdf(s0,(uint8_t*)"Responder HMAC key",18,kdfContext,SHA256_DIGEST_LENGTH,macKeyR);

    //generate zrtpkeys
    kdf(s0,(uint8_t*)"Initiator ZRTP key",18,kdfContext,AES1_KEY_LENGTH,zrtpKeyI);
    kdf(s0,(uint8_t*)"Responder ZRTP key",18,kdfContext,AES1_KEY_LENGTH,zrtpKeyR);
}

void Zrtp::encryptConfirmData()
{
    uint8_t started[40] = {0};
    uint8_t encrypted[40] = {0};
    int outputLength;
    int cipherTextLen;
    uint8_t iv[VECTOR_SIZE];

    EVP_CIPHER_CTX *ctx;
    ctx = EVP_CIPHER_CTX_new();

    if (myRole == Initiator)
    {
        uint8_t *buffer = confirm2->toBytes();

        memcpy(iv,confirm2->getVector(),VECTOR_SIZE);

        uint8_t *pos = &(buffer[36]);
        memcpy(started,pos,40);

        EVP_EncryptInit_ex(ctx, EVP_aes_128_cfb128(), 0, zrtpKeyI, iv);

        EVP_EncryptUpdate(ctx, encrypted, &outputLength, started, 40);

        cipherTextLen = outputLength;

        EVP_EncryptFinal_ex(ctx, encrypted + outputLength, &outputLength);

        cipherTextLen += outputLength;

        assert(cipherTextLen == 40);

        EVP_CIPHER_CTX_free(ctx);

        confirm2->setEncryptedPart(encrypted);
    }
    else    //Responder
    {
        uint8_t *buffer = confirm1->toBytes();

        memcpy(iv,confirm1->getVector(),VECTOR_SIZE);

        uint8_t *pos = &(buffer[36]);
        memcpy(started,pos,40);

        EVP_EncryptInit_ex(ctx, EVP_aes_128_cfb128(), 0, zrtpKeyR, iv);

        EVP_EncryptUpdate(ctx, encrypted, &outputLength, started, 40);

        cipherTextLen = outputLength;

        EVP_EncryptFinal_ex(ctx, encrypted + outputLength, &outputLength);

        cipherTextLen += outputLength;

        assert(cipherTextLen == 40);

        EVP_CIPHER_CTX_free(ctx);

        confirm1->setEncryptedPart(encrypted);
    }
}

void Zrtp::decryptConfirmData(uint8_t *data)
{
    uint8_t *pos = &(data[36]);

    uint8_t encrypted[40] = {0};
    memcpy(encrypted,pos,40);
    uint8_t decrypted[40] = {0};
    int outputLength;
    int plainTextLen;
    uint8_t iv[VECTOR_SIZE];

    EVP_CIPHER_CTX *ctx;
    ctx = EVP_CIPHER_CTX_new();

    if (myRole == Initiator)
    {
        memcpy(iv,confirm1->getVector(),VECTOR_SIZE);

        EVP_DecryptInit_ex(ctx, EVP_aes_128_cfb128(), 0, zrtpKeyR, iv);
    }
    else
    {
        memcpy(iv,confirm2->getVector(),VECTOR_SIZE);

        EVP_DecryptInit_ex(ctx, EVP_aes_128_cfb128(), 0, zrtpKeyI, iv);
    }
    EVP_DecryptUpdate(ctx, decrypted, &outputLength, encrypted, 40);

    plainTextLen = outputLength;

    EVP_DecryptFinal_ex(ctx, decrypted + outputLength, &outputLength);

    plainTextLen += outputLength;

    assert(plainTextLen == 40);

    EVP_CIPHER_CTX_free(ctx);
    //parsing of encrypted part
    uint8_t hashH0[HASHIMAGE_SIZE];
    uint16_t sigLen;
    uint32_t expInterval;

    pos = &(decrypted[0]);
    for (uint8_t i = 0; i < HASHIMAGE_SIZE; i++)
    {
        hashH0[i] = *(pos++);
    }
    pos ++;

    sigLen = *pos << 8 | *(pos + 1);
    pos += 3;
    expInterval = *pos << 24 | *(pos + 1) << 16 | *(pos + 2) << 8 | *(pos + 3);
    if(myRole == Initiator)
    {
        confirm1->setH0(hashH0);
        confirm1->setSigLen(sigLen);
        confirm1->setExpInterval(expInterval);
    }
    else
    {
        confirm2->setH0(hashH0);
        confirm2->setSigLen(sigLen);
        confirm2->setExpInterval(expInterval);
    }
}

void Zrtp::setPv(PacketDHPart *packet)
{
    assert(publicKey);
    uint8_t *buffer = (uint8_t*)malloc((BN_num_bytes(publicKey)+1) * sizeof(uint8_t));
    buffer[BN_num_bytes(publicKey)] = '\0';

    BN_bn2bin(publicKey,buffer);

    packet->setPv(buffer);
    free(buffer);
}
