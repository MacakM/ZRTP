#ifndef ZRTPMESSAGE_H
#define ZRTPMESSAGE_H

#include <QDebug>
#include <QThread>
#include <QMutex>
#include "networkmanager.h"

class NetworkManager;

/**
 * Class that runs processZrtpMessage in another thread.
 */
class ZrtpMessage : public QThread
{
    Q_OBJECT
public:
    /**
     * Constructor of ZrtpMessage
     *
     * @param manager   NetworkManager that created this thread
     * @param msg       message that is going to be sent
     * @param length    length of the message
     * @param delay     maximum simulated delay in milliseconds
     */
    explicit ZrtpMessage(NetworkManager *manager, uint8_t *msg, int32_t length, int32_t delay);

private:
    NetworkManager *manager;
    uint8_t *msg;
    int32_t length;
    int32_t delay;

    void run();
};

#endif // ZRTPMESSAGE_H
