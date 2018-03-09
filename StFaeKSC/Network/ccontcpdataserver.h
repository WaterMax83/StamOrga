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

#ifndef CCONTCPDATASERVER_H
#define CCONTCPDATASERVER_H

#include <QtCore/QList>
#include <QtCore/QObject>
#include <QtNetwork/QTcpServer>

#include "../Common/General/backgroundworker.h"
#include "ccontcpmainsocket.h"
#include "connectiondata.h"


class cConTcpDataServer : public BackgroundWorker
{
    Q_OBJECT
public:
    cConTcpDataServer();
    ~cConTcpDataServer();

    qint32 initialize(UserConData* pData);

    qint32 terminate();

signals:
    void signalServerClosed(quint16 destPort);

protected:
    int DoBackgroundWork() override;

private slots:
    void slotSocketConnected();
    void slotConnectionTimeoutFired();
    void slotDataSocketError(QAbstractSocket::SocketError socketError);

private:
    QTcpServer*  m_pTcpDataServer = NULL;
    UserConData* m_pUserConData   = NULL;
    QTimer*      m_pConTimeout    = NULL;
    QTcpSocket*  m_pTcpDataSocket = NULL;
};

#endif // CCONTCPDATASERVER_H
