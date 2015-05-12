#include <QCoreApplication>
#include "networkmanager.h"



int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    NetworkManager *manager = new NetworkManager(argc,argv);

    while(manager->hasEnded() == false)
    {
        a.processEvents();
    }

    delete(manager);

    return 0;
}
