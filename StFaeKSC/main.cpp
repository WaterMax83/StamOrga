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
*/

#include <QtCore/QCoreApplication>
#include <QtCore/QDataStream>
#include <QtCore/QDateTime>
#include <QtCore/QFile>
#include <QtCore/QUuid>

#ifdef Q_OS_LINUX
#include <sys/resource.h>
#endif
#include "../Common/General/backgroundcontroller.h"
#include "../Common/General/backgroundworker.h"
#include "../Common/General/config.h"
#include "../Common/General/globalfunctions.h"
#include "../Common/General/logging.h"
#include "Data/checkconsistentdata.h"
#include "Data/readonlinegames.h"
#include "General/console.h"
#include "General/globaldata.h"
#include "General/pushnotification.h"
#include "Manager/cglobalmanager.h"
#include "Network/ccontcpmaindata.h"
#include "Network/ccontcpmainserver.h"
//#include "Network/udpserver.h"

GlobalData* g_GlobalData;

#define CORE_DUMP_FILE_SIZE_MAX (256 * 1024 * 1024)

int main(int argc, char* argv[])
{
    QCoreApplication a(argc, argv);
    QCoreApplication::setOrganizationName("WaterMax");
    QCoreApplication::setOrganizationDomain("watermax.com");
    QCoreApplication::setApplicationName("StFaeKSC");

#ifdef Q_OS_LINUX
    // core dumps may be disallowed by parent of this process; change that
    struct rlimit core_limits;
    getrlimit(RLIMIT_CORE, &core_limits);
    core_limits.rlim_cur = core_limits.rlim_max = CORE_DUMP_FILE_SIZE_MAX;
    setrlimit(RLIMIT_CORE, &core_limits);
#endif

    qInfo().noquote() << "*************************************************************";
    qInfo().noquote() << QString("Starting StFaeKSC %1").arg(STAM_ORGA_VERSION_S);

    GlobalData globalData;
    g_GlobalData = &globalData;
    globalData.initialize();

    g_Console = new Console();

    // For Logging output
    qInfo().noquote() << "*************************************************************";
    qInfo().noquote() << QString("Starting StFaeKSC %1").arg(STAM_ORGA_VERSION_S);

    PushNotification pushNotify;
    pushNotify.initialize(&globalData);

    //    BackgroundController ctrlUdp;
    BackgroundController ctrlTcp;
    cGlobalManager       globalManager;
    if (argc > 1 && QString(argv[1]) == "-noServer") {
        qInfo() << "Starting only as a deamon without a server";
    } else {

        //        UdpServer* udpServ = new UdpServer(&globalData);
        //        ctrlUdp.Start(udpServ, false);

        cConTcpMainServer* tcpMain = new cConTcpMainServer();
        tcpMain->initialize();
        ctrlTcp.Start(tcpMain, false);

        globalManager.initialize();
        g_ConTcpMainData.initialize(&globalData.m_UserList);
    }

    BackgroundController ctrlConsistent;
    CheckConsistentData* checkConsistData = new CheckConsistentData();
    checkConsistData->initialize();
    ctrlConsistent.Start(checkConsistData, false);

    /* TODO: Connect for ctrlUdp::notifyBackgroundWorkerFinished */

    g_Console->run();
    QObject::connect(g_Console, SIGNAL(quit()), &a, SLOT(quit()));

    int result = a.exec();

    qDebug().noquote() << QString("Ending program %1: %2").arg(result).arg(QCoreApplication::applicationPid());
    //    ctrlUdp.Stop();
    delete g_Console;

    delete checkConsistData;

    return result;
}
