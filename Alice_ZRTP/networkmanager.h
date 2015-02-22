#ifndef NETWORKMANAGER_H
#define NETWORKMANAGER_H

#include <QObject>
#include <QUdpSocket>
#include "../ZRTP/zrtp.h"

class NetworkManager : public QObject
{
    Q_OBJECT

    typedef bool (*function)(const uint8_t* data, int32_t length);

public:
    explicit NetworkManager(QObject *parent = 0);

signals:

public slots:
    bool sendData(const uint8_t *data, int32_t length);

private:
    static QUdpSocket *socket;

    Zrtp *zrtp;
    ZrtpCallback *callbacks;
};

#endif // NETWORKMANAGER_H
