#ifndef STATEENGINE_H
#define STATEENGINE_H

#include "Integers.h"
#include <map>

class StateEngine;

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
    Secured             = 0x0010,
    WaitErrorAck        = 0x0011
}States;

typedef enum
{
    Initialize,
    End,
    Packet,
    Timeout
}EventType;

typedef struct {
    EventType type;
    int32_t length;
    uint8_t *data;
} Event;

typedef void (StateEngine::*Handler)(void);
typedef std::map<States, Handler> handlerMap;

class Zrtp;

class StateEngine
{
public:
    StateEngine();

private:
    handlerMap handlers;

    void initHandlers();
    void processEvent(Event *event);

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
