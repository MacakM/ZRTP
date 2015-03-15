#ifndef STATEENGINE_H
#define STATEENGINE_H

#include "zrtp.h"
#include "Integers.h"
#include <map>
#include "packet.h"

#include <iostream>

class StateEngine;
class Zrtp;

typedef enum
{
    Initial,
    SentHello,
    SentHelloAck,
    ReceivedHelloAck,
    SentCommit,
    WaitCommit,
    WaitDHPart2,
    WaitConfirm1,
    WaitConfirm2,
    WaitConf2Ack,
    Secured,
    WaitErrorAck
}States;

typedef enum
{
    Start,
    End,
    Message,
    Timeout
}EventType;

typedef struct {
    EventType type;
    uint8_t *message;
    int32_t messageLength;
} Event;

typedef struct {
    int32_t start;
    int32_t cap;
    int32_t actualTime;
    int32_t resendCounter;
    int32_t maxResend;
} zrtpTimer;

typedef void (StateEngine::*Handler)(void);
typedef std::map<States, Handler> handlerMap;

class StateEngine
{
public:
    StateEngine(Zrtp *zrtp);

    void processEvent(Event *event);

private:
    Zrtp *zrtp;
    handlerMap handlers;
    States actualState;
    Event *actualEvent;

    uint8_t *sentMessage;
    uint8_t sentMessageLength;

    zrtpTimer T1;
    zrtpTimer T2;

    void initHandlers();
    void timerStart(zrtpTimer *t);
    bool timerNext(zrtpTimer *t);

    void handleInitial();
    void handleSentHello();
    void handleSentHelloAck();
    void handleReceivedHelloAck();
    void handleSentCommit();
    void handleWaitCommit();
    void handleWaitDHPart2();
    void handleWaitConfirm1();
    void handleWaitConfirm2();
    void handleWaitConf2Ack();
    void handleSecured();
    void handleWaitErrorAck();
};

#endif // STATEENGINE_H
