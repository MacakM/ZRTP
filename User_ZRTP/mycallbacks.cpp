#include "mycallbacks.h"

MyCallbacks::MyCallbacks(NetworkManager *manager)
{
    this->manager = manager;
}

bool MyCallbacks::sendData (const uint8_t* data, int32_t length)
{
    assert(data);
    QByteArray datagram((char*)data, length);

    std::cout << "Sending " << data[4] << data[5] << data[6] << data[7] << data[8] << data[9] << data[10] << data[11] << std::endl;

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

void MyCallbacks::keyAgreed()
{
    manager->setActualSignal(3);
}
