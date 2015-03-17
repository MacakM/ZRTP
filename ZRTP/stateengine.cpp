#include "stateengine.h"

StateEngine::StateEngine(Zrtp *zrtp)
{
    initHandlers();
    this->zrtp = zrtp;

    T1.start = 50;
    T1.cap = 200;
    T1.maxResend = 20;

    T2.start = 150;
    T2.cap = 1200;
    T2.maxResend = 10;

    actualState = Initial;
}

void StateEngine::initHandlers()
{
    handlers[Initial] = &StateEngine::handleInitial;
    handlers[SentHello] = &StateEngine::handleSentHello;
    handlers[SentHelloAck] = &StateEngine::handleSentHelloAck;
    handlers[ReceivedHelloAck] = &StateEngine::handleReceivedHelloAck;
    handlers[SentCommit] = &StateEngine::handleSentCommit;
    handlers[WaitCommit] = &StateEngine::handleWaitCommit;
    handlers[WaitDHPart2] = &StateEngine::handleWaitDHPart2;
    handlers[WaitConfirm1] = &StateEngine::handleWaitConfirm1;
    handlers[WaitConfirm2] = &StateEngine::handleWaitConfirm2;
    handlers[WaitConf2Ack] = &StateEngine::handleWaitConf2Ack;
    handlers[Secured] = &StateEngine::handleSecured;
    handlers[WaitErrorAck] = &StateEngine::handleWaitErrorAck;
}

void StateEngine::timerStart(zrtpTimer *timer)
{
    timer->actualTime = timer->start;
    timer->resendCounter = 0;
    zrtp->activateTimer(timer->start);
}

bool StateEngine::timerNext(zrtpTimer *timer)
{
    timer->actualTime *= 2;
    if(timer->actualTime > timer->cap)
    {
        timer->actualTime = timer->cap;
    }
    timer->resendCounter++;
    if(timer->resendCounter > timer->maxResend)
    {
        return false;
    }
    return zrtp->activateTimer(timer->actualTime);
}

void StateEngine::processEvent(Event *event)
{
    actualEvent = event;
    (this->*handlers[actualState])();
}

void StateEngine::handleInitial()
{
    if(actualEvent->type != Start)
    {
        return;
    }

    uint8_t *message = zrtp->hello->toBytes();
    uint16_t messageLength = zrtp->hello->getLength() * WORD_SIZE;
    zrtp->sendData(message,messageLength);
    sentMessage = message;
    sentMessageLength = messageLength;
    timerStart(&T1);
    actualState = SentHello;
}

void StateEngine::handleSentHello()
{
    if(actualEvent->type == Timeout)
    {
        zrtp->sendData(sentMessage,sentMessageLength);
        if(!timerNext(&T1))
        {
            zrtp->cancelTimer();
        }
    }
    if(actualEvent->type == Message)
    {
        uint8_t *msg = actualEvent->message;
        uint8_t type[TYPE_SIZE];
        memcpy(type,(msg+4),TYPE_SIZE);
        if(memcmp(type,"Hello   ", TYPE_SIZE) == 0)
        {
            zrtp->peerHello = new PacketHello();
            zrtp->peerHello->parse(msg);
            zrtp->createHelloAckPacket();
            uint8_t *message = zrtp->helloAck->toBytes();
            uint16_t messageLength = zrtp->helloAck->getLength() * WORD_SIZE;
            zrtp->sendData(message,messageLength);
            actualState = SentHelloAck;
        }
        if(memcmp(type,"HelloACK", TYPE_SIZE) == 0)
        {
            zrtp->peerHelloAck = new PacketHelloAck();
            zrtp->peerHelloAck->parse(msg);
            zrtp->cancelTimer();
            actualState = ReceivedHelloAck;
        }
    }
}

void StateEngine::handleSentHelloAck()
{
    if(actualEvent->type == Timeout)
    {
        zrtp->sendData(sentMessage,sentMessageLength);
        if(!timerNext(&T1))
        {
            zrtp->cancelTimer();
        }
    }

    if(actualEvent->type == Message)
    {
        uint8_t *msg = actualEvent->message;
        uint8_t type[TYPE_SIZE];
        memcpy(type,(msg+4),TYPE_SIZE);
        if(memcmp(type,"Hello   ", TYPE_SIZE) == 0)
        {
            zrtp->peerHello->parse(msg);
            uint8_t *message = zrtp->helloAck->toBytes();
            uint16_t messageLength = zrtp->helloAck->getLength() * WORD_SIZE;
            zrtp->sendData(message,messageLength);
        }
        if(memcmp(type,"HelloACK", TYPE_SIZE) == 0)
        {
            zrtp->peerHelloAck = new PacketHelloAck();
            zrtp->peerHelloAck->parse(msg);
            zrtp->cancelTimer();
            if(zrtp->myRole == Responder)
            {
                actualState = WaitCommit;
            }
            else
            {
                zrtp->createCommitPacket();
                uint8_t *message = zrtp->commit->toBytes();
                uint16_t messageLength = zrtp->commit->getLength() * WORD_SIZE;
                zrtp->sendData(message,messageLength);
                sentMessage = message;
                sentMessageLength = messageLength;
                timerStart(&T2);
                actualState = SentCommit;
            }
        }
        if(memcmp(type,"Commit  ", TYPE_SIZE) == 0)
        {
            zrtp->commit = new PacketCommit();
            zrtp->commit->parse(msg);
            zrtp->createDHPart1Packet();
            uint8_t *message = zrtp->dhPart1->toBytes();
            uint16_t messageLength = zrtp->dhPart1->getLength() * WORD_SIZE;
            zrtp->sendData(message,messageLength);
            actualState = WaitDHPart2;
        }
    }
}

void StateEngine::handleReceivedHelloAck()
{
    if(actualEvent->type == Message)
    {
        uint8_t *msg = actualEvent->message;
        uint8_t type[TYPE_SIZE];
        memcpy(type,(msg+4),TYPE_SIZE);
        if(memcmp(type,"Hello   ", TYPE_SIZE) == 0)
        {
            zrtp->peerHello = new PacketHello();
            zrtp->peerHello->parse(msg);
            if(zrtp->myRole == Responder)
            {
                zrtp->createHelloAckPacket();
                uint8_t *message = zrtp->helloAck->toBytes();
                uint16_t messageLength = zrtp->helloAck->getLength() * WORD_SIZE;
                zrtp->sendData(message,messageLength);
                actualState = WaitCommit;
            }
            else
            {
                zrtp->createCommitPacket();
                uint8_t *message = zrtp->commit->toBytes();
                uint16_t messageLength = zrtp->commit->getLength() * WORD_SIZE;
                zrtp->sendData(message,messageLength);
                sentMessage = message;
                sentMessageLength = messageLength;
                timerStart(&T2);
                actualState = SentCommit;
            }
        }
    }
}

void StateEngine::handleSentCommit()
{
    if(actualEvent->type == Timeout)
    {
        zrtp->sendData(sentMessage,sentMessageLength);
        if(!timerNext(&T2))
        {
            zrtp->cancelTimer();
        }
    }
    if(actualEvent->type == Message)
    {
        uint8_t *msg = actualEvent->message;
        uint8_t type[TYPE_SIZE];
        memcpy(type,(msg+4),TYPE_SIZE);
        if(memcmp(type,"DHPart1 ", TYPE_SIZE) == 0)
        {
            zrtp->dhPart1 = new PacketDHPart();
            zrtp->dhPart1->parse(msg);
            zrtp->cancelTimer();
            uint8_t *message = zrtp->dhPart2->toBytes();
            uint16_t messageLength = zrtp->dhPart2->getLength() * WORD_SIZE;
            zrtp->sendData(message,messageLength);
            sentMessage = message;
            sentMessageLength = messageLength;

            zrtp->createDHResult();
            zrtp->sharedSecretCalculation();
            zrtp->keyDerivation();

            timerStart(&T2);
            actualState = WaitConfirm1;
        }
    }
}

void StateEngine::handleWaitCommit()
{
    if(actualEvent->type == Message)
    {
        uint8_t *msg = actualEvent->message;
        uint8_t type[TYPE_SIZE];
        memcpy(type,(msg+4),TYPE_SIZE);
        if(memcmp(type,"Hello   ", TYPE_SIZE) == 0)
        {
            zrtp->peerHello->parse(msg);
            uint8_t *message = zrtp->helloAck->toBytes();
            uint16_t messageLength = zrtp->helloAck->getLength() * WORD_SIZE;
            zrtp->sendData(message,messageLength);
        }
        if(memcmp(type,"Commit  ", TYPE_SIZE) == 0)
        {
            zrtp->commit = new PacketCommit();
            zrtp->commit->parse(msg);
            zrtp->createDHPart1Packet();

            uint8_t *message = (uint8_t*) malloc(zrtp->dhPart1->getLength() * WORD_SIZE);
            memcpy(message,zrtp->dhPart1->toBytes(),zrtp->dhPart1->getLength() * WORD_SIZE);

            uint16_t messageLength = zrtp->dhPart1->getLength() * WORD_SIZE;
            zrtp->sendData(message,messageLength);
            actualState = WaitDHPart2;
        }
    }
}

void StateEngine::handleWaitDHPart2()
{
    if(actualEvent->type == Message)
    {
        uint8_t *msg = actualEvent->message;
        uint8_t type[TYPE_SIZE];
        memcpy(type,(msg+4),TYPE_SIZE);
        if(memcmp(type,"Commit  ", TYPE_SIZE) == 0)
        {
            zrtp->commit->parse(msg);
            uint8_t *message = zrtp->dhPart1->toBytes();
            uint16_t messageLength = zrtp->dhPart1->getLength() * WORD_SIZE;
            zrtp->sendData(message,messageLength);
        }
        if(memcmp(type,"DHPart2 ", TYPE_SIZE) == 0)
        {
            zrtp->dhPart2 = new PacketDHPart();
            zrtp->dhPart2->parse(msg);

            zrtp->createDHResult();
            zrtp->sharedSecretCalculation();
            zrtp->keyDerivation();

            zrtp->createConfirm1Packet();
            uint8_t *message = zrtp->confirm1->toBytes();
            uint16_t messageLength = zrtp->confirm1->getLength() * WORD_SIZE;
            zrtp->sendData(message,messageLength);
            actualState = WaitConfirm2;
        }
    }
}

void StateEngine::handleWaitConfirm1()
{
    if(actualEvent->type == Timeout)
    {
        zrtp->sendData(sentMessage,sentMessageLength);
        if(!timerNext(&T2))
        {
            zrtp->cancelTimer();
        }
    }
    if(actualEvent->type == Message)
    {
        uint8_t *msg = actualEvent->message;
        uint8_t type[TYPE_SIZE];
        memcpy(type,(msg+4),TYPE_SIZE);
        if(memcmp(type,"Confirm1", TYPE_SIZE) == 0)
        {
            zrtp->cancelTimer();
            zrtp->createConfirm2Packet();
            uint8_t *message = zrtp->confirm2->toBytes();
            uint16_t messageLength = zrtp->confirm2->getLength() * WORD_SIZE;
            zrtp->sendData(message,messageLength);
            sentMessage = message;
            sentMessageLength = messageLength;
            timerStart(&T2);
            actualState = WaitConf2Ack;
        }
    }
}

void StateEngine::handleWaitConfirm2()
{
    if(actualEvent->type == Message)
    {
        uint8_t *msg = actualEvent->message;
        uint8_t type[TYPE_SIZE];
        memcpy(type,(msg+4),TYPE_SIZE);
        if(memcmp(type,"DHPart2 ", TYPE_SIZE) == 0)
        {
            zrtp->dhPart2->parse(msg);
            uint8_t *message = zrtp->confirm1->toBytes();
            uint16_t messageLength = zrtp->confirm1->getLength() * WORD_SIZE;
            zrtp->sendData(message,messageLength);
        }
        if(memcmp(type,"Confirm2", TYPE_SIZE) == 0)
        {
            uint8_t *message = zrtp->conf2Ack->toBytes();
            uint16_t messageLength = zrtp->conf2Ack->getLength() * WORD_SIZE;
            zrtp->sendData(message,messageLength);
            std::cout << "DONE";
            actualState = Secured;
        }
    }
}

void StateEngine::handleWaitConf2Ack()
{
    if(actualEvent->type == Timeout)
    {
        zrtp->sendData(sentMessage,sentMessageLength);
        if(!timerNext(&T2))
        {
            zrtp->cancelTimer();
        }
    }
    if(actualEvent->type == Message)
    {
        uint8_t *msg = actualEvent->message;
        uint8_t type[TYPE_SIZE];
        memcpy(type,(msg+4),TYPE_SIZE);
        if(memcmp(type,"Conf2ACK", TYPE_SIZE) == 0)
        {
            zrtp->cancelTimer();
            std::cout << "DONE";
            actualState = Secured;
        }
    }
}

void StateEngine::handleSecured()
{
    if(actualEvent->type == Message)
    {
        uint8_t *msg = actualEvent->message;
        uint8_t type[TYPE_SIZE];
        memcpy(type,(msg+4),TYPE_SIZE);
        if(memcmp(type,"Confirm2", TYPE_SIZE) == 0)
        {
            uint8_t *message = zrtp->conf2Ack->toBytes();
            uint16_t messageLength = zrtp->conf2Ack->getLength() * WORD_SIZE;
            zrtp->sendData(message,messageLength);
        }
    }
}

void StateEngine::handleWaitErrorAck()
{
    if(actualEvent->type == Timeout)
    {
        zrtp->sendData(sentMessage,sentMessageLength);
        if(!timerNext(&T2))
        {
            zrtp->cancelTimer();
        }
    }
    if(actualEvent->type == Message)
    {
        uint8_t *msg = actualEvent->message;
        uint8_t type[TYPE_SIZE];
        memcpy(type,(msg+4),TYPE_SIZE);
        if(memcmp(type,"ErrorACK", TYPE_SIZE) == 0)
        {
            zrtp->cancelTimer();
            actualState = Initial;
        }
    }
}
