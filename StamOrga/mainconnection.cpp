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

#include <QtCore/QByteArray>
#include <QtCore/QtEndian>
#include <QtNetwork/QUdpSocket>

#include "../Common/General/globalfunctions.h"
#include "../Common/General/globaltiming.h"
#include "../Common/Network/messagecommand.h"
#include "../Common/Network/messageprotocol.h"
#include "mainconnection.h"

MainConnection::MainConnection(GlobalData* pData)
    : BackgroundWorker()
{
    this->SetWorkerName("MainConnection");
    this->m_pGlobalData = pData;
}


int MainConnection::DoBackgroundWork()
{
    this->m_pMasterUdpSocket = new QUdpSocket();

    if (!this->m_pMasterUdpSocket->bind()) {
        emit this->connectionRequestFinished(0, this->m_pMasterUdpSocket->errorString());
        return -1;
    }

    connect(this->m_pMasterUdpSocket, &QUdpSocket::readyRead, this, &MainConnection::slotReadyReadMasterPort);
    typedef void (QAbstractSocket::*QAbstractSocketErrorSignal)(QAbstractSocket::SocketError);
    connect(this->m_pMasterUdpSocket, static_cast<QAbstractSocketErrorSignal>(&QAbstractSocket::error), this, &MainConnection::slotSocketMainError);

    this->m_hMasterReceiver = QHostAddress(this->m_pGlobalData->ipAddr());

    this->m_pConTimeout = new QTimer();
    this->m_pConTimeout->setSingleShot(true);
    connect(this->m_pConTimeout, &QTimer::timeout, this, &MainConnection::slotConnectionTimeoutFired);

    return 0;
}

void MainConnection::slotSendNewMainConRequest(QString username)
{
    QByteArray aData;
    aData.append(username);
    this->m_userName = username;

    MessageProtocol msg(OP_CODE_CMD_REQ::REQ_CONNECT_USER, aData);

    const char* pData = msg.getNetworkProtocol();
    this->m_pMasterUdpSocket->writeDatagram(pData, msg.getNetworkSize(), this->m_hMasterReceiver, this->m_pGlobalData->conMasterPort());

    this->m_pConTimeout->start(SOCKET_TIMEOUT_MS);
}


void MainConnection::slotSocketMainError(QAbstractSocket::SocketError socketError)
{
    qDebug().noquote() << QString("Socket Error %1 - %2 ").arg(socketError).arg(this->m_pMasterUdpSocket->errorString());
}

void MainConnection::slotConnectionTimeoutFired()
{
    emit this->connectionRequestFinished(ERROR_CODE_TIMEOUT, "Timeout");
    qDebug() << QString("Main Con Timeout %1").arg(this->m_pMasterUdpSocket->errorString());
}

void MainConnection::slotReadyReadMasterPort()
{
    while (this->m_pMasterUdpSocket->hasPendingDatagrams()) {
        QHostAddress sender;
        quint16      port;
        QByteArray   datagram;
        datagram.resize(this->m_pMasterUdpSocket->pendingDatagramSize());

        if (this->m_pMasterUdpSocket->readDatagram(datagram.data(), datagram.size(), &sender, &port)) {
            if (this->m_pGlobalData->conMasterPort() == port
                && this->m_hMasterReceiver.toIPv4Address() == sender.toIPv4Address()) {

                this->m_messageBuffer.StoreNewData(datagram);
            }
        }
    }
    this->checkNewOncomingData();
}

void MainConnection::checkNewOncomingData()
{
    MessageProtocol* msg;
    while ((msg = this->m_messageBuffer.GetNextMessage()) != NULL) {

        if (msg->getIndex() == OP_CODE_CMD_RES::ACK_CONNECT_USER) {
            this->m_pConTimeout->stop();
            if (msg->getDataLength() != 4)
                emit this->connectionRequestFinished(ERROR_CODE_WRONG_SIZE, QString("Datalength is wrong, expected 4").arg(msg->getDataLength()));
            else {
                qint32 rValue = msg->getIntData();
                if (rValue > ERROR_CODE_NO_ERROR) {
                    if (this->m_pGlobalData->userName() != this->m_userName) {
                        this->m_pGlobalData->setUserName(this->m_userName);
                        this->m_pGlobalData->saveGlobalUserSettings();
                    }
                    emit this->connectionRequestFinished(rValue, "");
                } else if (rValue == ERROR_CODE_NO_USER)
                    emit this->connectionRequestFinished(rValue, QString("Wrong user %1").arg(this->m_userName));
                else
                    emit this->connectionRequestFinished(rValue, QString("unkown error %1").arg(rValue));
            }
        }
        delete msg;
    }
}


MainConnection::~MainConnection()
{
    if (this->m_pMasterUdpSocket != NULL)
        delete this->m_pMasterUdpSocket;
}
