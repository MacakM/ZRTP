#include "restarter.h"

Restarter::Restarter(NetworkManager *manager, HANDLE semaphore)
{
    this->manager = manager;
    this->semaphore = semaphore;
}

void Restarter::run()
{
    WaitForSingleObject(semaphore,INFINITE);
    manager->setActualSignal(5);
}
