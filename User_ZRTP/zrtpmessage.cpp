#include "zrtpmessage.h"

ZrtpMessage::ZrtpMessage(NetworkManager *manager, uint8_t *msg, int32_t length)
{
    this->manager = manager;
    this->msg = msg;
    this->length = length;
}

void ZrtpMessage::run()
{
    std::cout << "Received " << msg[4] << msg[5] << msg[6] << msg[7] << msg[8] << msg[9] << msg[10] << msg[11]<< std::endl;
    manager->processZrtpMessage(msg,length);
    free(msg);
    return;
}
