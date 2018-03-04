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

#ifndef CCONTCPMAIN_H
#define CCONTCPMAIN_H

#include <QtCore/QObject>
#include <QtCore/QList>
#include <QtNetwork/QTcpServer>

#include "udpdataserver.h"
#include "connectiondata.h"
//#include "General/globaldata.h"
#include <../Common/General/backgroundworker.h>
#include <../Common/General/backgroundcontroller.h>
#include <../Common/Network/messagebuffer.h>


//struct UserConnection {
//    MessageBuffer           msgBuffer;
//    UserConData             userConData;
//    UdpDataServer           *pDataServer;
//    BackgroundController    *pctrlUdpDataServer;
//};


class cConTcpMain : public BackgroundWorker
{
    Q_OBJECT
public:
    cConTcpMain();
    ~cConTcpMain();

    qint32 initialize();

protected:
    int DoBackgroundWork() override;

//    QString m_workerName = "UDPServer";

private slots:
    void slotSocketConnected();
    void readyReadMasterPort();
//    void readChannelFinished();

//    void onConnectionTimedOut(quint16 port);

private:
    QTcpServer                  *m_pTcpMasterServer = NULL;

//    GlobalData                  *m_pGlobalData;

//    QList<UserConnection>       m_lUserCons;

//    UserConnection  *getUserMasterConnection(QHostAddress addr, quint16 port);

//    quint16 getFreeDataPort();

//    void checkNewOncomingData();
};

#endif // CCONTCPMAIN_H
