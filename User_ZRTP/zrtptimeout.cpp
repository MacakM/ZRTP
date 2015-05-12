#include "zrtptimeout.h"

ZrtpTimeout::ZrtpTimeout(NetworkManager *manager, int32_t delay)
{
    this->manager = manager;
    this->delay = delay;
}

void ZrtpTimeout::run()
{
    if(delay != 0)
    {
        qsrand(QTime::currentTime().msec());
        int32_t delayTime = qrand() % delay;
        msleep(delayTime);
    }
    manager->processZrtpTimeout();
    return;
}
