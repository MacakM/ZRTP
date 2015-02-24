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
