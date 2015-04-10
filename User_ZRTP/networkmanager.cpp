#include "networkmanager.h"

NetworkManager::NetworkManager(int argc, char *argv[], QObject *parent) :
    QObject(parent)
{
    ended = false;
    counter = 0;
    qsrand(QTime::currentTime().msec());
    threads.reserve(15);
    actualSignal = (Signal)0;
    actualTime = 0;

    connect(this,SIGNAL(signalReceived()),this,SLOT(processSignal()));
    mutex = new QMutex();
    sendSocket = new QUdpSocket();
    readSocket = new QUdpSocket();

    restartTimer = new QTimer();
    restartTimer->setSingleShot(true);
    connect(restartTimer, SIGNAL(timeout()), this, SLOT(restartZrtp()));

    setArguments(Parser::getArguments(argc,argv));

    info.versions.push_back("1.10");
    //demonstration of version negotiation
    if(myRole == Responder) info.versions.push_back("1.20");
    info.hashTypes.push_back("S256");
    info.cipherTypes.push_back("AES1");
    info.authTagTypes.push_back("HS32");
    //demonstration of algorithm negotiation
    if (myRole == Responder) info.keyAgreementTypes.push_back("EC52");
    info.keyAgreementTypes.push_back("DH3k");
    if (myRole == Initiator) info.keyAgreementTypes.push_back("EC52");
    info.sasTypes.push_back("B32 ");

    (myRole == Initiator) ? myFile.open("Alice.txt") : myFile.open("Bob.txt");
    myFile.close();
    readSocket->bind(receiveIp, receivePort);
    connect(readSocket, SIGNAL(readyRead()),
                this, SLOT(processPendingDatagram()));

    timer = new QTimer();
    timer->setSingleShot(true);
    connect(timer, SIGNAL(timeout()), this, SLOT(createTimeoutThread()));

    callbacks = new MyCallbacks(this);
    //testing
    (myRole == Initiator) ? Sleep(10000) : Sleep(5000);

    zrtp = new Zrtp(callbacks, myRole, "MacakM", &info);
    memcpy(myZid,zrtp->getZid(),ZID_SIZE);
}

void NetworkManager::setActualSignal(uint8_t signalNumber, int32_t time)
{
    actualSignal = (Signal)signalNumber;
    actualTime = time;
    emit signalReceived();
}

bool NetworkManager::hasEnded()
{
    return ended;
}

void NetworkManager::processPendingDatagram()
{
    int8_t randValue = qrand() % 100;

    QByteArray datagram;
    QHostAddress sender;
    quint16 senderPort;
    int32_t size;

    size = (int32_t)readSocket->pendingDatagramSize();
    datagram.resize(readSocket->pendingDatagramSize());
    readSocket->readDatagram(datagram.data(), datagram.size(), &sender, &senderPort);

    // simulated packet loss
    if(randValue < packetLoss)
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

    uint8_t *message = new uint8_t[size];
    memcpy(message,datagram.data(),size);
    ZrtpMessage *t = new ZrtpMessage(this,message,size,packetDelay);
    connect(t, SIGNAL(finished()), t, SLOT(deleteLater()));
    threads.push_back(t);
    t->start();
}

void NetworkManager::processSignal()
{
    if (actualSignal == activateTimer)
    {
        timer->start(actualTime);
    }
    else if (actualSignal == stopTimer)
    {
        timer->stop();
    }
    else if (actualSignal == zrtpEnded)
    {
        if(testing)
        {
            //restart zrtp communication after short delay
            (myRole == Responder) ? restartTimer->start(7000) : restartTimer->start(5000);
        }
        else
        {
            std::cout << "Call secured" << std::endl;
            ended = true;
        }
    }
    else if(actualSignal == zrtpFailed)
    {
        std::cout << "Agreement failed!" << std::endl;
    }
}

void NetworkManager::restartZrtp()
{
    delete (zrtp);
    for(int i = threads.size() - 1; i >= 0; i--)
    {
        threads.erase(threads.begin() + i);
    }
    counter++;
    std::cout << "COUNTER: " << counter << std::endl;
    if(counter == 10000)
    {
        std::cin.get();
    }
    zrtp = new Zrtp(callbacks, myRole, "MacakM", &info);
}

void NetworkManager::processZrtpTimeout()
{
    zrtp->processTimeout();
}

void NetworkManager::processZrtpMessage(uint8_t *msg, int32_t length)
{
    zrtp->processMessage(msg, length);
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

    if(args.packetLoss != NULL)
    {
        packetLoss = atoi(args.packetLoss);
    }
    else
    {
        packetLoss = 0;
    }

    if(args.packetDelay != NULL)
    {
        packetDelay = atoi(args.packetDelay);
    }
    else
    {
        packetDelay = 0;
    }

    if(args.testing == '1')
    {
        testing = true;
    }
    else
    {
        testing = false;
    }

}

void NetworkManager::createTimeoutThread()
{
    ZrtpTimeout *t = new ZrtpTimeout(this,packetDelay);
    connect(t, SIGNAL(finished()), t, SLOT(deleteLater()));
    threads.push_back(t);
    t->start();
}
