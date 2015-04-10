#include "mycallbacks.h"

MyCallbacks::MyCallbacks(NetworkManager *manager)
{
    this->manager = manager;
}

bool MyCallbacks::sendData (const uint8_t* data, int32_t length)
{
    assert(data);
    QByteArray datagram((char*)data, length);

    if (manager->sendSocket->writeDatagram(datagram, manager->sendIp, manager->sendPort) == -1)
    {
        return false;
    }
    std::cout << "Sending " << data[4] << data[5] << data[6] << data[7] << data[8] << data[9] << data[10] << data[11] << std::endl;
    return true;
}

bool MyCallbacks::activateTimer(int32_t time)
{
    manager->setActualSignal(1, time);
    return true;
}

bool MyCallbacks::cancelTimer()
{
    manager->setActualSignal(2);
    return true;
}

void MyCallbacks::enterCriticalSection()
{
    manager->mutex->lock();
}

void MyCallbacks::leaveCriticalSection()
{
    manager->mutex->unlock();
}

void MyCallbacks::keyAgreed(SrtpMaterial *material)
{
    // save all SRTP material
    memcpy(manager->material.srtpKeyI,material->srtpKeyI,AES1_KEY_LENGTH);
    memcpy(manager->material.srtpKeyR,material->srtpKeyR,AES1_KEY_LENGTH);
    memcpy(manager->material.srtpSaltI,material->srtpSaltI,SALT_KEY_LENGTH);
    memcpy(manager->material.srtpSaltR,material->srtpSaltR,SALT_KEY_LENGTH);
    memcpy(manager->material.sas,material->sas,SAS_LENGTH);

    //print SRTP keys and salts
    std::cout << "SRTP Initiator's key: ";
    for(int8_t i = 0; i < AES1_KEY_LENGTH; i++)
    {
        std::cout << std::hex << material->srtpKeyI[i];
    }
    std::cout << std::endl << "SRTP Initiator's salt: ";
    for(int8_t i = 0; i < SALT_KEY_LENGTH; i++)
    {
        std::cout << std::hex << material->srtpSaltI[i];
    }
    std::cout << std::endl << "SRTP Responder's key: ";
    for(int8_t i = 0; i < AES1_KEY_LENGTH; i++)
    {
        std::cout << std::hex << material->srtpKeyR[i];
    }
    std::cout << std::endl << "SRTP Responder's salt: ";
    for(int8_t i = 0; i < SALT_KEY_LENGTH; i++)
    {
        std::cout << std::hex << material->srtpSaltR[i];
    }
    std::cout << std::endl;
    //print SAS
    std::cout << std::endl << "SAS: ";
    for(int8_t i = 0; i < SAS_LENGTH; i++)
    {
        std::cout << std::hex << material->sas[i];
    }
    std::cout << std::endl;

    delete (material);

    manager->setActualSignal(3);
}

void MyCallbacks::agreementFailed()
{
    manager->setActualSignal(4);
}
