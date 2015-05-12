#ifndef RESTARTER_H
#define RESTARTER_H

#include <QThread>
#include <QSystemSemaphore>
#include "networkmanager.h"

class NetworkManager;

class Restarter : public QThread
{
    Q_OBJECT
public:
    /**
    * Constructor of Restarter.
    *
    * @param manager   NetworkManager that created this thread
    */
    explicit Restarter(NetworkManager *manager, QSystemSemaphore *semaphore);

private:
    NetworkManager *manager;
    QSystemSemaphore *semaphore;

    void run();
};

#endif // RESTARTER_H
