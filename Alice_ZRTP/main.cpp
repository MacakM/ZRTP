#include <QCoreApplication>
#include "networkmanager.h"
#include "parser.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    NetworkManager *manager = new NetworkManager();

    manager->setArguments(Parser::getArguments(argc,argv));


    //std::cout << manager->getMyZid(); problem with uint8_t
    return a.exec();
}
