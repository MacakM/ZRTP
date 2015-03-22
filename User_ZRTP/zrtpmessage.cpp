#include "zrtpmessage.h"

ZrtpMessage::ZrtpMessage(NetworkManager *manager, uint8_t *msg, int32_t length)
{
    this->manager = manager;
    this->msg = msg;
    this->length = length;
}

void ZrtpMessage::run()
{
    manager->processZrtpMessage(msg,length);
    return;
}
