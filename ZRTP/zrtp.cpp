#include "zrtp.h"

Zrtp::Zrtp(ZrtpCallback *cb, Role role, std::string clientId)
{
    callback = cb;
    myRole = role;

    RAND_bytes(myZID,ZID_SIZE);
    createHashImages();

    RAND_bytes(rs1,ID_SIZE);
    RAND_bytes(rs2,ID_SIZE);
    RAND_bytes(auxsecret,ID_SIZE);
    RAND_bytes(pbxsecret,ID_SIZE);

    engine = new StateEngine(this);

    createHelloPacket(clientId);

    peerHello = new PacketHello();
    helloAck = new PacketHelloAck();

    createCommitPacket();



    confirm1 = new PacketConfirm();
    confirm1->setType((uint8_t*)"Confirm1");
    confirm2 = new PacketConfirm();
    confirm2->setType((uint8_t*)"Confirm2");
    conf2Ack = new PacketConf2Ack();

    Event event;
    event.type = Start;
    engine->processEvent(&event);
}

void Zrtp::processMessage(uint8_t *msg, int32_t length)
{
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
}

void Zrtp::createDHPart1Packet()
{
    dhPart1 = new PacketDHPart();
    dhPart1->setType((uint8_t*)"DHPart1 ");
    dhPart1->setH1(h1);
    generateIds(dhPart1);
    diffieHellman(dhPart1);
    createMac(dhPart1);
}

void Zrtp::createDHPart2Packet()
{
    dhPart2 = new PacketDHPart();
    dhPart2->setType((uint8_t*)"DHPart2 ");
    dhPart2->setH1(h1);
    generateIds(dhPart2);
    diffieHellman(dhPart2);
    createMac(dhPart2);
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
    uint8_t *checkingHash = (uint8_t*)"S256";
    if(memcmp(hash,checkingHash,WORD_SIZE) == 0)
    {
        uint8_t id[ID_SIZE];
        SHA256(id,ID_SIZE,rs1);
        packet->setRs1Id(id);
        SHA256(id,ID_SIZE,rs2);
        packet->setRs1Id(id);
        SHA256(id,ID_SIZE,auxsecret);
        packet->setAuxsecretId(id);
        SHA256(id,ID_SIZE,pbxsecret);
        packet->setPbxsecretId(id);
    }
}

void Zrtp::createMac(Packet *packet)
{
    uint8_t key[HASHIMAGE_SIZE];
    uint8_t *checkType = (uint8_t*)"Hello   ";
    if(memcmp(packet->getType(), checkType, TYPE_SIZE) == 0)
    {
        memcpy(key,h2,HASHIMAGE_SIZE);
        checkType = (uint8_t*)"Commit  ";
    }
    else if(memcmp(packet->getType(), checkType, TYPE_SIZE) == 0)
    {
        memcpy(key,h1,HASHIMAGE_SIZE);
        checkType = (uint8_t*)"DHPart1 ";
    }
    else if(memcmp(packet->getType(), checkType, TYPE_SIZE) == 0)
    {
        memcpy(key,h0,HASHIMAGE_SIZE);
        checkType = (uint8_t*)"DHPart2 ";
    }
    else if(memcmp(packet->getType(), checkType, TYPE_SIZE) == 0)
    {
        memcpy(key,h0,HASHIMAGE_SIZE);
    }

    uint8_t *data = packet->toBytes();

    uint16_t length = packet->getLength() - 2;

    data[(length) * WORD_SIZE] = '\0';

    uint8_t* digest;
    digest = HMAC(EVP_sha256(), key, HASHIMAGE_SIZE,
                  data, length, NULL, NULL);

    uint8_t computedMac[MAC_SIZE + 1];
    for(int i = 0; i < 4; i++)
    {
        sprintf((char*)&computedMac[i*2], "%02x", (unsigned int)digest[i]);
    }
    packet->setMac(computedMac);
}

void Zrtp::diffieHellman(PacketDHPart *packet)
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

    BN_copy(privateKey,dh->priv_key);
    BN_copy(publicKey,dh->pub_key);

    uint8_t *buffer = (uint8_t*)malloc((BN_num_bytes(dh->pub_key)+1) * sizeof(uint8_t));
    buffer[BN_num_bytes(dh->pub_key)] = '\0';

    BN_bn2bin(dh->pub_key,buffer);

    packet->setPv(buffer);

    DH_free(dh);
}

void Zrtp::generateHvi()
{

}
