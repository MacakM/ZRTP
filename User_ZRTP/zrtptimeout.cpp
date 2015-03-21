#include "zrtptimeout.h"

ZrtpTimeout::ZrtpTimeout()
{
}

void ZrtpTimeout::run()
{
    qDebug()<<"From worker thread: "<<currentThreadId();
}
