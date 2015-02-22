#include "networkmanager.h"



NetworkManager::NetworkManager(QObject *parent) :
    QObject(parent)
{
    callbacks = new ZrtpCallback(&NetworkManager::sendData);

    //zrtp = new Zrtp((uint8_t*) 8, callbacks);
}

bool NetworkManager::sendData(const uint8_t *data, int32_t length)
{
    QByteArray datagram((char*)data, length);
    if (socket->writeDatagram(datagram, QHostAddress::LocalHost,1234) == -1)
    {
        return false;
    }
    return true;
}
