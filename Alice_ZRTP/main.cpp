#include <QCoreApplication>
#include "networkmanager.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    NetworkManager *manager = new NetworkManager();

    //std::cout << manager->getMyZid(); problem with uint8_t
    return a.exec();
}
