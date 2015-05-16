#include "networkmanager.h"

QSystemSemaphore *semaphoreA;
QSystemSemaphore *semaphoreB;

NetworkManager::NetworkManager(int argc, char *argv[], QObject *parent) :
    QObject(parent)
{
    //initalize
    semaphoreA = new QSystemSemaphore("semaphoreA", 0, QSystemSemaphore::Open);
    semaphoreB = new QSystemSemaphore("semaphoreB", 0, QSystemSemaphore::Open);
    restarted = false;
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

    //add supported algorithms
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

    (myRole == Initiator) ? semaphoreA->release() : semaphoreB->release();
    (myRole == Initiator) ? semaphoreB->acquire() : semaphoreA->acquire();

    elapsedTimer.start();
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
    delete(semaphoreA);
    delete(semaphoreB);
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
        //better output structure
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
        counter++;
        std::cout << "COUNTER: " << std::dec << counter << std::endl;
        if(counter == testCap)
        {
            std::cout << "Call secured" << std::endl;
            endZrtp();
        }
        else
        {
            if(restarted)
            {
                return;
            }
            (myRole == Initiator) ? semaphoreA->release() : semaphoreB->release();
            restarted = true;

            Restarter *t = new Restarter(this, (myRole == Initiator)? semaphoreB : semaphoreA);
            threads.push_back(t);
            t->start();
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
    restarted = false;

    delete (zrtp);
    zrtp = new Zrtp(callbacks, myRole, "MacakM", &info);
}

void NetworkManager::endZrtp()
{
    std:: cout << "Test took " << elapsedTimer.elapsed() << " milliseconds." << std::endl;
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

void NetworkManager::setArguments(Arguments *args)
{
    //default role Responder
    (args->role == '0') ? myRole = Initiator : myRole = Responder;

    //default receiveIp 0.0.0.0
    (args->receiveIp != NULL)? receiveIp = QHostAddress(args->receiveIp) : receiveIp = QHostAddress("0.0.0.0");

    //default receivePort 41001 - Initiator, 41002 - Responder
    (args->receivePort != NULL) ? receivePort = atoi(args->receivePort) :
        (myRole == Initiator) ? receivePort = 41001 : receivePort = 41002;

    //default sendIp 127.0.0.1
    (args->sendIp != NULL) ? sendIp = QHostAddress(args->sendIp) : sendIp = QHostAddress("127.0.0.1");

    //default sendPort 41001 - Responder, 41002 - Initiator
    (args->sendPort != NULL) ? sendPort = atoi(args->sendPort) :
        (myRole == Initiator) ? sendPort = 41002 : sendPort = 41001;

    //default packetLoss 0
    (args->packetLoss != NULL) ? packetLoss = atoi(args->packetLoss) : packetLoss = 0;

    //default packetDelay 0
    (args->packetDelay != NULL) ? packetDelay = atoi(args->packetDelay) : packetDelay = 0;

    //default number of iterations 1
    (args->testing != NULL) ? testCap = atoi(args->testing) : testCap = 1;
    delete(args);
}

void NetworkManager::createTimeoutThread()
{
    ZrtpTimeout *t = new ZrtpTimeout(this,packetDelay);
    threads.push_back(t);
    t->start();
}
