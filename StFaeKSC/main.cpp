/*
*	This file is part of StamOrga
*   Copyright (C) 2017 Markus Schneider
*
*	This program is free software; you can redistribute it and/or modify
*   it under the terms of the GNU General Public License as published by
*   the Free Software Foundation; either version 3 of the License, or
*   (at your option) any later version.
*
*	StamOrga is distributed in the hope that it will be useful,
*    but WITHOUT ANY WARRANTY; without even the implied warranty of
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*    GNU General Public License for more details.

*    You should have received a copy of the GNU General Public License
*    along with StamOrga.  If not, see <http://www.gnu.org/licenses/>.
*/#include <QtCore/QCoreApplication>
#include <QtCore/QDataStream>
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
