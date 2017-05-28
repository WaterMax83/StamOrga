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

#ifndef DATACONNECTION_H
#define DATACONNECTION_H

#include <QObject>

#include "../Common/Network/messageprotocol.h"
#include "../Network/connectiondata.h"
#include "globaldata.h"

class DataConnection : public QObject
{
    Q_OBJECT
public:
    explicit DataConnection(GlobalData* pGData, QObject* parent = 0);

    MessageProtocol* requestCheckUserLogin(MessageProtocol* msg);
    MessageProtocol* requestGetUserProperties();
    MessageProtocol* requestUserChangeLogin(MessageProtocol* msg);
    MessageProtocol* requestUserChangeReadname(MessageProtocol* msg);
    MessageProtocol* requestGetProgramVersion(MessageProtocol* msg);
    MessageProtocol* requestGetGamesList(MessageProtocol* msg);
    MessageProtocol* requestGetTicketsList(/*MessageProtocol *msg*/);
    MessageProtocol* requestAddSeasonTicket(MessageProtocol* msg);
    MessageProtocol* requestRemoveSeasonTicket(MessageProtocol* msg);
    MessageProtocol* requestNewPlaceSeasonTicket(MessageProtocol* msg);
    MessageProtocol* requestChangeStateSeasonTicket(MessageProtocol* msg);
    MessageProtocol* requestGetAvailableTicketList(MessageProtocol* msg);
    MessageProtocol* requestChangeGame(MessageProtocol* msg);

    void setUserConnectionData(UserConData* pUsrConData) { this->m_pUserConData = pUsrConData; }

signals:

public slots:

private:
    GlobalData*  m_pGlobalData;
    UserConData* m_pUserConData;
};

#endif // DATACONNECTION_H
