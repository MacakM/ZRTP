#include "networkmanager.h"

NetworkManager::NetworkManager(QObject *parent) :
    QObject(parent)
{
    sendSocket = new QUdpSocket();
    readSocket = new QUdpSocket();
    readSocket->bind(QHostAddress::LocalHost, 1234);
    connect(readSocket, SIGNAL(readyRead()),
                this, SLOT(processPendingDatagram()));

    myZid = 8;

    callbacks = new MyCallbacks(this);

    zrtp = new Zrtp(&myZid, callbacks);
}

void NetworkManager::processPendingDatagram()
{
    QByteArray datagram;
    QHostAddress sender;
    quint16 senderPort;

    datagram.resize(readSocket->pendingDatagramSize());
    readSocket->readDatagram(datagram.data(), datagram.size(), &sender, &senderPort);

    qDebug() << "Message: " << datagram;
    qDebug() << "Sender: " << sender;
    qDebug() << "Port: " << senderPort;
}

uint8_t NetworkManager::getMyZid()
{
     return myZid;
}
