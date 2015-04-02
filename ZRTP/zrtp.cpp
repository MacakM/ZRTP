#include "zrtp.h"

Zrtp::Zrtp(ZrtpCallback *cb, Role role, std::string clientId, UserInfo *info)
{
    if(!cb)
    {
        throw std::invalid_argument("NULL callback");
    }
    userInfo = info;
    callback = cb;
    myRole = role;

    RAND_bytes(myZID,ZID_SIZE);
    generateHashImages();

    RAND_bytes(rs1,ID_SIZE);
    RAND_bytes(rs2,ID_SIZE);
    RAND_bytes(auxsecret,ID_SIZE);
    RAND_bytes(pbxsecret,ID_SIZE);

    engine = new StateEngine(this);

    assert(engine);

    createHelloPacket(clientId);

    assert(hello);

    Event event;
    event.type = Start;
    event.messageLength = 0;
    engine->processEvent(&event);
}

Zrtp::~Zrtp()
{
    if(engine)
    {
        delete engine;
    }
}

void Zrtp::endZrtp()
{
    if(hello)
    {
        delete(hello);
    }
    if(peerHello)
    {
        delete(peerHello);
    }
    if(commit)
    {
        delete(commit);
    }
    if(dhPart1)
    {
        delete(dhPart1);
    }
    if(dhPart2)
    {
        delete(dhPart2);
    }
    if(confirm1)
    {
        delete(confirm1);
    }
    if(confirm2)
    {
        delete(confirm2);
    }
    if(error)
    {
        delete(error);
    }
    if(privateKey)
    {
        BN_clear_free(privateKey);
    }
    if(publicKey)
    {
        BN_clear_free(publicKey);
    }
    if(p)
    {
        BN_clear_free(p);
    }
}

void Zrtp::processMessage(uint8_t *msg, int32_t length)
{
    enterCriticalSection();
    //ignore packet with shorter length than it should be
    if(length < (int32_t)sizeof(Header) - WORD_SIZE)
    {
        leaveCriticalSection();
        return;
    }
    Event event;
    event.type = Message;
    event.message = msg;
    event.messageLength = length;
    engine->processEvent(&event);
    leaveCriticalSection();
}

void Zrtp::processTimeout()
{
    Event event;
    event.type = Timeout;
    event.messageLength = 0;
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

void Zrtp::enterCriticalSection()
{
    callback->enterCriticalSection();
}

void Zrtp::leaveCriticalSection()
{
    callback->leaveCriticalSection();
}

void Zrtp::createHelloPacket(std::string clientId)
{
    hello = new PacketHello();
    hello->setVersion((uint8_t*)chooseHighestVersion().c_str());
    hello->setClientId(clientId);
    hello->setH3(myH3);
    hello->setZid(myZID);
    hello->AddSupportedTypes(userInfo);
    uint8_t *mac = generateMac(hello, true);
    hello->setMac(mac);
    delete(mac);
}

void Zrtp::createCommitPacket()
{
    commit = new PacketCommit();
    commit->setH2(myH2);
    commit->setZid(myZID);
    commit->setKeyAgreement(keyAgreement);
    memcpy(hash,(uint8_t*)"S256",WORD_SIZE);
    commit->setHash(hash);
    memcpy(cipher,(uint8_t*)"AES1",WORD_SIZE);
    commit->setCipher(cipher);
    memcpy(authTag,(uint8_t*)"HS32",WORD_SIZE);
    commit->setAuthTag(authTag);
    memcpy(sas,(uint8_t*)"B32 ",WORD_SIZE);
    commit->setSas(sas);
    createDHPart2Packet();
    uint8_t *myHvi = generateHvi();
    commit->setHvi(myHvi);
    delete(myHvi);
    uint8_t *mac = generateMac(commit, true);
    commit->setMac(mac);
    delete(mac);
}

void Zrtp::createDHPart1Packet()
{
    dhPart1 = new PacketDHPart();
    dhPart1->setType((uint8_t*)"DHPart1 ");
    dhPart1->setH1(myH1);
    generateIds(dhPart1);
    setPv(dhPart1);
    uint8_t *mac = generateMac(dhPart1, true);
    dhPart1->setMac(mac);
    delete(mac);
}

void Zrtp::createDHPart2Packet()
{
    dhPart2 = new PacketDHPart();
    dhPart2->setType((uint8_t*)"DHPart2 ");
    dhPart2->setH1(myH1);
    generateIds(dhPart2);
    setPv(dhPart2);
    uint8_t *mac = generateMac(dhPart2, true);
    dhPart2->setMac(mac);
    delete(mac);
}

void Zrtp::createConfirm1Packet()
{
    confirm1 = new PacketConfirm();
    confirm1->setType((uint8_t*)"Confirm1");
    confirm1->setH0(myH0);
    confirm1->setSigLen(0);
    confirm1->setExpInterval(0);

    uint8_t vector[VECTOR_SIZE];
    RAND_bytes(vector,VECTOR_SIZE);
    confirm1->setInitVector(vector);

    encryptConfirmData();
    uint8_t *confirmMac = generateConfirmMac(confirm1,true);
    confirm1->setConfirmMac(confirmMac);
    delete(confirmMac);
}

void Zrtp::createConfirm2Packet()
{
    confirm2 = new PacketConfirm();
    confirm2->setType((uint8_t*)"Confirm2");
    confirm2->setH0(myH0);
    confirm2->setSigLen(0);
    confirm2->setExpInterval(0);

    uint8_t vector[VECTOR_SIZE];
    RAND_bytes(vector,VECTOR_SIZE);
    confirm2->setInitVector(vector);

    encryptConfirmData();
    uint8_t *confirmMac = generateConfirmMac(confirm2,true);
    confirm2->setConfirmMac(confirmMac);
    delete(confirmMac);
}

void Zrtp::generateHashImages()
{
    RAND_bytes(myH0,HASHIMAGE_SIZE);

    SHA256(myH0,HASHIMAGE_SIZE,myH1);
    SHA256(myH1,HASHIMAGE_SIZE,myH2);
    SHA256(myH2,HASHIMAGE_SIZE,myH3);
    return;
}

void Zrtp::calculatePeerH2(uint8_t *peerH1)
{
    SHA256(peerH1,HASHIMAGE_SIZE,peerH2);
    return;
}

bool Zrtp::isCorrectHashImage(uint8_t *previousH, uint8_t *actualH)
{
    uint8_t computedHash[HASHIMAGE_SIZE];
    SHA256(actualH,HASHIMAGE_SIZE,computedHash);
    if(memcmp(previousH,computedHash,HASHIMAGE_SIZE) == 0)
    {
        return true;
    }
    return false;
}

void Zrtp::generateIds(PacketDHPart *packet)
{
    if(memcmp(hash,"S256",WORD_SIZE) == 0)
    {
        uint8_t id[SHA256_DIGEST_LENGTH];
        SHA256(rs1,ID_SIZE,id);
        packet->setRs1Id(id);
        SHA256(rs2,ID_SIZE,id);
        packet->setRs2Id(id);
        SHA256(auxsecret,ID_SIZE,id);
        packet->setAuxsecretId(id);
        SHA256(pbxsecret,ID_SIZE,id);
        packet->setPbxsecretId(id);
    }
}

uint8_t *Zrtp::generateMac(Packet *packet, bool sending)
{
    uint8_t key[HASHIMAGE_SIZE];

    switch(packet->getType()[0])
    {
    //Hello
    case 'H':
        sending ? memcpy(key,myH2,HASHIMAGE_SIZE) : memcpy(key,peerH2,HASHIMAGE_SIZE);
        break;
    //Commit
    case 'C':
        sending ? memcpy(key,myH1,HASHIMAGE_SIZE) : memcpy(key,peerH1,HASHIMAGE_SIZE);
        break;
    //DHPart1 or DHPart2
    case 'D':
        sending ? memcpy(key,myH0,HASHIMAGE_SIZE) : memcpy(key,peerH0,HASHIMAGE_SIZE);
        break;
    }

    uint8_t *buffer = packet->toBytes();

    assert(buffer);

    uint16_t length = (packet->getLength() - 2) * WORD_SIZE;

    uint8_t* digest = HMAC(EVP_sha256(), key, HASHIMAGE_SIZE,
                  buffer, length, NULL, NULL);

    assert(digest);

    uint8_t *computedMac = new uint8_t[MAC_SIZE + 1];
    for(int i = 0; i < 4; i++)
    {
        sprintf((char*)&computedMac[i*2], "%02x", (unsigned int)digest[i]);
    }
    return computedMac;
}

uint8_t *Zrtp::generateConfirmMac(PacketConfirm *packet, bool sending)
{
    uint8_t key[SHA256_DIGEST_LENGTH];
    if(myRole == Initiator)
    {
        (sending) ? memcpy(&key,macKeyI,SHA256_DIGEST_LENGTH) : memcpy(&key,macKeyR,SHA256_DIGEST_LENGTH);
    }
    else
    {
        (sending) ? memcpy(&key,macKeyR,SHA256_DIGEST_LENGTH) : memcpy(&key,macKeyI,SHA256_DIGEST_LENGTH);
    }

    uint8_t *buffer = packet->toBytes();
    uint16_t bufferLength = packet->getLength() * WORD_SIZE;

    uint8_t *startHash = &(buffer[9*WORD_SIZE]);

    uint8_t* digest = HMAC(EVP_sha256(), key, HASHIMAGE_SIZE,
                  startHash, bufferLength - 9*WORD_SIZE, NULL, NULL);

    assert(digest);

    uint8_t *computedMac = new uint8_t[MAC_SIZE + 1];
    for(int i = 0; i < 4; i++)
    {
        sprintf((char*)&computedMac[i*2], "%02x", (unsigned int)digest[i]);
    }
    return computedMac;
}

bool Zrtp::chooseAlgorithm()
{
    bool found = false;
    uint8_t myKeyCount = hello->getKeyCount();
    std::string myAlgorithms ((char*)hello->getKeyAgreementTypes(), myKeyCount * WORD_SIZE);

    uint8_t peerKeyCount = peerHello->getKeyCount();
    std::string peerAlgorithms ((char*)peerHello->getKeyAgreementTypes(),peerKeyCount * WORD_SIZE);

    std::string myType;
    std::string peerType;

    //find my preferred type that peer supports
    for(uint8_t i = 0; i < myKeyCount; i++)
    {
        myType = myAlgorithms.substr(i * WORD_SIZE, WORD_SIZE);
        if(peerAlgorithms.find(myType) != std::string::npos)
        {
            found = true;
            break;
        }
    }
    if(found == false)
    {
        return false;
    }
    found = false;

    //find peer preferred type that I support
    for(uint8_t i = 0; i < peerKeyCount; i++)
    {
        peerType = peerAlgorithms.substr(i * WORD_SIZE, WORD_SIZE);
        if(myAlgorithms.find(peerType) != std::string::npos)
        {
            found = true;
            break;
        }
    }
    if(found == false)
    {
        return false;
    }

    std::string chosenType = chooseFasterType(myType,peerType);
    memcpy(keyAgreement,chosenType.c_str(),WORD_SIZE);
    return true;
}

std::string Zrtp::chooseFasterType(std::string firstType, std::string secondType)
{
    if(strcmp(firstType.c_str(),secondType.c_str()) == 0)
    {
        return firstType;
    }

    uint8_t firstRating = 0;
    uint8_t secondRating = 0;

    if(strcmp(firstType.c_str(),"DH2k") == 0)
    {
        firstRating = 1;
    }
    else if(strcmp(firstType.c_str(),"EC25") == 0)
    {
        firstRating = 2;
    }
    else if(strcmp(firstType.c_str(),"DH3k") == 0)
    {
        firstRating = 3;
    }
    else if(strcmp(firstType.c_str(),"EC38") == 0)
    {
        firstRating = 4;
    }
    else if(strcmp(firstType.c_str(),"EC52") == 0)
    {
        firstRating = 5;
    }

    if(strcmp(secondType.c_str(),"DH2k") == 0)
    {
        secondRating = 1;
    }
    else if(strcmp(secondType.c_str(),"EC25") == 0)
    {
        secondRating = 2;
    }
    else if(strcmp(secondType.c_str(),"DH3k") == 0)
    {
        secondRating = 3;
    }
    else if(strcmp(secondType.c_str(),"EC38") == 0)
    {
        secondRating = 4;
    }
    else if(strcmp(secondType.c_str(),"EC52") == 0)
    {
        secondRating = 5;
    }

    if(firstRating > secondRating)
    {
        return secondType;
    }
    return firstType;
}

bool Zrtp::checkCompatibility(uint32_t *errorCode)
{
    //check hash algorithm
    std::string receivedHash ((char*)commit->getHash(),WORD_SIZE);

    if(std::find(userInfo->hashTypes.begin(), userInfo->hashTypes.end(), receivedHash)
            == userInfo->hashTypes.end())
    {
        *errorCode = HashTypeNotSupported;
        return false;
    }
    else
    {
        memcpy(hash,commit->getHash(),WORD_SIZE);
    }
    //check cipher algorithm
    std::string receivedCipher ((char*)commit->getCipher(),WORD_SIZE);

    if(std::find(userInfo->cipherTypes.begin(), userInfo->cipherTypes.end(), receivedCipher)
            == userInfo->cipherTypes.end())
    {
        *errorCode = CipherTypeNotSupported;
        return false;
    }
    else
    {
        memcpy(cipher,commit->getCipher(),WORD_SIZE);
    }
    //check authTag type
    std::string receivedAuthTag ((char*)commit->getAuthTag(),WORD_SIZE);

    if(std::find(userInfo->authTagTypes.begin(), userInfo->authTagTypes.end(), receivedAuthTag)
            == userInfo->authTagTypes.end())
    {
        *errorCode = AuthTagNotSupported;
        return false;
    }
    else
    {
        memcpy(authTag,commit->getAuthTag(),WORD_SIZE);
    }
    //check key agreemenet type
    std::string receivedKeyAgreement ((char*)commit->getKeyAgreement(),WORD_SIZE);

    if(std::find(userInfo->keyAgreementTypes.begin(), userInfo->keyAgreementTypes.end(), receivedKeyAgreement)
            == userInfo->keyAgreementTypes.end())
    {
        *errorCode = KeyExchangeNotSupported;
        return false;
    }
    else
    {
        memcpy(keyAgreement,commit->getKeyAgreement(),WORD_SIZE);
    }
    //check sas type
    std::string receivedSas ((char*)commit->getSas(),WORD_SIZE);

    if(std::find(userInfo->sasTypes.begin(), userInfo->sasTypes.end(), receivedSas)
            == userInfo->sasTypes.end())
    {
        *errorCode = SasNotSupported;
        return false;
    }
    else
    {
        memcpy(sas,commit->getSas(),WORD_SIZE);
    }
    return true;
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

    DH_free(dh);
}

uint8_t *Zrtp::generateHvi()
{
    PacketHello *helloR;
    (myRole == Initiator) ? helloR = peerHello : helloR = hello;

    assert(dhPart2);
    uint8_t *buffer = new uint8_t[(dhPart2->getLength() + helloR->getLength()) * WORD_SIZE];

    memcpy(buffer, dhPart2->toBytes(), dhPart2->getLength() * WORD_SIZE);
    uint8_t *secondHalf = &(buffer[dhPart2->getLength() * WORD_SIZE]);
    memcpy(secondHalf, helloR->toBytes(), helloR->getLength() * WORD_SIZE);

    uint8_t *hash = new uint8_t[SHA256_DIGEST_LENGTH];
    assert(buffer);
    SHA256(buffer, (dhPart2->getLength() + helloR->getLength()) * WORD_SIZE, hash);
    delete(buffer);
    return hash;
}

void Zrtp::setPv(PacketDHPart *packet)
{
    assert(publicKey);
    uint8_t *buffer = new uint8_t[(BN_num_bytes(publicKey)+1) * sizeof(uint8_t)];
    buffer[BN_num_bytes(publicKey)] = '\0';

    BN_bn2bin(publicKey,buffer);

    packet->setPv(buffer);
    delete(buffer);
}

bool Zrtp::isValidPeerPv()
{
    PacketDHPart *packet;
    (myRole == Initiator) ? packet = dhPart1 : packet = dhPart2;

    BIGNUM *peerPublicKey = BN_new();
    BIGNUM *testingNumber = BN_new();

    BN_bin2bn(packet->getPv(),DH3K_LENGTH,peerPublicKey);

    BN_hex2bn(&testingNumber,"1");

    //check publicKey != 1
    if(BN_cmp(peerPublicKey,testingNumber) == 0)
    {
        BN_clear_free(peerPublicKey);
        BN_free(testingNumber);
        return false;
    }

    BN_sub(testingNumber,p,testingNumber);

    //check publicKey != p - 1
    if(BN_cmp(peerPublicKey,testingNumber) == 0)
    {
        BN_clear_free(peerPublicKey);
        BN_free(testingNumber);
        return false;
    }

    BN_clear_free(peerPublicKey);
    BN_free(testingNumber);
    return true;
}

void Zrtp::createTotalHash()
{
    PacketHello *helloR;
    (myRole == Initiator) ? helloR = peerHello : helloR = hello;

    uint8_t *buffer = new uint8_t[(helloR->getLength() + commit->getLength() +
                                        dhPart1->getLength() + dhPart2->getLength()) * WORD_SIZE];

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
    delete(buffer);
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

    BN_clear_free(peerPublicKey);
    BN_clear_free(result);
    BN_CTX_free(ctx);
/*
    uint8_t *buffer;
    buffer = (uint8_t*)calloc(BN_num_bytes(dhResult), sizeof(uint8_t));

    BN_bn2bin(dhResult,buffer);

    std::cout << std::endl;
    std::cout << "DHResult:" << std::endl;
    for(int i = 0; i < BN_num_bytes(dhResult); i++)
    {
        std::cout << buffer[i];
    }
    std::cout << std::endl;

    free(buffer);*/
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

void Zrtp::sharedSecretCalculation()
{
    createTotalHash();
    createS0();
    createKDFContext();
}

void Zrtp::createS0()
{
    int32_t counter = 1;
    bool initiator = (myRole == Initiator);

    uint8_t *buffer = new uint8_t[sizeof(int32_t) + BN_num_bytes(dhResult)
                                       + 13*sizeof(uint8_t) + 2*ZID_SIZE + SHA256_DIGEST_LENGTH
                                       + 3*ID_SIZE + 3*sizeof(int32_t)];
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
    delete(buffer);
    BN_clear_free(dhResult);
}

void Zrtp::kdf(uint8_t *key, uint8_t *label, int32_t labelLength, uint8_t *context, int32_t lengthL, uint8_t *derivedKey)
{
    int32_t counter = 1;
    uint8_t ki[SHA256_DIGEST_LENGTH];
    memcpy(ki,key,SHA256_DIGEST_LENGTH);

    int32_t bufferLength = sizeof(int32_t) + labelLength + sizeof(uint8_t) + KDF_CONTEXT_LENGTH + sizeof(int32_t);
    uint8_t *buffer = new uint8_t[bufferLength];
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

    uint8_t* digest = HMAC(EVP_sha256(), ki, lengthL,
                  buffer, bufferLength, NULL, NULL);

    assert(digest);

    uint8_t computedMac[lengthL + 1];
    for(int i = 0; i < lengthL; i++)
    {
        sprintf((char*)&computedMac[i*2], "%02x", (unsigned int)digest[i]);
    }
    memcpy(derivedKey,computedMac,lengthL);
    delete(buffer);
}

void Zrtp::keyDerivation()
{
    //generate ZRTPSess
    kdf(s0,(uint8_t*)"ZRTP Session Key",16, kdfContext,SHA256_DIGEST_LENGTH, zrtpSess);

    //generate sashash and sasvalue
    kdf(s0,(uint8_t*)"SAS",3,kdfContext,SHA256_DIGEST_LENGTH, sasHash);
    sasValue = sasHash[0] | (sasHash[1] << 8) | (sasHash[2] << 16) | (sasHash[3] << 24);

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
    uint8_t started[ENCRYPTED_PART_LENGTH] = {0};
    uint8_t encrypted[ENCRYPTED_PART_LENGTH] = {0};
    int outputLength;
    int cipherTextLen;
    uint8_t iv[VECTOR_SIZE];

    EVP_CIPHER_CTX *ctx;
    ctx = EVP_CIPHER_CTX_new();

    if (myRole == Initiator)
    {
        assert(confirm2);
        uint8_t *buffer = confirm2->toBytes();

        memcpy(iv,confirm2->getVector(),VECTOR_SIZE);

        uint8_t *pos = &(buffer[36]);
        memcpy(started,pos,ENCRYPTED_PART_LENGTH);

        EVP_EncryptInit_ex(ctx, EVP_aes_128_cfb128(), 0, zrtpKeyI, iv);

        EVP_EncryptUpdate(ctx, encrypted, &outputLength, started, ENCRYPTED_PART_LENGTH);

        cipherTextLen = outputLength;

        EVP_EncryptFinal_ex(ctx, encrypted + outputLength, &outputLength);

        cipherTextLen += outputLength;

        assert(cipherTextLen == ENCRYPTED_PART_LENGTH);

        confirm2->setEncryptedPart(encrypted);
    }
    else    //Responder
    {
        assert(confirm1);
        uint8_t *buffer = confirm1->toBytes();

        memcpy(iv,confirm1->getVector(),VECTOR_SIZE);

        uint8_t *pos = &(buffer[36]);
        memcpy(started,pos,ENCRYPTED_PART_LENGTH);

        EVP_EncryptInit_ex(ctx, EVP_aes_128_cfb128(), 0, zrtpKeyR, iv);

        EVP_EncryptUpdate(ctx, encrypted, &outputLength, started, ENCRYPTED_PART_LENGTH);

        cipherTextLen = outputLength;

        EVP_EncryptFinal_ex(ctx, encrypted + outputLength, &outputLength);

        cipherTextLen += outputLength;

        assert(cipherTextLen == ENCRYPTED_PART_LENGTH);

        confirm1->setEncryptedPart(encrypted);
    }
    EVP_CIPHER_CTX_free(ctx);
}

void Zrtp::decryptConfirmData(uint8_t *data)
{
    uint8_t *pos = &(data[36]);

    uint8_t encrypted[ENCRYPTED_PART_LENGTH] = {0};
    memcpy(encrypted,pos,ENCRYPTED_PART_LENGTH);
    uint8_t decrypted[ENCRYPTED_PART_LENGTH] = {0};
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
    else //Responder
    {
        memcpy(iv,confirm2->getVector(),VECTOR_SIZE);

        EVP_DecryptInit_ex(ctx, EVP_aes_128_cfb128(), 0, zrtpKeyI, iv);
    }
    EVP_DecryptUpdate(ctx, decrypted, &outputLength, encrypted, ENCRYPTED_PART_LENGTH);

    plainTextLen = outputLength;

    EVP_DecryptFinal_ex(ctx, decrypted + outputLength, &outputLength);

    plainTextLen += outputLength;

    assert(plainTextLen == ENCRYPTED_PART_LENGTH);

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
    else //Responder
    {
        confirm2->setH0(hashH0);
        confirm2->setSigLen(sigLen);
        confirm2->setExpInterval(expInterval);
    }
}

std::string Zrtp::chooseHighestVersion()
{
    std::string max = userInfo->versions.at(0);
    for(uint8_t i = 0; i < userInfo->versions.size();i++)
    {
        std::string value = userInfo->versions.at(i);
        if (strcmp(value.c_str(),max.c_str()) > 0)
        {
            max = value;
        }
    }
    return max;
}

bool Zrtp::compareVersions()
{
    bool changedVersion = false;
    char buffer[5];
    memcpy(buffer,peerHello->getVersion(),4);
    buffer[4] = '\0';
    std::string peerVersion(buffer);
    std::string myVersion = chooseHighestVersion();

    if(myVersion.compare(peerVersion) == 0)
    {
        return true;
    }

    if(myVersion.compare(peerVersion) > 0)
    {
        for(uint8_t i = 0; i < userInfo->versions.size(); i++)
        {
            std::string value = userInfo->versions.at(i);
            //erase all versions higher then peerVersion
            if(strcmp(value.c_str(),peerVersion.c_str()) > 0)
            {
                userInfo->versions.erase(userInfo->versions.begin() + i);
                changedVersion = true;
            }
        }
        if(userInfo->versions.size() != 0 && changedVersion)
        {
            //get clientID from actual Hello in order to create new Hello
            char *buffer = (char*)hello->getClientId();
            std::string id(buffer, ID_SIZE);

            delete(hello);
            createHelloPacket(id);
        }
    }
    return false;
}

char *Zrtp::base32(uint32_t bits)
{
    int i, n, shift;
    char *result = new char[4];
    for(i = 0, shift = 27; i!=4; ++i, shift -= 5)
    {
        n = (bits >> shift) & 31;
        result[i] = "ybndrfg8ejkmcpqxot1uwisza345h769"[n];
    }
    return result;
}
