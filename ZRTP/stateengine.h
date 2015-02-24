#ifndef STATEENGINE_H
#define STATEENGINE_H

#include "Integers.h"
#include "States.h"
#include <map>

class StateEngine;

typedef enum
{
    Init            = 0x0000,
    WaitHelloACK    = 0x0001
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
};

#endif // STATEENGINE_H
