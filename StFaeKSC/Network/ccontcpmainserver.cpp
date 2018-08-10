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

#include <QtNetwork/QSslSocket>

#include "../../Common/General/globalfunctions.h"
#include "../../Common/Network/messagecommand.h"
#include "ccontcpmaindata.h"
#include "ccontcpmainserver.h"

cConTcpMainServer::cConTcpMainServer()
    : BackgroundWorker()
{
    this->SetWorkerName("TCPMasterServer");
}

qint32 cConTcpMainServer::initialize()
{
    qInfo().noquote() << "SSH Version: " << QSslSocket::sslLibraryVersionString();

    this->m_initialized = true;

    return ERROR_CODE_SUCCESS;
}


int cConTcpMainServer::DoBackgroundWork()
{
    this->m_pTcpMasterServerNoSsl = new cConSslServer(cConSslUsage::NO_SSL);
    if (!this->m_pTcpMasterServerNoSsl->listen(QHostAddress::Any, TCP_PORT)) {
        qCritical() << QString("Error listening master server no ssl %1\n").arg(this->m_pTcpMasterServerNoSsl->errorString());
        return -1;
    }

    this->m_pTcpMasterServerSsl = new cConSslServer(cConSslUsage::USE_SSL);
    if (!this->m_pTcpMasterServerSsl->listen(QHostAddress::Any, TCP_PORT + 1)) {
        qCritical() << QString("Error listening master server ssl %1\n").arg(this->m_pTcpMasterServerSsl->errorString());
        return -1;
    }

    connect(this->m_pTcpMasterServerNoSsl, &QTcpServer::newConnection, this, &cConTcpMainServer::slotSocketConnected);
    connect(this->m_pTcpMasterServerSsl, &QTcpServer::newConnection, this, &cConTcpMainServer::slotSocketConnected);

    qInfo().noquote() << QString("Started Master TCP Server with ports %1, %2").arg(TCP_PORT).arg(TCP_PORT + 1);

    return ERROR_CODE_SUCCESS;
}


void cConTcpMainServer::slotSocketConnected()
{
    while (this->m_pTcpMasterServerNoSsl->hasPendingConnections()) {
        QTcpSocket* pSocket = this->m_pTcpMasterServerNoSsl->nextPendingConnection();
        if (pSocket == NULL)
            continue;

        this->createNewUserMainConnection(pSocket, this->m_pTcpMasterServerNoSsl->getSslUsage());
    }
    while (this->m_pTcpMasterServerSsl->hasPendingConnections()) {
        QSslSocket* pSocket = dynamic_cast<QSslSocket*>(this->m_pTcpMasterServerSsl->nextPendingConnection());
        if (pSocket == NULL)
            continue;

        connect(pSocket, SIGNAL(sslErrors(const QList<QSslError>&)), this, SLOT(slotSslErrors(const QList<QSslError>&)));

        this->createNewUserMainConnection(pSocket, this->m_pTcpMasterServerSsl->getSslUsage());
    }
}

void cConTcpMainServer::createNewUserMainConnection(QTcpSocket* pSocket, const cConSslUsage sslUsage)
{
    UserMainConnection* pMain = new UserMainConnection();
    pMain->m_pMainSocket      = new cConTcpMainSocket();
    pMain->m_pctrlMainSocket  = new BackgroundController();
    pMain->m_remotePort       = pSocket->peerPort();
    pMain->m_pMainSocket->initialize(pSocket, sslUsage);
    this->connect(pMain->m_pMainSocket, &cConTcpMainSocket::signalSocketClosed, this, &cConTcpMainServer::slotSocketClosed);
    pMain->m_pctrlMainSocket->Start(pMain->m_pMainSocket, false);

    this->m_lUserMainCons.append(pMain);
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

void cConTcpMainServer::slotSslErrors(const QList<QSslError>& errors)
{
    qInfo() << "SSL Main Server Errors " << errors;
}

cConTcpMainServer::~cConTcpMainServer()
{
    if (this->m_pTcpMasterServerNoSsl != NULL)
        delete this->m_pTcpMasterServerNoSsl;

    if (this->m_pTcpMasterServerSsl != NULL)
        delete this->m_pTcpMasterServerSsl;
}
