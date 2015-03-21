#include "networkmanager.h"

NetworkManager::NetworkManager(int argc, char *argv[], QObject *parent) :
    QObject(parent)
{
    mutex = new QMutex();
    srand (time(NULL));
    sendSocket = new QUdpSocket();
    readSocket = new QUdpSocket();

    setArguments(Parser::getArguments(argc,argv));

    (myRole == Initiator) ? myFile.open("Alice.txt") : myFile.open("Bob.txt");
    myFile.close();
    readSocket->bind(receiveIp, receivePort);
    connect(readSocket, SIGNAL(readyRead()),
                this, SLOT(processPendingDatagram()));

    timer.setSingleShot(true);
    connect(&timer, SIGNAL(timeout()), this, SLOT(processZrtpTimeout()));

    callbacks = new MyCallbacks(this);

    //testing
    (myRole == Initiator) ? Sleep(10000) : Sleep(5000);

    zrtp = new Zrtp(callbacks, myRole, "MacakM");
    myZid = zrtp->getZid();
}

void NetworkManager::processPendingDatagram()
{
    uint8_t randValue = rand() % 5;

    QByteArray datagram;
    QHostAddress sender;
    quint16 senderPort;
    int32_t size;

    size = (int32_t)readSocket->pendingDatagramSize();
    datagram.resize(readSocket->pendingDatagramSize());
    readSocket->readDatagram(datagram.data(), datagram.size(), &sender, &senderPort);

    // simulated 20% packet loss
    if(randValue == 0)
    {
        return;
    }

    (myRole == Initiator) ? myFile.open("Alice.txt",std::ios::app) : myFile.open("Bob.txt",std::ios::app);
    myFile << "Message: " << std::endl;
    for(int i = 0; i < size; i++)
    {
        myFile << datagram[i];
        if(i % 12 == 11)
        {
            myFile << std::endl;
        }
    }
    myFile << std::endl;
    myFile << "Sender: " << sender.toString().toStdString();
    myFile << " Port: " << senderPort;
    myFile << std::endl << std::endl;
    myFile.close();

    processZrtpMessage((uint8_t*)datagram.data(), size);
}

void NetworkManager::processZrtpTimeout()
{
    zrtp->processTimeout();
}

void NetworkManager::processZrtpMessage(uint8_t *msg, int32_t length)
{
    zrtp->processMessage(msg, length);
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
