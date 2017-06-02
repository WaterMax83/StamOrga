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

#include "udpdataserver.h"
#include "../Common/General/globalfunctions.h"
#include "../Common/General/globaltiming.h"
#include "../Common/Network/messagecommand.h"
#include "../Common/Network/messageprotocol.h"


UdpDataServer::UdpDataServer(UserConData* pUsrConData, GlobalData* pGlobalData)
    : BackgroundWorker()
{
    this->SetWorkerName(QString("UDP Data Server %1").arg(pUsrConData->dstDataPort));

    this->m_pUsrConData = pUsrConData;
    this->m_pGlobalData = pGlobalData;
}


int UdpDataServer::DoBackgroundWork()
{
    this->m_pUdpSocket = new QUdpSocket();
    if (!this->m_pUdpSocket->bind(QHostAddress::Any, this->m_pUsrConData->dstDataPort)) {
        qDebug() << QString("Error binding socket  for port %1: %2\n").arg(this->m_pUsrConData->dstDataPort).arg(this->m_pUdpSocket->errorString());
        return -1;
    }
    connect(this->m_pUdpSocket, &QUdpSocket::readyRead, this, &UdpDataServer::readyReadSocketPort);

    this->m_pConLoginTimer = new QTimer();
    this->m_pConLoginTimer->setSingleShot(true);
    this->m_pConLoginTimer->setInterval(CON_LOGIN_TIMEOUT_MSEC);
    connect(this->m_pConLoginTimer, &QTimer::timeout, this, &UdpDataServer::onConnectionLoginTimeout);

    this->m_pConResetTimer = new QTimer();
    this->m_pConResetTimer->setSingleShot(true);
    this->m_pConResetTimer->setInterval(CON_RESET_TIMEOUT_MSEC);
    connect(this->m_pConResetTimer, &QTimer::timeout, this, &UdpDataServer::onConnectionResetTimeout);
    this->m_pConResetTimer->start();

    this->m_pDataConnection = new DataConnection(this->m_pGlobalData);
    this->m_pDataConnection->setUserConnectionData(this->m_pUsrConData);

    return 0;
}

void UdpDataServer::readyReadSocketPort()
{
    this->m_pConResetTimer->start();


    while (this->m_pUdpSocket->hasPendingDatagrams()) {
        QHostAddress sender;
        quint16      port;
        QByteArray   datagram;
        datagram.resize(this->m_pUdpSocket->pendingDatagramSize());

        if (this->m_pUdpSocket->readDatagram(datagram.data(), datagram.size(), &sender, &port)) {
            if (sender.toIPv4Address() == this->m_pUsrConData->sender.toIPv4Address()) {
                this->m_msgBuffer.StoreNewData(datagram);
                this->m_pUsrConData->srcDataPort = port;
            }
        }
    }

    this->checkNewOncomingData();
}

void UdpDataServer::onConnectionLoginTimeout()
{
    this->m_pUsrConData->bIsConnected = false;
    qDebug().noquote() << QString("User %1 with Port %2 was inactive, logged out")
                              .arg(this->m_pUsrConData->userName)
                              .arg(this->m_pUsrConData->dstDataPort);
}

void UdpDataServer::onConnectionResetTimeout()
{
    emit this->notifyConnectionTimedOut(this->m_pUsrConData->dstDataPort);
}

void UdpDataServer::checkNewOncomingData()
{
    MessageProtocol* msg;
    while ((msg = this->m_msgBuffer.GetNextMessage()) != NULL) {

        MessageProtocol* ack = checkNewMessage(msg);

        if (ack != NULL) {

            const char* pData = ack->getNetworkProtocol();
            this->m_pUdpSocket->writeDatagram(pData, ack->getNetworkSize(),
                                              this->m_pUsrConData->sender,
                                              this->m_pUsrConData->srcDataPort);
            delete ack;
        }
        delete msg;
    }
}

MessageProtocol* UdpDataServer::checkNewMessage(MessageProtocol* msg)
{
    MessageProtocol* ack = NULL;

    if (this->m_pUsrConData->bIsConnected) {

        switch (msg->getIndex()) {
        case OP_CODE_CMD_REQ::REQ_LOGIN_USER:
            ack = this->m_pDataConnection->requestCheckUserLogin(msg);
            break;

        case OP_CODE_CMD_REQ::REQ_GET_USER_PROPS:
            ack = this->m_pDataConnection->requestGetUserProperties();
            break;

        case OP_CODE_CMD_REQ::REQ_USER_CHANGE_LOGIN:
            ack = this->m_pDataConnection->requestUserChangeLogin(msg);
            break;

        case OP_CODE_CMD_REQ::REQ_USER_CHANGE_READNAME:
            ack = this->m_pDataConnection->requestUserChangeReadname(msg);
            break;

        case OP_CODE_CMD_REQ::REQ_GET_VERSION:
            ack = this->m_pDataConnection->requestGetProgramVersion(msg);
            break;

        case OP_CODE_CMD_REQ::REQ_GET_GAMES_LIST:
            ack = this->m_pDataConnection->requestGetGamesList(msg);
            break;

        case OP_CODE_CMD_REQ::REQ_GET_TICKETS_LIST:
            ack = this->m_pDataConnection->requestGetTicketsList();
            break;

        case OP_CODE_CMD_REQ::REQ_ADD_TICKET:
            ack = this->m_pDataConnection->requestAddSeasonTicket(msg);
            break;

        case OP_CODE_CMD_REQ::REQ_REMOVE_TICKET:
            ack = this->m_pDataConnection->requestRemoveSeasonTicket(msg);
            break;

        case OP_CODE_CMD_REQ::REQ_NEW_TICKET_PLACE:
            ack = this->m_pDataConnection->requestNewPlaceSeasonTicket(msg);
            break;

        case OP_CODE_CMD_REQ::REQ_STATE_CHANGE_SEASON_TICKET:
            ack = this->m_pDataConnection->requestChangeStateSeasonTicket(msg);
            break;

        case OP_CODE_CMD_REQ::REQ_GET_AVAILABLE_TICKETS:
            ack = this->m_pDataConnection->requestGetAvailableTicketList(msg);
            break;

        case OP_CODE_CMD_REQ::REQ_CHANGE_GAME:
            ack = this->m_pDataConnection->requestChangeGame(msg);
            break;

        case OP_CODE_CMD_REQ::REQ_CHANGE_MEETING_INFO:
            ack = this->m_pDataConnection->requestChangeMeetingInfo(msg);
            break;

        default:
            qWarning().noquote() << QString("Unkown command 0x%1").arg(QString::number(msg->getIndex()));
            break;
        }
        if (ack != NULL && this->m_pUsrConData->bIsConnected)
            this->m_pConLoginTimer->start(); // reset Timer
    } else if (msg->getIndex() == OP_CODE_CMD_REQ::REQ_LOGIN_USER) {

        ack = this->m_pDataConnection->requestCheckUserLogin(msg);
        if (this->m_pUsrConData->bIsConnected)
            this->m_pConLoginTimer->start(); // start Timer
    } else
        ack = new MessageProtocol(OP_CODE_CMD_RES::ACK_NOT_LOGGED_IN);

    return ack;
    ;
}

UdpDataServer::~UdpDataServer()
{
    if (this->m_pDataConnection != NULL)
        delete this->m_pDataConnection;

    if (this->m_pConLoginTimer != NULL)
        delete this->m_pConLoginTimer;

    if (this->m_pConResetTimer != NULL)
        delete this->m_pConResetTimer;

    if (this->m_pUdpSocket != NULL)
        delete this->m_pUdpSocket;
}
