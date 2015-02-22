#include "mycallbacks.h"

MyCallbacks::MyCallbacks()
{
}

bool MyCallbacks::sendData (const uint8_t* data, int32_t length)
{
    QByteArray datagram((char*)data, length);
    if (manager->socket->writeDatagram(datagram, QHostAddress::LocalHost,1234) == -1)
    {
        return false;
    }
    return true;
}
