#include "zrtp.h"

Zrtp::Zrtp(uint8_t *zid, ZrtpCallback *cb, Role role, std::string clientId)
{
    myZID = zid;
    callback = cb;
    myRole = role;

    createHashImages();

    engine = new StateEngine(this);
    hello = new PacketHello();
    hello->setClientId(clientId);
    hello->setH3(h3);
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

void Zrtp::createHashImages()
{
    RAND_bytes(h0,HASHIMAGE_SIZE);

    SHA256(h0,HASHIMAGE_SIZE,h1);
    SHA256(h1,HASHIMAGE_SIZE,h2);
    SHA256(h2,HASHIMAGE_SIZE,h3);
}
