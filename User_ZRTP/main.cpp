#include <QCoreApplication>
#include "networkmanager.h"

//just for testing
#include <windows.h>
#include <conio.h>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    //testing
    Sleep(10000);

    NetworkManager *manager = new NetworkManager(argc,argv);


    //std::cout << manager->getMyZid(); problem with uint8_t
    return a.exec();
}
