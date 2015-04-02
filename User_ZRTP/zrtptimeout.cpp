#include "zrtptimeout.h"

ZrtpTimeout::ZrtpTimeout(NetworkManager *manager, int32_t delay)
{
    this->manager = manager;
    this->delay = delay;
}

void ZrtpTimeout::run()
{
    qsrand(QTime::currentTime().msec());
    int32_t delayTime = qrand() % delay;
    Sleep(delayTime);
    manager->processZrtpTimeout();
    return;
}
