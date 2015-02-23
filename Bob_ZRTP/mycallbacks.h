#ifndef MYCALLBACKS_H
#define MYCALLBACKS_H

#include <QObject>
#include <QUdpSocket>
#include "networkmanager.h"

class NetworkManager;

class MyCallbacks : public ZrtpCallback
{
public:
    MyCallbacks(NetworkManager *manager);

    virtual bool sendData (const uint8_t* data, int32_t length);

private:
    NetworkManager *manager;
};

#endif // MYCALLBACKS_H
