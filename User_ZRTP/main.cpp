#include <QCoreApplication>
#include "networkmanager.h"



int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    NetworkManager *manager = new NetworkManager(argc,argv);


    //std::cout << manager->getMyZid(); problem with uint8_t
    return a.exec();
}
