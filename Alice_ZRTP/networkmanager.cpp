#include "networkmanager.h"

NetworkManager::NetworkManager(QObject *parent) :
    QObject(parent)
{
    myZid = (uint8_t*) 8;
    callbacks = new MyCallbacks();

    zrtp = new Zrtp(myZid, callbacks);
}
