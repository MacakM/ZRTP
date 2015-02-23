#include <QCoreApplication>
#include "networkmanager.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    NetworkManager *manager = new NetworkManager();

    return a.exec();
}
