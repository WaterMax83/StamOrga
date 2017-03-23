#include <QCoreApplication>

#include "../Common/General/backgroundcontroller.h"
#include "../Common/General/backgroundworker.h"
#include "Network/udpserver.h"
#include "General/globaldata.h"
#include "General/console.h"


int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    QCoreApplication::setOrganizationName("WaterMax");
    QCoreApplication::setOrganizationDomain("watermax.com");
    QCoreApplication::setApplicationName("StFaeKSC");

    GlobalData globalData;

    BackgroundController ctrlUdp;
    UdpServer *udpServ = new UdpServer(&globalData);

    /* TODO: Connect for ctrlUdp::notifyBackgroundWorkerFinished */

    ctrlUdp.Start(udpServ, false);

    Console con(&globalData);
    con.run();
    QObject::connect(&con, SIGNAL(quit()), &a, SLOT(quit()));

    return a.exec();
}
