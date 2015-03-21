#include "zrtpmessage.h"

ZrtpMessage::ZrtpMessage()
{
}

void ZrtpMessage::run()
{
    qDebug()<<"From worker thread: "<<currentThreadId();
}
