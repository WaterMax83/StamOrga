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

#include "ccontcpmainsocket.h"
#include "../../Common/General/globalfunctions.h"
#include "../../Common/General/globaltiming.h"
#include "../../Common/Network/messagecommand.h"
#include "ccontcpmaindata.h"


cConTcpMainSocket::cConTcpMainSocket()
    : BackgroundWorker()
{
}

qint32 cConTcpMainSocket::initialize(QTcpSocket* socket)
{
    this->m_pTcpMasterSocket = socket;
    this->m_pTcpMasterSocket->setParent(this);

    this->SetWorkerName(QString("TCPMastSock%1").arg(socket->peerAddress().toString()));

    this->m_initialized = true;

    return ERROR_CODE_SUCCESS;
}


int cConTcpMainSocket::DoBackgroundWork()
{
    typedef void (QAbstractSocket::*QAbstractSocketErrorSignal)(QAbstractSocket::SocketError);
    connect(this->m_pTcpMasterSocket, static_cast<QAbstractSocketErrorSignal>(&QAbstractSocket::error), this, &cConTcpMainSocket::slotMainSocketError);
    connect(this->m_pTcpMasterSocket, &QTcpSocket::readyRead, this, &cConTcpMainSocket::readyReadMasterPort);

    this->m_pConTimeout = new QTimer(this);
    this->m_pConTimeout->setSingleShot(true);
    connect(this->m_pConTimeout, &QTimer::timeout, this, &cConTcpMainSocket::slotConnectionTimeoutFired);
    this->m_pConTimeout->start(SOCKET_TIMEOUT_MS);

    qInfo().noquote() << QString("Started Master TCP Socket for %1").arg(this->m_pTcpMasterSocket->peerAddress().toString());

    return ERROR_CODE_SUCCESS;
}


void cConTcpMainSocket::slotMainSocketError(QAbstractSocket::SocketError socketError)
{
    Q_UNUSED(socketError);

    emit this->signalSocketClosed(this->m_pTcpMasterSocket->peerPort());
}

void cConTcpMainSocket::slotConnectionTimeoutFired()
{
    emit this->signalSocketClosed(this->m_pTcpMasterSocket->peerPort());
    this->m_pTcpMasterSocket->close();
}

void cConTcpMainSocket::readyReadMasterPort()
{
    QByteArray datagram = this->m_pTcpMasterSocket->readAll();
    this->m_msgBuffer.StoreNewData(datagram);

    this->checkNewOncomingData();
}

void cConTcpMainSocket::checkNewOncomingData()
{
    MessageProtocol* msg;

    while ((msg = this->m_msgBuffer.GetNextMessage()) != NULL) {

        if (msg->getIndex() == OP_CODE_CMD_REQ::REQ_CONNECT_USER) {

            this->m_pConTimeout->start();
            /* Get userName from packet */
            QString          userName(QByteArray(msg->getPointerToData(), msg->getDataLength()));
            MessageProtocol* ack = g_ConTcpMainData.getNewUserAcknowledge(userName, this->m_pTcpMasterSocket->peerAddress());

            /* send answer */
            const char* pData = ack->getNetworkProtocol();
            this->m_pTcpMasterSocket->write(pData, ack->getNetworkSize());

            delete ack;
        }

        delete msg;
    }
}

qint32 cConTcpMainSocket::terminate()
{
    if (this->m_pTcpMasterSocket != NULL)
        this->m_pTcpMasterSocket->deleteLater();
    this->m_pTcpMasterSocket = NULL;

    if (this->m_pConTimeout != NULL)
        this->m_pConTimeout->deleteLater();
    this->m_pConTimeout = NULL;

    return ERROR_CODE_SUCCESS;
}

cConTcpMainSocket::~cConTcpMainSocket()
{
}
