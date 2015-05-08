#ifndef RESTARTER_H
#define RESTARTER_H

#include <QThread>
#include <windows.h>
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
    explicit Restarter(NetworkManager *manager, HANDLE semaphore);

private:
    NetworkManager *manager;
    HANDLE semaphore;

    void run();
};

#endif // RESTARTER_H
