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

#include "ccontcpmainserver.h"
#include "../../Common/General/globalfunctions.h"
#include "../../Common/Network/messagecommand.h"
#include "ccontcpmaindata.h"

cConTcpMainServer::cConTcpMainServer()
    : BackgroundWorker()
{
    this->SetWorkerName("TCPMasterServer");
}

qint32 cConTcpMainServer::initialize()
{
    this->m_initialized = true;

    return ERROR_CODE_SUCCESS;
}


int cConTcpMainServer::DoBackgroundWork()
{
    this->m_pTcpMasterServer = new QTcpServer();
    if (!this->m_pTcpMasterServer->listen(QHostAddress::Any, TCP_PORT)) {
        qCritical() << QString("Error listening master server %1\n").arg(this->m_pTcpMasterServer->errorString());
        return -1;
    }

    connect(this->m_pTcpMasterServer, &QTcpServer::newConnection, this, &cConTcpMainServer::slotSocketConnected);

    qInfo().noquote() << "Started Master TCP Server";

    return ERROR_CODE_SUCCESS;
}

void cConTcpMainServer::slotSocketConnected()
{
    while (this->m_pTcpMasterServer->hasPendingConnections()) {
        QTcpSocket* socket = this->m_pTcpMasterServer->nextPendingConnection();
        if (socket == NULL)
            continue;

        UserMainConnection* pMain = new UserMainConnection();
        pMain->m_pMainSocket      = new cConTcpMainSocket();
        pMain->m_pctrlMainSocket  = new BackgroundController();
        pMain->m_remotePort       = socket->peerPort();
        pMain->m_pMainSocket->initialize(socket);
        this->connect(pMain->m_pMainSocket, &cConTcpMainSocket::signalSocketClosed, this, &cConTcpMainServer::slotSocketClosed);
        pMain->m_pctrlMainSocket->Start(pMain->m_pMainSocket, false);

        this->m_lUserMainCons.append(pMain);
    }
}


void cConTcpMainServer::slotSocketClosed(quint16 remotePort)
{
    for (int i = 0; i < this->m_lUserMainCons.size(); i++) {
        if (this->m_lUserMainCons[i]->m_remotePort == remotePort) {
            //            qInfo().noquote() << QString("Connection timeout for user %1 with port %2").arg(this->m_lUserCons[i].userConData.m_userName).arg(port);
            this->m_lUserMainCons[i]->m_pMainSocket->terminate();
            this->m_lUserMainCons[i]->m_pctrlMainSocket->Stop();
            delete this->m_lUserMainCons[i]->m_pctrlMainSocket;
            this->m_lUserMainCons.removeAt(i);
            return;
        }
    }
}

cConTcpMainServer::~cConTcpMainServer()
{
    if (this->m_pTcpMasterServer != NULL)
        delete this->m_pTcpMasterServer;
}
