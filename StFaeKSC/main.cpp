#include <QtCore/QCoreApplication>
#include <QtCore/QFile>
#include <QtCore/QTextStream>
#include <QtCore/QDateTime>

#include "../Common/General/backgroundcontroller.h"
#include "../Common/General/backgroundworker.h"
#include "../Common/General/globalfunctions.h"
#include "Network/udpserver.h"
#include "General/globaldata.h"
#include "General/console.h"

static QFile *m_logFile = NULL;

void myMessageOutput(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    QString outPutString;
    switch (type) {
    case QtDebugMsg:
        outPutString = QString("Debug:   %1\n").arg(msg);
        break;
    case QtInfoMsg:
        outPutString = QString("Info:    %1\n").arg(msg);
        break;
    case QtWarningMsg:
        outPutString = QString("Warning: %1\n").arg(msg);
        break;
    case QtCriticalMsg:
        outPutString = QString("Error:   %1\n").arg(msg);
        break;
    case QtFatalMsg:
        outPutString = QString("Fatal:   %1\n").arg(msg);
        abort();
    }

    if (m_logFile != NULL) {
        QTextStream out(m_logFile);
        out << QDateTime::currentDateTime().toString("MMM.dd hh:mm:ss.zzz") << ": " << outPutString;
    }
    else if (type != QtDebugMsg && type != QtInfoMsg){
        std::cout << outPutString.toStdString() << std::endl;
    }
}


int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    QCoreApplication::setOrganizationName("WaterMax");
    QCoreApplication::setOrganizationDomain("watermax.com");
    QCoreApplication::setApplicationName("StFaeKSC");

    QString logginPath = QCoreApplication::applicationDirPath() + "/Log/Actual.log";

    if (!CheckFilePathExistAndCreate(logginPath)) {
        CONSOLE_CRITICAL(QString("Could not create File for Logging"));
    }
    else {
        m_logFile = new QFile(logginPath);
        if (!m_logFile->open(QFile::WriteOnly | QFile::Text | QFile::Append))
            m_logFile = NULL;
    }

    qInstallMessageHandler(myMessageOutput);

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
