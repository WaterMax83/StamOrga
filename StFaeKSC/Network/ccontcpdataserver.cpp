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
#include "../Manager/cgamesmanager.h"
#include "../Manager/cmeetinginfomanager.h"
#include "../Manager/cnewsdatamanager.h"
#include "../Manager/cstatisticmanager.h"
#include "../Manager/cticketmanager.h"
#include "ccontcpmaindata.h"

cConTcpDataServer::cConTcpDataServer()
    : BackgroundWorker()
{
    //    this->SetWorkerName("TCPDataServer");
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

    QMutexLocker lock(&this->m_mutex);

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
    QMutexLocker lock(&this->m_mutex);

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

        //        QString tmp = QString("Connected %1").arg(this->m_pUserConData->m_dstDataPort);
        //        CONSOLE_INFO(tmp);

        this->m_pTcpDataSocket = socket;
        typedef void (QAbstractSocket::*QAbstractSocketErrorSignal)(QAbstractSocket::SocketError);
        connect(this->m_pTcpDataSocket, static_cast<QAbstractSocketErrorSignal>(&QAbstractSocket::error), this, &cConTcpDataServer::slotDataSocketError);
        connect(this->m_pTcpDataSocket, &QTcpSocket::readyRead, this, &cConTcpDataServer::readyReadDataPort);
    }
}

void cConTcpDataServer::slotConnectionTimeoutFired()
{
    QMutexLocker lock(&this->m_mutex);

    this->m_pTcpDataServer->close();

    if (this->m_pTcpDataSocket != NULL)
        delete this->m_pTcpDataSocket;
    this->m_pTcpDataSocket = NULL;

    //    QString tmp = QString("Timeout %1").arg(this->m_pUserConData->m_dstDataPort);
    //    CONSOLE_INFO(tmp);

    emit this->signalServerClosed(this->m_pUserConData->m_dstDataPort);
}

void cConTcpDataServer::slotDataSocketError(QAbstractSocket::SocketError socketError)
{
    QMutexLocker lock(&this->m_mutex);

    Q_UNUSED(socketError);

    //    QString tmp = QString("Error %1").arg(this->m_pUserConData->m_dstDataPort);
    //    CONSOLE_INFO(tmp);

    emit this->signalServerClosed(this->m_pUserConData->m_dstDataPort);
}

void cConTcpDataServer::readyReadDataPort()
{
    QMutexLocker lock(&this->m_mutex);

    if (this->m_pTcpDataSocket == NULL)
        return;

    //    QString tmp = QString("Data %1").arg(this->m_pUserConData->m_dstDataPort);
    //    CONSOLE_INFO(tmp);

    QByteArray datagram = this->m_pTcpDataSocket->readAll();
    this->m_msgBuffer.StoreNewData(datagram);

    this->checkNewOncomingData();
}

void cConTcpDataServer::checkNewOncomingData()
{
    MessageProtocol* msg;

    while ((msg = this->m_msgBuffer.GetNextMessage()) != NULL) {

        MessageProtocol* ack = checkNewMessage(msg);

        if (ack != NULL) {

            quint32     sendBytes       = 0;
            quint32     totalPacketSize = ack->getNetworkSize();
            const char* pData           = ack->getNetworkProtocol();

            do {
                quint32 currentSendSize;
                if ((totalPacketSize - sendBytes) > MAX_DATAGRAMM_SIZE)
                    currentSendSize = MAX_DATAGRAMM_SIZE;
                else
                    currentSendSize = totalPacketSize - sendBytes;

                qint64 rValue = this->m_pTcpDataSocket->write(pData + sendBytes, currentSendSize);
                if (rValue < 0)
                    break;
                sendBytes += rValue;
                //                QThread::msleep(25);
            } while (sendBytes < totalPacketSize);

            delete ack;
        }
        delete msg;
    }
}

MessageProtocol* cConTcpDataServer::checkNewMessage(MessageProtocol* msg)
{
    MessageProtocol* ack = NULL;

    if (this->m_pUserConData->m_bIsConnected && msg->getIndex() != OP_CODE_CMD_REQ::REQ_LOGIN_USER) {
        //        ack = new MessageProtocol(OP_CODE_CMD_RES::ACK_NOT_LOGGED_IN);
        switch (msg->getIndex()) {
        case OP_CODE_CMD_REQ::REQ_GET_VERSION:
            ack = g_ConTcpMainData.getUserCheckVersion(this->m_pUserConData, msg);
            break;
        case OP_CODE_CMD_REQ::REQ_GET_USER_PROPS:
            ack = g_ConTcpMainData.getUserProperties(this->m_pUserConData, msg);
            break;
        case OP_CODE_CMD_REQ::REQ_USER_CHANGE_READNAME:
            ack = g_ConTcpMainData.getUserChangeReadableName(this->m_pUserConData, msg);
            break;
        case OP_CODE_CMD_REQ::REQ_USER_CHANGE_LOGIN:
            ack = g_ConTcpMainData.getUserChangePassword(this->m_pUserConData, msg);
            break;
        case OP_CODE_CMD_REQ::REQ_SET_USER_EVENTS:
            ack = g_ConTcpMainData.getSetUserEvent(this->m_pUserConData, msg);
            break;

        case OP_CODE_CMD_REQ::REQ_GET_TICKETS_LIST:
            ack = g_TicketManager.getSeasonTicketList(this->m_pUserConData, msg);
            break;
        case OP_CODE_CMD_REQ::REQ_ADD_TICKET:
            ack = g_TicketManager.getSeasonTicketAddRequest(this->m_pUserConData, msg);
            break;
        case OP_CODE_CMD_REQ::REQ_REMOVE_TICKET:
            ack = g_TicketManager.getSeasonTicketRemoveRequest(this->m_pUserConData, msg);
            break;

        case OP_CODE_CMD_REQ::REQ_GET_AVAILABLE_TICKETS:
            ack = g_TicketManager.getAvailableSeasonTicketList(this->m_pUserConData, msg);
            break;
        case OP_CODE_CMD_REQ::REQ_STATE_CHANGE_SEASON_TICKET:
            ack = g_TicketManager.getChangeAvailableTicketState(this->m_pUserConData, msg);
            break;

        case OP_CODE_CMD_REQ::REQ_GET_GAMES_LIST:
            ack = g_GamesManager.getGamesList(this->m_pUserConData, msg);
            break;
        case OP_CODE_CMD_REQ::REQ_GET_GAMES_INFO_LIST:
            ack = g_GamesManager.getGamesInfoList(this->m_pUserConData, msg);
            break;
        case OP_CODE_CMD_REQ::REQ_CHANGE_GAME:
            ack = g_GamesManager.getChangeGameRequest(this->m_pUserConData, msg);
            break;

        case OP_CODE_CMD_REQ::REQ_GET_NEWS_DATA_LIST:
            ack = g_NewsDataManager.getNewsDataList(this->m_pUserConData, msg);
            break;
        case OP_CODE_CMD_REQ::REQ_GET_NEWS_DATA_ITEM:
            ack = g_NewsDataManager.getNewsDataItem(this->m_pUserConData, msg);
            break;
        case OP_CODE_CMD_REQ::REQ_CHANGE_NEWS_DATA:
            ack = g_NewsDataManager.getNewsDataChangeRequest(this->m_pUserConData, msg);
            break;
        case OP_CODE_CMD_REQ::REQ_DEL_NEWS_DATA_ITEM:
            ack = g_NewsDataManager.getNewsDataRemoveRequest(this->m_pUserConData, msg);
            break;

        case OP_CODE_CMD_REQ::REQ_GET_MEETING_INFO:
        case OP_CODE_CMD_REQ::REQ_GET_AWAYTRIP_INFO:
            ack = g_MeetingInfoManager.getMeetingInfo(this->m_pUserConData, msg);
            break;
        case OP_CODE_CMD_REQ::REQ_CHANGE_MEETING_INFO:
        case OP_CODE_CMD_REQ::REQ_CHANGE_AWAYTRIP_INFO:
            ack = g_MeetingInfoManager.getChangeMeetingInfo(this->m_pUserConData, msg);
            break;
        case OP_CODE_CMD_REQ::REQ_ACCEPT_MEETING:
        case OP_CODE_CMD_REQ::REQ_ACCEPT_AWAYTRIP:
            ack = g_MeetingInfoManager.getAcceptMeetingInfo(this->m_pUserConData, msg);
            break;

        case OP_CODE_CMD_REQ::REQ_CMD_STATISTIC:
            ack = g_StatisticManager.handleStatisticCommand(this->m_pUserConData, msg);
            break;
        }
    } else if (msg->getIndex() == OP_CODE_CMD_REQ::REQ_LOGIN_USER) {
        ack = g_ConTcpMainData.getUserCheckLogin(this->m_pUserConData, msg);
    } else
        ack = new MessageProtocol(OP_CODE_CMD_RES::ACK_NOT_LOGGED_IN);
    return ack;
}

qint32 cConTcpDataServer::terminate()
{
    if (!this->m_initialized)
        return ERROR_CODE_NOT_INITIALIZED;

    QMutexLocker lock(&this->m_mutex);

    if (this->m_pTcpDataServer != NULL)
        this->m_pTcpDataServer->deleteLater();
    this->m_pTcpDataServer = NULL;

    if (this->m_pConTimeout != NULL)
        this->m_pConTimeout->deleteLater();
    this->m_pConTimeout = NULL;

    if (this->m_pConTimeout != NULL)
        this->m_pConTimeout->deleteLater();
    this->m_pConTimeout = NULL;

    if (this->m_pTcpDataSocket != NULL)
        this->m_pTcpDataSocket->deleteLater();
    this->m_pTcpDataSocket = NULL;

    this->m_initialized = false;

    return ERROR_CODE_SUCCESS;
}

cConTcpDataServer::~cConTcpDataServer()
{
}
