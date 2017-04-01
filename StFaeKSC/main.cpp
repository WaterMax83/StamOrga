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

    QString logginPath = getUserHomeConfigPath() + "/Log/Actual.log";

    if (!checkFilePathExistAndCreate(logginPath)) {
        CONSOLE_CRITICAL(QString("Could not create file for Logging"));
    }
    else {
        QFile *logFile = new QFile(logginPath);
        if (logFile->open(QFile::ReadWrite | QFile::Text | QFile::Append))
            logSetLogFile(logFile);
    }

    qInstallMessageHandler(logMyMessageLogginOutPut);

    qInfo() << "*************************************************************";
    qInfo() << "Starting StFaeKSC";

    GlobalData globalData;

    BackgroundController ctrlUdp;
    if (argc > 1 && QString(argv[1]) == "-noServer") {
        qInfo() << "Starting only as a deamon without a server";
    } else {

        UdpServer *udpServ = new UdpServer(&globalData);

        ctrlUdp.Start(udpServ, false);
    }

    /* TODO: Connect for ctrlUdp::notifyBackgroundWorkerFinished */



    Console con(&globalData);
    con.run();
    QObject::connect(&con, SIGNAL(quit()), &a, SLOT(quit()));

    int result = a.exec();

    qDebug().noquote() << QString("Ending program %1: %2").arg(result).arg(QCoreApplication::applicationPid());

    return result;
}
