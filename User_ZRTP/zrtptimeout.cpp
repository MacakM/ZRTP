#include "zrtptimeout.h"

ZrtpTimeout::ZrtpTimeout(NetworkManager *manager)
{
    this->manager = manager;
}

void ZrtpTimeout::run()
{
    manager->processZrtpTimeout();
    return;
}
