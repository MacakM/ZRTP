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

void StateEngine::timerStart(zrtpTimer *t)
{
    t->actualTime = t->start;
    t->resendCounter = 0;
    zrtp->activateTimer(t->start);
}

bool StateEngine::timerNext(zrtpTimer *t)
{
    t->actualTime *= 2;
    if(t->actualTime > t->cap)
    {
        t->actualTime = t->cap;
    }
    t->resendCounter++;
    if(t->resendCounter > t->maxResend)
    {
        return false;
    }
    return zrtp->activateTimer(t->actualTime);
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
        uint8_t first = *(msg + 4);
        uint8_t last = *(msg + 11);
        //Hello
        if(first == 'H' && last == ' ')
        {
            zrtp->peerHello = new PacketHello();
            zrtp->peerHello->parse(msg);
            zrtp->createHelloAckPacket();
            uint8_t *message = zrtp->helloAck->toBytes();
            uint16_t messageLength = zrtp->helloAck->getLength() * WORD_SIZE;
            zrtp->sendData(message,messageLength);
            actualState = SentHelloAck;
        }
        //HelloACK
        if(first == 'H' && last == 'K')
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
        uint8_t first = *(msg + 4);
        uint8_t last = *(msg + 11);
        //Hello
        if(first == 'H' && last == ' ')
        {
            zrtp->peerHello->parse(msg);
            uint8_t *message = zrtp->helloAck->toBytes();
            uint16_t messageLength = zrtp->helloAck->getLength() * WORD_SIZE;
            zrtp->sendData(message,messageLength);
        }
        //HelloACK
        if(first == 'H' && last == 'K')
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
        //Commit
        if(first == 'C' && last == ' ')
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
        uint8_t first = *(msg + 4);
        uint8_t last = *(msg + 11);
        //Hello
        if(first == 'H' && last == ' ')
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
        uint8_t first = *(msg + 4);
        uint8_t secondLast = *(msg + 10);
        //DHPart1
        if(first == 'D' && secondLast == '1')
        {
            zrtp->dhPart1 = new PacketDHPart();
            zrtp->dhPart1->parse(msg);
            zrtp->cancelTimer();
            uint8_t *message = zrtp->dhPart2->toBytes();
            uint16_t messageLength = zrtp->dhPart2->getLength() * WORD_SIZE;
            zrtp->sendData(message,messageLength);
            sentMessage = message;
            sentMessageLength = messageLength;
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
        uint8_t first = *(msg + 4);
        uint8_t last = *(msg + 11);
        //Hello
        if(first == 'H' && last == ' ')
        {
            zrtp->peerHello->parse(msg);
            uint8_t *message = zrtp->helloAck->toBytes();
            uint16_t messageLength = zrtp->helloAck->getLength() * WORD_SIZE;
            zrtp->sendData(message,messageLength);
        }
        //Commit
        if(first == 'C' && last == ' ')
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
        uint8_t first = *(msg + 4);
        uint8_t secondLast = *(msg + 10);
        //Commit
        if(first == 'C' && secondLast == ' ')
        {
            zrtp->commit->parse(msg);
            uint8_t *message = zrtp->dhPart1->toBytes();
            uint16_t messageLength = zrtp->dhPart1->getLength() * WORD_SIZE;
            zrtp->sendData(message,messageLength);
        }
        //DHPart2
        if(first == 'D' && secondLast == '2')
        {
            zrtp->dhPart2 = new PacketDHPart();
            zrtp->dhPart2->parse(msg);
            //create confirm1
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
        uint8_t first = *(msg + 4);
        uint8_t last = *(msg + 11);
        //Confirm1
        if(first == 'C' && last == '1')
        {
            zrtp->cancelTimer();
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
        uint8_t first = *(msg + 4);
        uint8_t last = *(msg + 11);
        //DHPart2
        if(first == 'D' && last == '2')
        {
            zrtp->dhPart2->parse(msg);
            uint8_t *message = zrtp->confirm1->toBytes();
            uint16_t messageLength = zrtp->confirm1->getLength() * WORD_SIZE;
            zrtp->sendData(message,messageLength);
        }
        //Confirm2
        if(first == 'C' && last == '2')
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
        uint8_t first = *(msg + 4);
        uint8_t last = *(msg + 11);
        //Conf2ACK
        if(first == 'C' && last == 'K')
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
        uint8_t first = *(msg + 4);
        uint8_t last = *(msg + 11);
        //Confirm2
        if(first == 'C' && last == '2')
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
        uint8_t first = *(msg + 4);
        uint8_t last = *(msg + 11);
        //ErrorACK
        if(first == 'E' && last == 'K')
        {
            zrtp->cancelTimer();
            actualState = Initial;
        }
    }
}
