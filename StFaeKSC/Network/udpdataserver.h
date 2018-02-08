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

#ifndef UDPDATASERVER_H
#define UDPDATASERVER_H

#include <QtCore/QString>
#include <QtCore/QTimer>
#include <QtNetwork/QUdpSocket>

#include "../Common/General/backgroundworker.h"
#include "../Common/Network/messagebuffer.h"
#include "../General/dataconnection.h"
#include "../General/globaldata.h"
#include "connectiondata.h"

class UdpDataServer : public BackgroundWorker
{
    Q_OBJECT
public:
    UdpDataServer(UserConData* pUsrConData, GlobalData* pGlobalData);
    ~UdpDataServer();

protected:
    int DoBackgroundWork() override;

signals:
    void notifyConnectionTimedOut(quint16 port);

private slots:
    void readyReadSocketPort();
    void onConnectionResetTimeout();
    void onConnectionLoginTimeout();

private:
    GlobalData*     m_pGlobalData;
    UserConData*    m_pUsrConData;
    DataConnection* m_pDataConnection = NULL;

    QUdpSocket*   m_pUdpSocket = NULL;
    MessageBuffer m_msgBuffer;

    QTimer* m_pConLoginTimer = NULL;
    QTimer* m_pConResetTimer = NULL;

    void checkNewOncomingData();

    MessageProtocol* checkNewMessage(MessageProtocol* msg);
};

#endif // UDPDATASERVER_H
