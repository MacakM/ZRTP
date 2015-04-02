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

    std::cout << "Actual state: Initial" << std::endl;
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

bool StateEngine::timerStart(zrtpTimer *timer)
{
    timer->actualTime = timer->start;
    timer->resendCounter = 0;
    return zrtp->activateTimer(timer->start);
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
    assert(event);
    if(event->type == End)
    {
        std::cout << "Actual state: Initial" << std::endl;
        actualState = Initial;
    }

    if(event->messageLength > 0)
    {
        if(memcmp(event->message + 4,"Error   ",TYPE_SIZE) == 0)
        {
            zrtp->error = new PacketError();
            zrtp->error->parse(event->message, &errorCode);

            //check whether the packet length fits declared length
            if(zrtp->error->getLength() * WORD_SIZE != event->messageLength)
            {
                sendError(MalformedPacket);
                return;
            }
            PacketErrorAck *packet = new PacketErrorAck();
            uint8_t *message = packet->toBytes();
            uint16_t messageLength = packet->getLength() * WORD_SIZE;
            zrtp->sendData(message,messageLength);
            std::cout << "Actual state: Initial" << std::endl;
            actualState = Initial;
            delete(packet);
        }
    }

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
    std::cout << "Actual state: SentHello" << std::endl;
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

    else if(actualEvent->type == Message)
    {
        uint8_t *msg = actualEvent->message;
        uint8_t type[TYPE_SIZE];
        memcpy(type,(msg+4),TYPE_SIZE);

        if(memcmp(type,"Hello   ", TYPE_SIZE) == 0)
        {
            zrtp->peerHello = new PacketHello();
            if(!zrtp->peerHello->parse(msg, &errorCode))
            {
                sendError(errorCode);
                return;
            }

            //check whether the packet length fits declared length
            if(zrtp->peerHello->getLength() * WORD_SIZE != actualEvent->messageLength)
            {
                sendError(MalformedPacket);
                return;
            }
            //equal ZID check
            if(memcmp(zrtp->peerHello->getZid(),zrtp->hello->getZid(),ZID_SIZE) == 0)
            {
                sendError(EqualZid);
                return;
            }
            memcpy(zrtp->peerH3,zrtp->peerHello->getH3(),HASHIMAGE_SIZE);

            if(zrtp->compareVersions())
            {
                //versions are the same
                PacketHelloAck *packet = new PacketHelloAck();
                uint8_t *message = packet->toBytes();
                uint16_t messageLength = packet->getLength() * WORD_SIZE;
                zrtp->sendData(message,messageLength);
                delete(packet);
                if(!zrtp->chooseAlgorithm())
                {
                    sendError(KeyExchangeNotSupported);
                    return;
                }

                //generate private and public key
                zrtp->diffieHellman();

                assert(zrtp->publicKey);
                assert(zrtp->privateKey);
                assert(zrtp->p);

                std::cout << "Actual state: SentHelloAck" << std::endl;
                actualState = SentHelloAck;
                return;
            }

            if(zrtp->userInfo->versions.size() == 0)
            {
                sendError(UnsupportedVersion);
                return;
            }
            //different versions
            uint8_t *message = zrtp->hello->toBytes();
            uint16_t messageLength = zrtp->hello->getLength() * WORD_SIZE;
            zrtp->sendData(message,messageLength);

            sentMessage = message;
            sentMessageLength = messageLength;
            timerStart(&T1);
        }

        else if(memcmp(type,"HelloACK", TYPE_SIZE) == 0)
        {
            PacketHelloAck *packet = new PacketHelloAck();
            if(!packet->parse(msg, &errorCode))
            {
                sendError(errorCode);
                return;
            }

            //check whether the packet length fits declared length
            if(packet->getLength() * WORD_SIZE != actualEvent->messageLength)
            {
                sendError(MalformedPacket);
                return;
            }

            zrtp->cancelTimer();
            delete(packet);

            std::cout << "Actual state: ReceivedHelloAck" << std::endl;
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

    else if(actualEvent->type == Message)
    {
        uint8_t *msg = actualEvent->message;
        uint8_t type[TYPE_SIZE];
        memcpy(type,(msg+4),TYPE_SIZE);

        if(memcmp(type,"Hello   ", TYPE_SIZE) == 0)
        {
            //have to prevent parsing older Hello with greater version
            if(!zrtp->compareVersions())
            {
                if(!zrtp->peerHello->parse(msg, &errorCode))
                {
                    sendError(errorCode);
                    return;
                }
                //check whether the packet length fits declared length
                if(zrtp->peerHello->getLength() * WORD_SIZE != actualEvent->messageLength)
                {
                    sendError(MalformedPacket);
                    return;
                }
            }
            //have to check if new Hello has same version
            if(zrtp->compareVersions())
            {
                //versions are the same
                PacketHelloAck *packet = new PacketHelloAck();
                uint8_t *message = packet->toBytes();
                uint16_t messageLength = packet->getLength() * WORD_SIZE;
                zrtp->sendData(message,messageLength);
                delete(packet);
                return;
            }

            //different versions
            uint8_t *message = zrtp->hello->toBytes();
            uint16_t messageLength = zrtp->hello->getLength() * WORD_SIZE;
            zrtp->sendData(message,messageLength);

            sentMessage = message;
            sentMessageLength = messageLength;
            timerStart(&T1);
        }

        else if(memcmp(type,"HelloACK", TYPE_SIZE) == 0)
        {
            PacketHelloAck *packet = new PacketHelloAck();
            if(!packet->parse(msg, &errorCode))
            {
                sendError(errorCode);
                return;
            }

            //check whether the packet length fits declared length
            if(packet->getLength() * WORD_SIZE != actualEvent->messageLength)
            {
                sendError(MalformedPacket);
                return;
            }
            zrtp->cancelTimer();
            delete(packet);

            if(zrtp->myRole == Responder)
            {
                std::cout << "Actual state: WaitCommit" << std::endl;
                actualState = WaitCommit;
            }
            else    //Initiator
            {               
                zrtp->createCommitPacket();
                uint8_t *message = zrtp->commit->toBytes();
                uint16_t messageLength = zrtp->commit->getLength() * WORD_SIZE;
                zrtp->sendData(message,messageLength);

                sentMessage = message;
                sentMessageLength = messageLength;
                timerStart(&T2);
                std::cout << "Actual state: SentCommit" << std::endl;
                actualState = SentCommit;
            }
        }

        else if(memcmp(type,"Commit  ", TYPE_SIZE) == 0)
        {

            if(zrtp->myRole == Responder)
            {
                zrtp->commit = new PacketCommit();
                if(!zrtp->commit->parse(msg, &errorCode))
                {
                    sendError(errorCode);
                    return;
                }

                //check whether the packet length fits declared length
                if(zrtp->commit->getLength() * WORD_SIZE != actualEvent->messageLength)
                {
                    sendError(MalformedPacket);
                    return;
                }
                //check hash image h3
                memcpy(zrtp->peerH2,zrtp->commit->getH2(),HASHIMAGE_SIZE);
                if(!zrtp->isCorrectHashImage(zrtp->peerH3,zrtp->peerH2))
                {
                    sendError(CriticalSoftwareError);
                    return;
                }
                //check Hello MAC
                uint8_t *mac = zrtp->generateMac(zrtp->peerHello,false);
                if(memcmp(mac,zrtp->peerHello->getMac(),MAC_SIZE) != 0)
                {
                    delete(mac);
                    sendError(CriticalSoftwareError);
                    return;
                }
                delete(mac);
                //check whether commit contains supported algorithms
                if(!zrtp->checkCompatibility(&errorCode))
                {
                    sendError(errorCode);
                    return;
                }

                zrtp->createDHPart1Packet();

                uint8_t *message = zrtp->dhPart1->toBytes();
                uint16_t messageLength = zrtp->dhPart1->getLength() * WORD_SIZE;
                zrtp->sendData(message,messageLength);
                std::cout << "Actual state: WaitDHPart2" << std::endl;
                //after 10 seconds invoke protocol timeout error
                zrtp->activateTimer(PROTOCOL_TIMEOUT);
                actualState = WaitDHPart2;
                return;
            }
            //Commit contention phase
            zrtp->createCommitPacket();

            PacketCommit *peerCommit = new PacketCommit();
            if(!peerCommit->parse(msg, &errorCode))
            {
                sendError(errorCode);
                return;
            }

            //check whether the packet length fits declared length
            if(peerCommit->getLength() * WORD_SIZE != actualEvent->messageLength)
            {
                sendError(MalformedPacket);
                return;
            }
            //check hash image h3
            memcpy(zrtp->peerH2,peerCommit->getH2(),HASHIMAGE_SIZE);
            if(!zrtp->isCorrectHashImage(zrtp->peerH3,zrtp->peerH2))
            {
                sendError(CriticalSoftwareError);
                return;
            }
            //check Hello MAC
            uint8_t *mac = zrtp->generateMac(zrtp->peerHello,false);
            if(memcmp(mac,zrtp->peerHello->getMac(),MAC_SIZE) != 0)
            {
                delete(mac);
                sendError(CriticalSoftwareError);
                return;
            }
            delete(mac);

            //discard commit with lower hvi
            if(zrtp->commit->hasGreaterHvi(peerCommit))
            {
                //Initiator
                delete(peerCommit);
                uint8_t *message = zrtp->commit->toBytes();
                uint16_t messageLength = zrtp->commit->getLength() * WORD_SIZE;
                zrtp->sendData(message,messageLength);

                sentMessage = message;
                sentMessageLength = messageLength;
                timerStart(&T2);
                std::cout << "Actual state: SentCommit" << std::endl;
                actualState = SentCommit;
                return;
            }
            //Responder
            delete(zrtp->commit);
            zrtp->commit = peerCommit;
            zrtp->myRole = Responder;
            //delete created DHPart2 and create new DHPart1
            delete(zrtp->dhPart2);

            if(!zrtp->checkCompatibility(&errorCode))
            {
                sendError(errorCode);
                return;
            }

            zrtp->createDHPart1Packet();

            uint8_t *message = zrtp->dhPart1->toBytes();
            uint16_t messageLength = zrtp->dhPart1->getLength() * WORD_SIZE;
            zrtp->sendData(message,messageLength);
            std::cout << "Actual state: WaitDHPart2" << std::endl;
            //after 10 seconds invoke protocol timeout error
            zrtp->activateTimer(PROTOCOL_TIMEOUT);
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
            if(!zrtp->peerHello->parse(msg, &errorCode))
            {
                sendError(errorCode);
                return;
            }

            //check whether the packet length fits declared length
            if(zrtp->peerHello->getLength() * WORD_SIZE != actualEvent->messageLength)
            {
                sendError(MalformedPacket);
                return;
            }
            //equal ZID check
            if(memcmp(zrtp->peerHello->getZid(),zrtp->hello->getZid(),ZID_SIZE) == 0)
            {
                sendError(EqualZid);
                return;
            }
            memcpy(zrtp->peerH3,zrtp->peerHello->getH3(),HASHIMAGE_SIZE);

            if(!zrtp->compareVersions())
            {
                //different versions
                uint8_t *message = zrtp->hello->toBytes();
                uint16_t messageLength = zrtp->hello->getLength() * WORD_SIZE;
                zrtp->sendData(message,messageLength);

                sentMessage = message;
                sentMessageLength = messageLength;
                timerStart(&T1);
                return;
            }

            if(!zrtp->chooseAlgorithm())
            {
                sendError(KeyExchangeNotSupported);
                return;
            }

            //generate private and public key
            zrtp->diffieHellman();

            assert(zrtp->publicKey);
            assert(zrtp->privateKey);
            assert(zrtp->p);


            if(zrtp->myRole == Responder)
            {
                PacketHelloAck *packet = new PacketHelloAck();
                uint8_t *message = packet->toBytes();
                uint16_t messageLength = packet->getLength() * WORD_SIZE;
                zrtp->sendData(message,messageLength);
                delete(packet);

                std::cout << "Actual state: WaitCommit" << std::endl;
                actualState = WaitCommit;
            }
            else    //Initiator
            {
                //can send commit instead of helloACK
                zrtp->createCommitPacket();
                uint8_t *message = zrtp->commit->toBytes();
                uint16_t messageLength = zrtp->commit->getLength() * WORD_SIZE;
                zrtp->sendData(message,messageLength);

                sentMessage = message;
                sentMessageLength = messageLength;
                timerStart(&T2);
                std::cout << "Actual state: SentCommit" << std::endl;
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

    else if(actualEvent->type == Message)
    {
        uint8_t *msg = actualEvent->message;
        uint8_t type[TYPE_SIZE];
        memcpy(type,(msg+4),TYPE_SIZE);

        if(memcmp(type,"DHPart1 ", TYPE_SIZE) == 0)
        {
            zrtp->dhPart1 = new PacketDHPart();
            if(!zrtp->dhPart1->parse(msg, &errorCode))
            {
                sendError(errorCode);
                return;
            }

            //check whether the packet length fits declared length
            if(zrtp->dhPart1->getLength() * WORD_SIZE != actualEvent->messageLength)
            {
                sendError(MalformedPacket);
                return;
            }
            //check whether the public value is valid
            if(!zrtp->isValidPeerPv())
            {
                sendError(BadDhPublicValue);
                return;
            }
            //calculate hash image h2 and check hash image h3 and h2
            memcpy(zrtp->peerH1,zrtp->dhPart1->getH1(),HASHIMAGE_SIZE);
            zrtp->calculatePeerH2(zrtp->peerH1);
            if(!zrtp->isCorrectHashImage(zrtp->peerH3,zrtp->peerH2))
            {
                sendError(CriticalSoftwareError);
                return;
            }
            if(!zrtp->isCorrectHashImage(zrtp->peerH2,zrtp->peerH1))
            {
                sendError(CriticalSoftwareError);
                return;
            }
            //check Hello MAC
            uint8_t *mac = zrtp->generateMac(zrtp->peerHello,false);
            if(memcmp(mac,zrtp->peerHello->getMac(),MAC_SIZE) != 0)
            {
                delete(mac);
                sendError(CriticalSoftwareError);
                return;
            }
            delete(mac);

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
            std::cout << "Actual state: WaitConfirm1" << std::endl;
            actualState = WaitConfirm1;
        }

        //commit contention
        else if(memcmp(type,"Commit  ",TYPE_SIZE) == 0)
        {
            PacketCommit *peerCommit = new PacketCommit();
            if(!peerCommit->parse(msg, &errorCode))
            {
                sendError(errorCode);
                return;
            }

            //check whether the packet length fits declared length
            if(peerCommit->getLength() * WORD_SIZE != actualEvent->messageLength)
            {
                sendError(MalformedPacket);
                return;
            }
            //check hash image h3
            memcpy(zrtp->peerH2,peerCommit->getH2(),HASHIMAGE_SIZE);
            if(!zrtp->isCorrectHashImage(zrtp->peerH3,zrtp->peerH2))
            {
                sendError(CriticalSoftwareError);
                return;
            }
            //check Hello MAC
            uint8_t *mac = zrtp->generateMac(zrtp->peerHello,false);
            if(memcmp(mac,zrtp->peerHello->getMac(),MAC_SIZE) != 0)
            {
                delete(mac);
                sendError(CriticalSoftwareError);
                return;
            }
            delete(mac);

            //discard commit with lower hvi
            if(zrtp->commit->hasGreaterHvi(peerCommit))
            {
                //Initiator
                delete(peerCommit);
                return;
            }
            //Responder
            delete(zrtp->commit);
            zrtp->commit = peerCommit;
            zrtp->myRole = Responder;
            zrtp->cancelTimer();
            //delete created DHPart2 and create new DHPart1
            delete(zrtp->dhPart2);

            if(!zrtp->checkCompatibility(&errorCode))
            {
                sendError(errorCode);
                return;
            }

            zrtp->createDHPart1Packet();
            uint8_t *message = zrtp->dhPart1->toBytes();
            uint16_t messageLength = zrtp->dhPart1->getLength() * WORD_SIZE;
            zrtp->sendData(message,messageLength);
            std::cout << "Actual state: WaitDHPart2" << std::endl;
            //after 10 seconds invoke protocol timeout error
            zrtp->activateTimer(PROTOCOL_TIMEOUT);
            actualState = WaitDHPart2;
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
            //have to prevent parsing older Hello with greater version
            if(!zrtp->compareVersions())
            {
                if(!zrtp->peerHello->parse(msg, &errorCode))
                {
                    sendError(errorCode);
                    return;
                }

                //check whether the packet length fits declared length
                if(zrtp->peerHello->getLength() * WORD_SIZE != actualEvent->messageLength)
                {
                    sendError(MalformedPacket);
                    return;
                }
            }
            //have to check if new Hello has same version
            if(zrtp->compareVersions())
            {
                //versions are the same
                PacketHelloAck *packet = new PacketHelloAck();
                uint8_t *message = packet->toBytes();
                uint16_t messageLength = packet->getLength() * WORD_SIZE;
                zrtp->sendData(message,messageLength);
                delete(packet);
                return;
            }

            //different versions
            uint8_t *message = zrtp->hello->toBytes();
            uint16_t messageLength = zrtp->hello->getLength() * WORD_SIZE;
            zrtp->sendData(message,messageLength);

            sentMessage = message;
            sentMessageLength = messageLength;
            timerStart(&T1);
        }

        else if(memcmp(type,"Commit  ", TYPE_SIZE) == 0)
        {
            zrtp->commit = new PacketCommit();
            if(!zrtp->commit->parse(msg, &errorCode))
            {
                sendError(errorCode);
                return;
            }

            //check whether the packet length fits declared length
            if(zrtp->commit->getLength() * WORD_SIZE != actualEvent->messageLength)
            {
                sendError(MalformedPacket);
                return;
            }
            //check hash image h3
            memcpy(zrtp->peerH2,zrtp->commit->getH2(),HASHIMAGE_SIZE);
            if(!zrtp->isCorrectHashImage(zrtp->peerH3,zrtp->peerH2))
            {
                sendError(CriticalSoftwareError);
                return;
            }
            //check Hello MAC
            uint8_t *mac = zrtp->generateMac(zrtp->peerHello,false);
            if(memcmp(mac,zrtp->peerHello->getMac(),MAC_SIZE) != 0)
            {
                delete(mac);
                sendError(CriticalSoftwareError);
                return;
            }
            delete(mac);

            if(!zrtp->checkCompatibility(&errorCode))
            {
                sendError(errorCode);
                return;
            }

            zrtp->createDHPart1Packet();

            uint8_t *message = zrtp->dhPart1->toBytes();
            uint16_t messageLength = zrtp->dhPart1->getLength() * WORD_SIZE;
            zrtp->sendData(message,messageLength);
            std::cout << "Actual state: WaitDHPart2" << std::endl;
            //after 10 seconds invoke protocol timeout error
            zrtp->activateTimer(PROTOCOL_TIMEOUT);
            actualState = WaitDHPart2;
        }
    }
}

void StateEngine::handleWaitDHPart2()
{
    if(actualEvent->type == Timeout)
    {
        sendError(ProtocolTimeout);
        return;
    }
    if(actualEvent->type == Message)
    {
        zrtp->cancelTimer();
        uint8_t *msg = actualEvent->message;
        uint8_t type[TYPE_SIZE];
        memcpy(type,(msg+4),TYPE_SIZE);

        if(memcmp(type,"Commit  ", TYPE_SIZE) == 0)
        {
            uint8_t *message = zrtp->dhPart1->toBytes();
            uint16_t messageLength = zrtp->dhPart1->getLength() * WORD_SIZE;
            zrtp->sendData(message,messageLength);
        }

        else if(memcmp(type,"DHPart2 ", TYPE_SIZE) == 0)
        {
            zrtp->dhPart2 = new PacketDHPart();
            if(!zrtp->dhPart2->parse(msg, &errorCode))
            {
                sendError(errorCode);
                return;
            }

            //check whether the packet length fits declared length
            if(zrtp->dhPart2->getLength() * WORD_SIZE != actualEvent->messageLength)
            {
                sendError(MalformedPacket);
                return;
            }
            //check whether the calculated hvi is equaled received hvi
            uint8_t *calculatedHvi = zrtp->generateHvi();
            if(memcmp(zrtp->commit->getHvi(),calculatedHvi,HVI_SIZE) != 0)
            {
                delete(calculatedHvi);
                sendError(DhHviError);
                return;
            }
            delete(calculatedHvi);
            //check whether the public value is valid
            if(!zrtp->isValidPeerPv())
            {
                sendError(BadDhPublicValue);
                return;
            }
            //check hash image h2
            memcpy(zrtp->peerH1,zrtp->dhPart2->getH1(),HASHIMAGE_SIZE);
            if(!zrtp->isCorrectHashImage(zrtp->peerH2,zrtp->peerH1))
            {
                sendError(CriticalSoftwareError);
                return;
            }
            //check Commit MAC
            uint8_t *mac = zrtp->generateMac(zrtp->commit,false);
            if(memcmp(mac,zrtp->commit->getMac(),MAC_SIZE) != 0)
            {
                delete(mac);
                sendError(CriticalSoftwareError);
                return;
            }
            delete(mac);

            zrtp->createDHResult();
            zrtp->sharedSecretCalculation();
            zrtp->keyDerivation();

            zrtp->createConfirm1Packet();
            uint8_t *message = zrtp->confirm1->toBytes();
            uint16_t messageLength = zrtp->confirm1->getLength() * WORD_SIZE;
            zrtp->sendData(message,messageLength);
            std::cout << "Actual state: WaitConfirm2" << std::endl;
            actualState = WaitConfirm2;
        }
        zrtp->activateTimer(PROTOCOL_TIMEOUT);
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

    else if(actualEvent->type == Message)
    {
        uint8_t *msg = actualEvent->message;
        uint8_t type[TYPE_SIZE];
        memcpy(type,(msg+4),TYPE_SIZE);

        if(memcmp(type,"Confirm1", TYPE_SIZE) == 0)
        {
            zrtp->cancelTimer();
            zrtp->confirm1 = new PacketConfirm();
            if(!zrtp->confirm1->parse(msg, &errorCode))
            {
                sendError(errorCode);
                return;
            }

            //check whether the packet length fits declared length
            if(zrtp->confirm1->getLength() * WORD_SIZE != actualEvent->messageLength)
            {
                sendError(MalformedPacket);
                return;
            }
            //check confirm mac
            uint8_t *confMac = zrtp->generateConfirmMac(zrtp->confirm1,false);
            if(memcmp(confMac,zrtp->confirm1->getConfirmMac(),MAC_SIZE) != 0)
            {
                delete(confMac);
                sendError(BadConfirm);
                return;
            }
            delete(confMac);
            zrtp->decryptConfirmData(msg);
            //check hash image h1
            memcpy(zrtp->peerH0,zrtp->confirm1->getH0(),HASHIMAGE_SIZE);
            if(!zrtp->isCorrectHashImage(zrtp->peerH1,zrtp->peerH0))
            {
                sendError(CriticalSoftwareError);
                return;
            }
            //check DHPart1 MAC
            uint8_t *mac = zrtp->generateMac(zrtp->dhPart1,false);
            if(memcmp(mac,zrtp->dhPart1->getMac(),MAC_SIZE) != 0)
            {
                delete(mac);
                sendError(CriticalSoftwareError);
                return;
            }
            delete(mac);

            zrtp->createConfirm2Packet();
            uint8_t *message = zrtp->confirm2->toBytes();
            uint16_t messageLength = zrtp->confirm2->getLength() * WORD_SIZE;
            zrtp->sendData(message,messageLength);

            sentMessage = message;
            sentMessageLength = messageLength;
            timerStart(&T2);
            std::cout << "Actual state: WaitConf2Ack" << std::endl;
            actualState = WaitConf2Ack;
        }
    }
}

void StateEngine::handleWaitConfirm2()
{
    if(actualEvent->type == Timeout)
    {
        sendError(ProtocolTimeout);
        return;
    }
    if(actualEvent->type == Message)
    {
        zrtp->cancelTimer();
        uint8_t *msg = actualEvent->message;
        uint8_t type[TYPE_SIZE];
        memcpy(type,(msg+4),TYPE_SIZE);

        if(memcmp(type,"DHPart2 ", TYPE_SIZE) == 0)
        {
            if(!zrtp->dhPart2->parse(msg, &errorCode))
            {
                sendError(errorCode);
                return;
            }

            //check whether the packet length fits declared length
            if(zrtp->dhPart2->getLength() * WORD_SIZE != actualEvent->messageLength)
            {
                sendError(MalformedPacket);
                return;
            }
            //check whether the calculated hvi is equaled received hvi
            uint8_t *calculatedHvi = zrtp->generateHvi();
            if(memcmp(zrtp->commit->getHvi(),calculatedHvi,HVI_SIZE) != 0)
            {
                delete(calculatedHvi);
                sendError(DhHviError);
                return;
            }
            delete(calculatedHvi);
            //check whether the public value is valid
            if(!zrtp->isValidPeerPv())
            {
                sendError(BadDhPublicValue);
                return;
            }
            //check hash image h2
            memcpy(zrtp->peerH1,zrtp->dhPart2->getH1(),HASHIMAGE_SIZE);
            if(!zrtp->isCorrectHashImage(zrtp->peerH2,zrtp->peerH1))
            {
                sendError(CriticalSoftwareError);
                return;
            }
            //check Commit MAC
            uint8_t *mac = zrtp->generateMac(zrtp->commit,false);
            if(memcmp(mac,zrtp->commit->getMac(),MAC_SIZE) != 0)
            {
                delete(mac);
                sendError(CriticalSoftwareError);
                return;
            }
            delete(mac);

            uint8_t *message = zrtp->confirm1->toBytes();
            uint16_t messageLength = zrtp->confirm1->getLength() * WORD_SIZE;
            zrtp->sendData(message,messageLength);
            zrtp->activateTimer(PROTOCOL_TIMEOUT);
        }

        else if(memcmp(type,"Confirm2", TYPE_SIZE) == 0)
        {
            zrtp->confirm2 = new PacketConfirm();
            if(!zrtp->confirm2->parse(msg, &errorCode))
            {
                sendError(errorCode);
                return;
            }

            //check whether the packet length fits declared length
            if(zrtp->confirm2->getLength() * WORD_SIZE != actualEvent->messageLength)
            {
                sendError(MalformedPacket);
                return;
            }
            //check confirm mac
            uint8_t *confMac = zrtp->generateConfirmMac(zrtp->confirm2,false);
            if(memcmp(confMac,zrtp->confirm2->getConfirmMac(),MAC_SIZE) != 0)
            {
                delete(confMac);
                sendError(BadConfirm);
                return;
            }
            delete(confMac);
            zrtp->decryptConfirmData(msg);
            //check hash image h1
            memcpy(zrtp->peerH0,zrtp->confirm2->getH0(),HASHIMAGE_SIZE);
            if(!zrtp->isCorrectHashImage(zrtp->peerH1,zrtp->peerH0))
            {
                sendError(CriticalSoftwareError);
                return;
            }
            //check DHPart2 MAC
            uint8_t *mac = zrtp->generateMac(zrtp->dhPart2,false);
            if(memcmp(mac,zrtp->dhPart2->getMac(),MAC_SIZE) != 0)
            {
                delete(mac);
                sendError(CriticalSoftwareError);
                return;
            }
            delete(mac);

            PacketConf2Ack *packet = new PacketConf2Ack();
            uint8_t *message = packet->toBytes();
            uint16_t messageLength = packet->getLength() * WORD_SIZE;
            zrtp->sendData(message,messageLength);
            delete(packet);
            std::cout << "Actual state: Secured" << std::endl;
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

    else if(actualEvent->type == Message)
    {
        uint8_t *msg = actualEvent->message;
        uint8_t type[TYPE_SIZE];
        memcpy(type,(msg+4),TYPE_SIZE);

        if(memcmp(type,"Conf2ACK", TYPE_SIZE) == 0)
        {
            zrtp->cancelTimer();
            std::cout << "Actual state: Secured" << std::endl;
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
            PacketConf2Ack *packet = new PacketConf2Ack();
            uint8_t *message = packet->toBytes();
            uint16_t messageLength = packet->getLength() * WORD_SIZE;
            zrtp->sendData(message,messageLength);
            delete(packet);
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

    else if(actualEvent->type == Message)
    {
        uint8_t *msg = actualEvent->message;
        uint8_t type[TYPE_SIZE];
        memcpy(type,(msg+4),TYPE_SIZE);

        if(memcmp(type,"ErrorACK", TYPE_SIZE) == 0)
        {
            zrtp->cancelTimer();
            std::cout << "Actual state: Initial" << std::endl;
            actualState = Initial;
        }
    }
}

void StateEngine::sendError(uint32_t code)
{
    zrtp->error = new PacketError(code);
    uint8_t *message = zrtp->error->toBytes();
    uint16_t messageLength = zrtp->error->getLength() * WORD_SIZE;
    zrtp->sendData(message,messageLength);

    sentMessage = message;
    sentMessageLength = messageLength;
    timerStart(&T2);
    std::cout << "Actual state: WaitErrorAck" << std::endl;
    actualState = WaitErrorAck;
    return;
}
