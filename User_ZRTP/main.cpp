#include <QCoreApplication>
#include "networkmanager.h"



int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    NetworkManager *manager = new NetworkManager(argc,argv);

    return a.exec();
}
