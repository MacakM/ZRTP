#ifndef ZRTPTIMEOUT_H
#define ZRTPTIMEOUT_H

#include <QDebug>
#include <QThread>
#include <QMutex>
#include "networkmanager.h"

class NetworkManager;

/**
 * Class that runs processZrtpTimeout in another thread.
 */
class ZrtpTimeout : public QThread
{
    Q_OBJECT
public:
    /**
     * Constructor of ZrtpTimeout.
     * @param manager   NetworkManager that created this thread.
     * @param delay     maximum simulated delay in milliseconds
     */
    explicit ZrtpTimeout(NetworkManager *manager, int32_t delay);

private:
    NetworkManager *manager;

    int32_t delay;

    void run();
};

#endif // ZRTPTIMEOUT_H
