#include "zrtp.h"

Zrtp::Zrtp(ZrtpCallback *cb, Role role, std::string clientId)
{
    callback = cb;
    myRole = role;

    RAND_bytes(myZID,ZID_SIZE);
    createHashImages();

    engine = new StateEngine(this);

    createHelloPacket(clientId);

    helloAck = new PacketHelloAck();
    commit = new PacketCommit();
    dhPart1 = new PacketDHPart();
    dhPart1->setType((uint8_t*)"DHPart1 ");
    dhPart2 = new PacketDHPart();
    dhPart2->setType((uint8_t*)"DHPart2 ");
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
    createHelloMac();
}

void Zrtp::createHashImages()
{
    RAND_bytes(h0,HASHIMAGE_SIZE);

    SHA256(h0,HASHIMAGE_SIZE,h1);
    SHA256(h1,HASHIMAGE_SIZE,h2);
    SHA256(h2,HASHIMAGE_SIZE,h3);
}

void Zrtp::createHelloMac()
{
    uint8_t key[HASHIMAGE_SIZE];
    memcpy(key,h2,HASHIMAGE_SIZE);

    uint8_t *data = hello->toBytes();
    uint16_t length = hello->getLength() - 2;

    data[(length) * WORD_SIZE] = '\0';

    uint8_t* digest;
    digest = HMAC(EVP_sha256(), key, HASHIMAGE_SIZE,
                  data, length, NULL, NULL);

    uint8_t computedMac[MAC_SIZE];
    for(int i = 0; i < 4; i++)
    {
        sprintf((char*)&computedMac[i*2], "%02x", (unsigned int)digest[i]);
    }
    hello->setMac(computedMac);
}
