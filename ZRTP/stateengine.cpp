#include "stateengine.h"

StateEngine::StateEngine(Zrtp *zrtp)
{
    initHandlers();
    this->zrtp = zrtp;
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

void StateEngine::processEvent(Event *event)
{
    actualEvent = event;
    (this->*handlers[actualState])();
}

void StateEngine::handleInitial()
{
    if(actualEvent->type == Start)
    {
        uint8_t* msg = (uint8_t*)"Hello";
        zrtp->sendData(msg,5);
        zrtp->activateTimer(2000);
        //actualState = SentHello;
    }

    if(actualEvent->type == Timeout)
    {
        uint8_t* msg = (uint8_t*)"HelloTimer";
        zrtp->sendData(msg,10);
        zrtp->activateTimer(2000);
    }
}

void StateEngine::handleSentHello()
{

}

void StateEngine::handleSentHelloAck()
{

}

void StateEngine::handleReceivedHelloAck()
{

}

void StateEngine::handleSentCommit()
{

}

void StateEngine::handleWaitCommit()
{

}

void StateEngine::handleWaitDHPart2()
{

}

void StateEngine::handleWaitConfirm1()
{

}

void StateEngine::handleWaitConfirm2()
{

}

void StateEngine::handleWaitConf2Ack()
{

}

void StateEngine::handleSecured()
{

}

void StateEngine::handleWaitErrorAck()
{

}
