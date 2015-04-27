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
    Message,
    Timeout
}EventType;

typedef struct {
    EventType type;
    uint8_t *message;
    uint16_t messageLength;
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
    /**
     * StateEngine constructor.
     *
     * Zrtp class that cooperates with StateEngine
     */
    StateEngine(Zrtp *zrtp);

    /**
      * StateEngine destructor.
      */
    ~StateEngine();

    /**
     * Process given event from Zrtp.
     * Then calls state handler according to actual state.
     *
     * @param event
     */
    void processEvent(Event *event);

    States *getActualState();

private:
    Zrtp *zrtp;
    handlerMap handlers;
    States actualState;
    Event *actualEvent;

    uint8_t *sentMessage;
    uint32_t sentMessageLength;

    zrtpTimer T1;
    zrtpTimer T2;

    uint32_t errorCode;

    /**
     * Initializes all function pointers to map.
     */
    void initHandlers();

    /**
     * Activate chosen timer for the first time.
     *
     * @param timer     timer
     * @return          true = successful, false = otherwise
     */
    bool timerStart(zrtpTimer *timer);

    /**
     * Activate chosen timer again.
     *
     * @param timer     timer
     * @return          true = successful, false = otherwise
     */
    bool timerNext(zrtpTimer *timer);

    /**
     * Handles Initial state of created state machine.
     */
    void handleInitial();

    /**
     * Handles SentHello state of created state machine.
     */
    void handleSentHello();

    /**
     * Handles SentHelloAck state of created state machine.
     */
    void handleSentHelloAck();

    /**
     * Handles ReceivedHelloAck state of created state machine.
     */
    void handleReceivedHelloAck();

    /**
     * Handles SentCommit state of created state machine.
     */
    void handleSentCommit();

    /**
     * Handles WaitCommit state of created state machine.
     */
    void handleWaitCommit();

    /**
     * Handles WaitDHPart2 state of created state machine.
     */
    void handleWaitDHPart2();

    /**
     * Handles WaitConfirm1 state of created state machine.
     */
    void handleWaitConfirm1();

    /**
     * Handles WaitConfirm2 state of created state machine.
     */
    void handleWaitConfirm2();

    /**
     * Handles WaitConf2Ack state of created state machine.
     */
    void handleWaitConf2Ack();

    /**
     * Handles Secured state of created state machine.
     */
    void handleSecured();

    /**
     * Handles WaitErrorAck state of created state machine.
     */
    void handleWaitErrorAck();

    /**
     * When something's wrong, sends error with given error code.
     * @param code  error code
     */
    void sendError(uint32_t code);
};

#endif // STATEENGINE_H
