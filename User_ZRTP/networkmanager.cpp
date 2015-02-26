#include "networkmanager.h"

NetworkManager::NetworkManager(int argc, char *argv[], QObject *parent) :
    QObject(parent)
{
    sendSocket = new QUdpSocket();
    readSocket = new QUdpSocket();

    setArguments(Parser::getArguments(argc,argv));

    readSocket->bind(receiveIp, receivePort);
    connect(readSocket, SIGNAL(readyRead()),
                this, SLOT(processPendingDatagram()));

    timer.setSingleShot(true);
    connect(&timer, SIGNAL(timeout()), this, SLOT(sendTimeout()));

    if(myRole == Initiator)
    {
        myZid = 8;
    }
    else    //Responder
    {
        myZid = 14;
    }

    callbacks = new MyCallbacks(this);

    //testing
    Sleep(10000);

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

void NetworkManager::sendTimeout()
{
    zrtp->processTimeout();
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

    if(args.receiveIp != NULL)
    {
        receiveIp = QHostAddress(args.receiveIp);
    }
    else    //default - 0.0.0.0
    {
        receiveIp = QHostAddress("0.0.0.0");
    }
    if(args.receivePort != NULL)
    {
        receivePort = atoi(args.receivePort);
    }
    else    //default 41001 - Initiator, 41002 - Responder
    {
        if(myRole == Initiator)
        {
            receivePort = 41001;
        }
        else
        {
            receivePort = 41002;
        }
    }

    if(args.sendIp != NULL)
    {
        sendIp = QHostAddress(args.sendIp);
    }
    else    //default 127.0.0.1
    {
        sendIp = QHostAddress("127.0.0.1");
    }
    if(args.sendPort != NULL)
    {
        sendPort = atoi(args.sendPort);
    }
    else    //default 41001 - Responder, 41002 - Initiator
    {
        if(myRole == Initiator)
        {
            sendPort = 41002;
        }
        else
        {
            sendPort = 41001;
        }
    }
}