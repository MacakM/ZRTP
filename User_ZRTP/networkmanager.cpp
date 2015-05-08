#include "networkmanager.h"

HANDLE semaphoreA;
HANDLE semaphoreB;
HANDLE semaphoreEnd;

NetworkManager::NetworkManager(int argc, char *argv[], QObject *parent) :
    QObject(parent)
{
    if(strcmp(argv[1],"--create-semaphores") == 0)
    {
        semaphoreA = CreateSemaphore(NULL,0,1,L"Global\\semaphoreA");
        semaphoreB = CreateSemaphore(NULL,0,1,L"Global\\semaphoreB");
        semaphoreEnd = CreateSemaphore(NULL,0,2,L"Global\\semaphoreEnd");
        WaitForSingleObject(semaphoreEnd,INFINITE);
        WaitForSingleObject(semaphoreEnd,INFINITE);
        CloseHandle(semaphoreA);
        CloseHandle(semaphoreB);
        CloseHandle(semaphoreEnd);
        ended = true;
        return;
    }

    semaphoreA = OpenSemaphore(SEMAPHORE_ALL_ACCESS, TRUE, L"Global\\semaphoreA");
    semaphoreB = OpenSemaphore(SEMAPHORE_ALL_ACCESS, TRUE, L"Global\\semaphoreB");
    semaphoreEnd = OpenSemaphore(SEMAPHORE_ALL_ACCESS, TRUE, L"Global\\semaphoreEnd");

    if(semaphoreA == NULL || semaphoreB == NULL || semaphoreEnd == NULL)
    {
        std::cout << "Please, first create semaphores only with --create-semaphores option" << std::endl;
        ended = true;
        return;
    }

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

    ReleaseSemaphore((myRole == Initiator) ? semaphoreA : semaphoreB,1,NULL);
    WaitForSingleObject((myRole == Initiator)? semaphoreB : semaphoreA,INFINITE);

    zrtp = new Zrtp(callbacks, myRole, "MacakM", &info);
    memcpy(myZid,zrtp->getZid(),ZID_SIZE);
}

NetworkManager::~NetworkManager()
{
    delete(zrtp);
    info.versions.clear();
    info.hashTypes.clear();
    info.cipherTypes.clear();
    info.authTagTypes.clear();
    info.keyAgreementTypes.clear();
    info.sasTypes.clear();
    delete(mutex);
    delete(callbacks);
    delete(timer);
    delete(sendSocket);
    delete(readSocket);
    ReleaseSemaphore(semaphoreEnd,1,NULL);
}

void NetworkManager::setActualSignal(uint8_t signalNumber, int32_t time)
{
    actualSignal = (Signal)signalNumber;
    actualTime = time;
    emit signalReceived();
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
            ReleaseSemaphore((myRole == Initiator) ? semaphoreA : semaphoreB,1,NULL);

            Restarter *t = new Restarter(this, (myRole == Initiator)? semaphoreB : semaphoreA);
            threads.push_back(t);
            t->start();
        }
        else
        {
            std::cout << "Call secured" << std::endl;
            endZrtp();
        }
    }
    else if(actualSignal == zrtpFailed)
    {
        std::cout << "Agreement failed!" << std::endl;
        endZrtp();
    }
    else if(actualSignal == zrtpRestart)
    {
        std::cout << "Call secured" << std::endl;
        restartZrtp();
    }
}

void NetworkManager::restartZrtp()
{
    //wait for all threads to end
    for(int i = threads.size() - 1; i >= 0; i--)
    {
        QThread *thread = threads.at(i);
        thread->wait();
        delete(threads.at(i));
        threads.erase(threads.begin() + i);
    }
    counter++;
    std::cout << "COUNTER: " << counter << std::endl;
    if(counter == 10000)
    {
        ended = true;
    }
	else
    {
		delete (zrtp);
		zrtp = new Zrtp(callbacks, myRole, "MacakM", &info);
	}
}

void NetworkManager::endZrtp()
{
    //wait for all threads to end
    for(int i = threads.size() - 1; i >= 0; i--)
    {
        QThread *thread = threads.at(i);
        thread->wait();
        delete(threads.at(i));
        threads.erase(threads.begin() + i);
    }

    ended = true;
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
    threads.push_back(t);
    t->start();
}
