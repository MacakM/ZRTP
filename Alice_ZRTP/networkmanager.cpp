#include "networkmanager.h"

NetworkManager::NetworkManager(QObject *parent) :
    QObject(parent)
{
    sendSocket = new QUdpSocket();
    readSocket = new QUdpSocket();

    readSocket->bind(QHostAddress::LocalHost, 4321);
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

void NetworkManager::setArguments(Arguments args)
{
    if(args.role == '0')
    {
        myRole = Initiator;
    }
    else    //default - Responder
    {
        myRole = Responder;
    }

    /*PROBLEM WITH BIND
     *if(args.receiveIp != NULL)
    {
        if(args.receivePort != NULL)
        {
            readSocket->bind(args.receiveIp, args.receivePort);
        }
        else
        {
            readSocket->bind(args.receiveIp, 41001);
        }
    }
    else
    {
        readSocket->bind("0.0.0.0", 41001);
    }
    connect(readSocket, SIGNAL(readyRead()),
            this, SLOT(processPendingDatagram()));
    */
}
