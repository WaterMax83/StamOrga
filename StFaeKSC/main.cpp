#include <QtCore/QCoreApplication>
#include <QtCore/QFile>

#include "../Common/General/backgroundcontroller.h"
#include "../Common/General/backgroundworker.h"
#include "../Common/General/globalfunctions.h"
#include "../Common/General/logging.h"
#include "Network/udpserver.h"
#include "General/globaldata.h"
#include "General/console.h"




int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    QCoreApplication::setOrganizationName("WaterMax");
    QCoreApplication::setOrganizationDomain("watermax.com");
    QCoreApplication::setApplicationName("StFaeKSC");

    QString logginPath = QCoreApplication::applicationDirPath() + "/Log/Actual.log";

    if (!CheckFilePathExistAndCreate(logginPath)) {
        CONSOLE_CRITICAL(QString("Could not create file for Logging"));
    }
    else {
        QFile *logFile = new QFile(logginPath);
        if (logFile->open(QFile::ReadWrite | QFile::Text | QFile::Append))
            logSetLogFile(logFile);
    }

    qInstallMessageHandler(logMyMessageLogginOutPut);

    qDebug() << "*************************************************************";
    qInfo() << "Starting StFaeKSC";

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
