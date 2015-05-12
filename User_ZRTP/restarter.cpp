#include "restarter.h"

Restarter::Restarter(NetworkManager *manager, QSystemSemaphore *semaphore)
{
    this->manager = manager;
    this->semaphore = semaphore;
}

void Restarter::run()
{
    semaphore->acquire();
    manager->setActualSignal(5);
}
