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

#ifndef CCONTCPMAINSOCKET_H
#define CCONTCPMAINSOCKET_H

//#include <QtCore/QList>
#include <QtCore/QObject>
#include <QtCore/QTimer>
#include <QtNetwork/QTcpSocket>

//#include "connectiondata.h"
//#include "udpdataserver.h"
//#include "General/globaldata.h"
//#include <../Common/General/backgroundcontroller.h>
#include <../Common/General/backgroundworker.h>
#include <../Common/Network/messagebuffer.h>


class cConTcpMainSocket : public BackgroundWorker
{
    Q_OBJECT
public:
    cConTcpMainSocket();
    ~cConTcpMainSocket();

    qint32 initialize(QTcpSocket* socket);

    qint32 terminate();

signals:
    void signalSocketClosed(quint16 remotePort);

protected:
    int DoBackgroundWork() override;

private slots:
    void readyReadMasterPort();
    void slotMainSocketError(QAbstractSocket::SocketError socketError);
    void slotConnectionTimeoutFired();


private:
    QTcpSocket*   m_pTcpMasterSocket = NULL;
    MessageBuffer m_msgBuffer;
    QTimer*       m_pConTimeout = NULL;

    void checkNewOncomingData();
};

#endif // CCONTCPMAINSOCKET_H
