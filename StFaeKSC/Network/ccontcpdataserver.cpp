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

#include "ccontcpdataserver.h"
#include "../Common/General/globalfunctions.h"
#include "../Common/General/globaltiming.h"
#include "../Common/Network/messagecommand.h"
#include "ccontcpmaindata.h"

cConTcpDataServer::cConTcpDataServer()
    : BackgroundWorker()
{
    this->SetWorkerName("TCPDataServer");
}

qint32 cConTcpDataServer::initialize(UserConData* pData)
{
    this->m_pUserConData = pData;
    this->SetWorkerName(QString("TCPDataServer%1").arg(pData->m_sender.toString()));

    this->m_initialized = true;

    return ERROR_CODE_SUCCESS;
}


int cConTcpDataServer::DoBackgroundWork()
{
    if (!this->m_initialized)
        return ERROR_CODE_NOT_INITIALIZED;

    this->m_pTcpDataServer = new QTcpServer(this);
    if (!this->m_pTcpDataServer->listen(QHostAddress::Any, this->m_pUserConData->m_dstDataPort)) {
        qCritical() << QString("Error listening data server %1\n").arg(this->m_pTcpDataServer->errorString());
        return -1;
    }

    connect(this->m_pTcpDataServer, &QTcpServer::newConnection, this, &cConTcpDataServer::slotSocketConnected);

    this->m_pConTimeout = new QTimer(this);
    this->m_pConTimeout->setSingleShot(true);
    this->m_pConTimeout->setInterval(SOCKET_TIMEOUT_MS);
    connect(this->m_pConTimeout, &QTimer::timeout, this, &cConTcpDataServer::slotConnectionTimeoutFired);
    this->m_pConTimeout->start();

    return ERROR_CODE_SUCCESS;
}

void cConTcpDataServer::slotSocketConnected()
{
    while (this->m_pTcpDataServer->hasPendingConnections()) {
        QTcpSocket* socket = this->m_pTcpDataServer->nextPendingConnection();
        if (socket == NULL)
            continue;

        if (socket->peerAddress() != this->m_pUserConData->m_sender) {
            socket->close();
            delete socket;
            continue;
        }

        this->m_pConTimeout->stop();

        if (this->m_pTcpDataSocket != NULL) {
            socket->close();
            delete socket;
            continue;
        }

        this->m_pTcpDataSocket = socket;
        typedef void (QAbstractSocket::*QAbstractSocketErrorSignal)(QAbstractSocket::SocketError);
        connect(this->m_pTcpDataSocket, static_cast<QAbstractSocketErrorSignal>(&QAbstractSocket::error), this, &cConTcpDataServer::slotDataSocketError);

        //        UserMainConnection* pMain = new UserMainConnection();
        //        pMain->m_pMainSocket      = new cConTcpMainSocket();
        //        pMain->m_pctrlMainSocket  = new BackgroundController();
        //        pMain->m_remotePort       = socket->peerPort();
        //        pMain->m_pMainSocket->initialize(socket);
        //        this->connect(pMain->m_pMainSocket, &cConTcpMainSocket::signalSocketClosed, this, &cConTcpDataServer::slotSocketClosed);
        //        pMain->m_pctrlMainSocket->Start(pMain->m_pMainSocket, false);

        //        this->m_lUserMainCons.append(pMain);
    }
}

void cConTcpDataServer::slotConnectionTimeoutFired()
{
    emit this->signalServerClosed(this->m_pUserConData->m_dstDataPort);
    this->m_pTcpDataServer->close();
}

void cConTcpDataServer::slotDataSocketError(QAbstractSocket::SocketError socketError)
{
    Q_UNUSED(socketError);

    emit this->signalServerClosed(this->m_pUserConData->m_dstDataPort);
}


//void cConTcpDataServer::slotSocketClosed(quint16 remotePort)
//{
//    for (int i = 0; i < this->m_lUserMainCons.size(); i++) {
//        if (this->m_lUserMainCons[i]->m_remotePort == remotePort) {
//            //            qInfo().noquote() << QString("Connection timeout for user %1 with port %2").arg(this->m_lUserCons[i].userConData.m_userName).arg(port);
//            this->m_lUserMainCons[i]->m_pMainSocket->terminate();
//            this->m_lUserMainCons[i]->m_pctrlMainSocket->Stop();
//            delete this->m_lUserMainCons[i]->m_pctrlMainSocket;
//            this->m_lUserMainCons.removeAt(i);
//            return;
//        }
//    }
//}

qint32 cConTcpDataServer::terminate()
{
    if (!this->m_initialized)
        return ERROR_CODE_NOT_INITIALIZED;

    if (this->m_pTcpDataServer != NULL)
        this->m_pTcpDataServer->deleteLater();
    this->m_pTcpDataServer = NULL;

    if (this->m_pConTimeout != NULL)
        this->m_pConTimeout->deleteLater();
    this->m_pConTimeout = NULL;

    if (this->m_pConTimeout != NULL)
        this->m_pConTimeout->deleteLater();
    this->m_pConTimeout = NULL;

    this->m_initialized = false;

    return ERROR_CODE_SUCCESS;
}

cConTcpDataServer::~cConTcpDataServer()
{
}
