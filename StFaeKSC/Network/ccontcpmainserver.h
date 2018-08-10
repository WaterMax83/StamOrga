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

#include <QtCore/QList>
#include <QtCore/QObject>
#include <QtNetwork/QSslError>
#include <QtNetwork/QTcpServer>

#include "cconsslserver.h"
#include "ccontcpmainsocket.h"
#include <../Common/General/backgroundcontroller.h>
#include <../Common/General/backgroundworker.h>

struct UserMainConnection {
    quint16               m_remotePort;
    cConTcpMainSocket*    m_pMainSocket;
    BackgroundController* m_pctrlMainSocket;
};


class cConTcpMainServer : public BackgroundWorker
{
    Q_OBJECT
public:
    cConTcpMainServer();
    ~cConTcpMainServer();

    qint32 initialize();

protected:
    int DoBackgroundWork() override;

private slots:
    void slotSocketConnected();
    void slotSocketClosed(quint16 remotePort);
    void slotSslErrors(const QList<QSslError>& errors);

private:
    cConSslServer* m_pTcpMasterServerNoSsl = NULL;
    cConSslServer* m_pTcpMasterServerSsl   = NULL;

    QList<UserMainConnection*> m_lUserMainCons;
    void createNewUserMainConnection(QTcpSocket* pSocket, const cConSslUsage sslUsage);
};

#endif // CCONTCPMAIN_H
