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
    if(actualEvent->type == Start)
    {
        uint8_t* msg = (uint8_t*)"Hello";
        zrtp->sendData(msg,5);
        timerStart(&T1);
        actualState = SentHello;
    }
}

void StateEngine::handleSentHello()
{
    if(actualEvent->type == Timeout)
    {
        uint8_t* msg = (uint8_t*)"HelloTimer";
        zrtp->sendData(msg,10);
        if(!timerNext(&T1))
        {
            zrtp->cancelTimer();
        }
    }
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