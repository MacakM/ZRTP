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
    Initial             = 0x0000,
    SentHello           = 0x0001,
    SentHelloAck        = 0x0002,
    ReceivedHelloAck    = 0x0003,
    SentCommit          = 0x0004,
    WaitCommit          = 0x0005,
    WaitDHPart2         = 0x0006,
    WaitConfirm1        = 0x0007,
    WaitConfirm2        = 0x0008,
    WaitConf2Ack        = 0x0009,
    Secured             = 0x000a,
    WaitErrorAck        = 0x000b
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
