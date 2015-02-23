#include "networkmanager.h"

NetworkManager::NetworkManager(QObject *parent) :
    QObject(parent)
{
    socket = new QUdpSocket();
    myZid = 8;

    callbacks = new MyCallbacks(this);

    zrtp = new Zrtp(&myZid, callbacks);
}

uint8_t NetworkManager::getMyZid()
{
     return myZid;
}
