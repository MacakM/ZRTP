#include "mycallbacks.h"

MyCallbacks::MyCallbacks(NetworkManager *manager)
{
    this->manager = manager;
}

bool MyCallbacks::sendData (const uint8_t* data, int32_t length)
{
    QByteArray datagram((char*)data, length);

    if (manager->sendSocket->writeDatagram(datagram, manager->sendIp, manager->sendPort) == -1)
    {
        return false;
    }
    return true;
}

bool MyCallbacks::activateTimer(int32_t time)
{
    manager->setActualSignal(1, time);
    return true;
}

bool MyCallbacks::cancelTimer()
{
    manager->setActualSignal(2);
    return true;
}

void MyCallbacks::enterCriticalSection()
{
    manager->mutex->lock();
}

void MyCallbacks::leaveCriticalSection()
{
    manager->mutex->unlock();
}
