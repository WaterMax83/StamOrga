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

#ifndef CCONTCPMAINDATA_H
#define CCONTCPMAINDATA_H

#include <QtCore/QList>
#include <QtCore/QMutex>

#include "../../Common/Network/messagebuffer.h"
#include "../Common/General/backgroundcontroller.h"
#include "../Common/General/cgendisposer.h"
#include "Data/listeduser.h"
#include "ccontcpdataserver.h"
#include "connectiondata.h"


#define TCP_PORT 55000

struct TcpUserConnection {
    UserConData           m_userConData;
    cConTcpDataServer*    m_pDataServer;
    BackgroundController* m_pctrlTcpDataServer;
};

class cConTcpMainData : public cGenDisposer
{
public:
    cConTcpMainData();

    qint32 initialize(ListedUser* pListedUser);

    MessageProtocol* getNewUserAcknowledge(const QString userName, const QHostAddress addr);

    MessageProtocol* getUserCheckLogin(UserConData* pUserCon, MessageProtocol* request);

    MessageProtocol* getUserCheckVersion(UserConData* pUserCon, MessageProtocol* request);

    MessageProtocol* getUserProperties(UserConData* pUserCon, MessageProtocol* request);

    MessageProtocol* getUserChangeReadableName(UserConData* pUserCon, MessageProtocol* request);

    MessageProtocol* getUserChangePassword(UserConData* pUserCon, MessageProtocol* request);

private slots:
    void slotServerClosed(quint16 destPort);

private:
    ListedUser*               m_pListedUser;
    QList<TcpUserConnection*> m_lTcpUserCons;
    QMutex                    m_mutex;

    quint16 getFreeDataPort();
};

extern cConTcpMainData g_ConTcpMainData;

#endif // CCONTCPMAINDATA_H
