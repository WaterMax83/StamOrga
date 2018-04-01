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

#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>


#include "../Common/General/globalfunctions.h"
#include "../Common/General/globaltiming.h"
#include "../Common/Network/messagecommand.h"
#include "../Data/cdatagamesmanager.h"
#include "../Data/cdatanewsdatamanager.h"
#include "../Data/cdatastatisticmanager.h"
#include "../Data/cdataticketmanager.h"
#include "../cstaglobalsettings.h"
#include "cconmanager.h"
#include "ccontcpdata.h"
#include "cconusersettings.h"

cConTcpData::cConTcpData()
    : BackgroundWorker()
{
    this->SetWorkerName("cConTcpData");
    this->m_bRequestLoginAgain = false;
}

qint32 cConTcpData::initialize(QString host, qint32 port)
{
    this->m_hMasterReceiver = QHostAddress(host);
    if (m_hMasterReceiver.isNull())
        return ERROR_CODE_WRONG_PARAMETER;

    this->m_dataPort = port;

    return ERROR_CODE_SUCCESS;
}

int cConTcpData::DoBackgroundWork()
{
    /* Have to do it here or it will not be in the correct thread */
    this->m_pConTimeout = new QTimer(this);
    this->m_pConTimeout->setSingleShot(true);
    this->m_pConTimeout->setInterval(SOCKET_TIMEOUT_MS);
    connect(this->m_pConTimeout, &QTimer::timeout, this, &cConTcpData::slotConnectionTimeoutFired);

    //    this->m_pDataHandle = new DataHandling(this->m_pGlobalData);

    this->m_pDataTcpSocket = new QTcpSocket(this);
    if (!this->m_pDataTcpSocket->bind()) {
        qCritical().noquote() << QString("Could not bind socket for dataconnection");
        return -1;
    }
    connect(this->m_pDataTcpSocket, &QTcpSocket::connected, this, &cConTcpData::slotDataSocketConnected);
    connect(this->m_pDataTcpSocket, &QTcpSocket::readyRead, this, &cConTcpData::slotReadyReadDataPort);
    typedef void (QAbstractSocket::*QAbstractSocketErrorSignal)(QAbstractSocket::SocketError);
    connect(this->m_pDataTcpSocket, static_cast<QAbstractSocketErrorSignal>(&QAbstractSocket::error), this, &cConTcpData::slotSocketDataError);

    this->m_pDataTcpSocket->connectToHost(this->m_hMasterReceiver, this->m_dataPort);
    this->m_pConTimeout->start();

    return 0;
}

void cConTcpData::slotConnectionTimeoutFired()
{
    qInfo().noquote() << "cConTcpData: Timeout from Data UdpServer";
    while (this->m_lActualRequest.size() > 0) {
        TcpDataConRequest* request = this->m_lActualRequest.last();
        request->m_result          = ERROR_CODE_TIMEOUT;
        emit this->notifyLastRequestFinished(request);
        this->m_lActualRequest.removeLast();
    }
    this->m_bRequestLoginAgain = false;

    emit this->signalDataConnectionFinished(ERROR_CODE_TIMEOUT, "Timeout");
}

void cConTcpData::slotSocketDataError(QAbstractSocket::SocketError socketError)
{
    this->m_pConTimeout->stop();
    qWarning().noquote() << QString("Socket Error Data %1 - %2 ").arg(socketError).arg(this->m_pDataTcpSocket->errorString());
    while (this->m_lActualRequest.size() > 0) {
        TcpDataConRequest* request = this->m_lActualRequest.last();
        request->m_result          = ERROR_CODE_NO_CONNECTION;
        emit this->notifyLastRequestFinished(request);
        this->m_lActualRequest.removeLast();
    }
    this->m_bRequestLoginAgain = false;

    emit this->signalDataConnectionFinished(ERROR_CODE_NO_CONNECTION, this->m_pDataTcpSocket->errorString());
}

void cConTcpData::slotDataSocketConnected()
{
    this->m_pConTimeout->stop();

    emit this->signalDataConnectionFinished(ERROR_CODE_SUCCESS, "");
}

/*
 * Function/slot to call when new data is ready from udp socket
 */
void cConTcpData::slotReadyReadDataPort()
{
    QByteArray datagram = this->m_pDataTcpSocket->readAll();
    this->m_messageBuffer.StoreNewData(datagram);

    this->checkNewOncomingData();
}

/*
 * Function to check which data was received
 */
void cConTcpData::checkNewOncomingData()
{
    MessageProtocol* msg;
    while ((msg = this->m_messageBuffer.GetNextMessage()) != NULL) {


        if (msg->getIndex() == OP_CODE_CMD_RES::ACK_NOT_LOGGED_IN) {
            this->m_pConTimeout->stop();
            //            emit this->signalDataConnectionFinished(ERROR_CODE_NOT_LOGGED_IN, "Not logged in");
            this->m_bRequestLoginAgain = true;
            g_ConManager.sendLoginRequest();
            return;
        }
        TcpDataConRequest* request = this->getActualRequest(msg->getIndex() & 0x00FFFFFF);
        if (request == NULL)
            continue;


        switch (msg->getIndex()) {
        case OP_CODE_CMD_RES::ACK_LOGIN_USER:
            request->m_result = msg->getIntData();
            if (this->m_bRequestLoginAgain) {
                this->sendActualRequestsAgain(request->m_result);

                this->m_bRequestLoginAgain = false;
                continue;
            }

            break;

        case OP_CODE_CMD_RES::ACK_GET_VERSION:
            request->m_result = g_StaGlobalSettings.handleVersionResponse(msg);
            break;

        case OP_CODE_CMD_RES::ACK_GET_USER_PROPS:
            request->m_result = g_ConUserSettings.handleUserPropsResponse(msg);
            break;

        //        case OP_CODE_CMD_RES::ACK_GET_USER_EVENTS:
        //            request.m_result = this->m_pDataHandle->getHandleUserEventsResponse(msg);
        //            break;

        //        case OP_CODE_CMD_RES::ACK_SET_USER_EVENTS:
        //            request.m_result = msg->getIntData();
        //            break;

        case OP_CODE_CMD_RES::ACK_USER_CHANGE_LOGIN:
            request->m_result = g_ConUserSettings.handleUpdatePasswordResponse(msg);
            break;

        case OP_CODE_CMD_RES::ACK_USER_CHANGE_READNAME:
            request->m_result = g_ConUserSettings.handleUpdateReadableNameResponse(msg);
            break;

        case OP_CODE_CMD_RES::ACK_GET_GAMES_LIST:
            request->m_result = g_DataGamesManager.handleListGamesResponse(msg);
            break;

        case OP_CODE_CMD_RES::ACK_GET_GAMES_INFO_LIST:
            request->m_result = g_DataGamesManager.handleListGamesInfoResponse(msg);
            break;

        case OP_CODE_CMD_RES::ACK_CHANGE_GAME:
            request->m_result = g_DataGamesManager.handleChangeGameResponse(msg);
            break;

        //        case OP_CODE_CMD_RES::ACK_SET_FIXED_GAME_TIME:
        //                    request.m_result = msg->getIntData();
        //            break;

        case OP_CODE_CMD_RES::ACK_ADD_TICKET:
            request->m_result = g_DataTicketManager.handleAddSeasonTicketResponse(msg);
            break;

        case OP_CODE_CMD_RES::ACK_REMOVE_TICKET:
            request->m_result = g_DataTicketManager.handleRemoveSeasonTicketResponse(msg);
            break;

        case OP_CODE_CMD_RES::ACK_GET_TICKETS_LIST:
            request->m_result = g_DataTicketManager.handleListSeasonTicketsResponse(msg);
            break;

        //        case OP_CODE_CMD_RES::ACK_STATE_CHANGE_SEASON_TICKET:
        //            request.m_result = this->m_pDataHandle->getHandleChangeTicketStateResponse(msg);
        //            break;

        //        case OP_CODE_CMD_RES::ACK_GET_AVAILABLE_TICKETS:
        //            request.m_result = this->m_pDataHandle->getHandleAvailableTicketListResponse(msg, request.m_lData.at(0).toUInt());
        //            break;

        //        case OP_CODE_CMD_RES::ACK_CHANGE_MEETING_INFO:
        //        case OP_CODE_CMD_RES::ACK_CHANGE_AWAYTRIP_INFO:
        //            request.m_result = this->m_pDataHandle->getHandleChangeMeetingResponse(msg);
        //            break;

        //        case OP_CODE_CMD_RES::ACK_GET_MEETING_INFO:
        //            request.m_result = this->m_pDataHandle->getHandleLoadMeetingInfo(msg, MEETING_TYPE_MEETING);
        //            break;

        //        case OP_CODE_CMD_RES::ACK_GET_AWAYTRIP_INFO:
        //            request.m_result = this->m_pDataHandle->getHandleLoadMeetingInfo(msg, MEETING_TYPE_AWAYTRIP);
        //            break;

        //        case OP_CODE_CMD_RES::ACK_ACCEPT_MEETING:
        //        case OP_CODE_CMD_RES::ACK_ACCEPT_AWAYTRIP:
        //            request.m_result = this->m_pDataHandle->getHandleAcceptMeetingResponse(msg);
        //            break;

        case OP_CODE_CMD_RES::ACK_CHANGE_NEWS_DATA:
            request->m_result = g_DataNewsDataManager.handleChangeNewsDataResponse(msg);
            break;

        case OP_CODE_CMD_RES::ACK_GET_NEWS_DATA_LIST:
            request->m_result = g_DataNewsDataManager.handleListNewsDataResponse(msg);
            break;

        case OP_CODE_CMD_RES::ACK_GET_NEWS_DATA_ITEM:
            request->m_result = g_DataNewsDataManager.handleGetNewsDataItem(msg);
            break;

        case OP_CODE_CMD_RES::ACK_DEL_NEWS_DATA_ITEM:
            request->m_result = g_DataNewsDataManager.handleRemoveNewsDataItemResponse(msg);
            break;

        case OP_CODE_CMD_RES::ACK_CMD_STATISTIC:
            request->m_result = g_DataStatisticManager.handleStatisticResponse(msg);
            break;


        default:
            delete msg;
            continue;
        }
        emit this->notifyLastRequestFinished(request);
        this->removeActualRequest(request);
        if (this->m_lActualRequest.size() == 0)
            this->m_pConTimeout->stop();
        delete msg;
    }
}

//void cConTcpData::startSendGetUserEventsRequest(DataConRequest request)
//{
//    char data[8];
//    memset(&data[0], 0x0, 8);

//    MessageProtocol msg(request.m_request, &data[0], 8);
//    this->sendMessageRequest(&msg, request);
//}

//void cConTcpData::startSendSetUserEventsRequest(DataConRequest request)
//{
//    qint32 data[3];

//    qint64 eventID = qToLittleEndian(request.m_lData.at(0).toLongLong());
//    memcpy(&data[0], &eventID, sizeof(qint64));
//    data[2] = qToLittleEndian(request.m_lData.at(1).toInt()); // Status

//    MessageProtocol msg(request.m_request, (char*)&data[0], 12);
//    this->sendMessageRequest(&msg, request);
//}

//void cConTcpData::startSendChangeTicketState(DataConRequest request)
//{
//    QByteArray  seasonTicket;
//    QString     name = request.m_lData.at(3);
//    QDataStream wSeasonTicket(&seasonTicket, QIODevice::WriteOnly);
//    wSeasonTicket.setByteOrder(QDataStream::LittleEndian);
//    wSeasonTicket << request.m_lData.at(0).toUInt(); /* ticketIndex */
//    wSeasonTicket << request.m_lData.at(1).toUInt(); /* game Index */
//    wSeasonTicket << request.m_lData.at(2).toUInt(); /* state */
//    wSeasonTicket << quint16(name.toUtf8().size());
//    seasonTicket.append(name);

//    MessageProtocol msg(request.m_request, seasonTicket);
//    this->sendMessageRequest(&msg, request);
//}

//void cConTcpData::startSendChangeMeetingInfo(DataConRequest request)
//{
//    QByteArray  data;
//    QDataStream wData(&data, QIODevice::WriteOnly);
//    wData.setByteOrder(QDataStream::LittleEndian);
//    wData << request.m_lData.at(0).toUInt(); /* game Index */
//    data.append(request.m_lData.at(1));      /* when */
//    data.append(char(0x00));
//    data.append(request.m_lData.at(2)); /* where */
//    data.append(char(0x00));
//    data.append(request.m_lData.at(3)); /* info */
//    data.append(char(0x00));
//    wData.device()->seek(data.length());
//    wData << request.m_lData.at(4).toUInt();

//    MessageProtocol msg(request.m_request, data);
//    this->sendMessageRequest(&msg, request);
//}

//void cConTcpData::startSendGetMeetingInfo(DataConRequest request)
//{
//    quint32 data[2];
//    data[0] = qToLittleEndian(request.m_lData.at(0).toUInt()); /* game Index */
//    data[1] = qToLittleEndian(request.m_lData.at(1).toUInt()); /* type */

//    MessageProtocol msg(request.m_request, (char*)&data[0], 8);
//    this->sendMessageRequest(&msg, request);
//}


//#define BUFFER_SIZE 1000
//void cConTcpData::startSendAcceptMeeting(DataConRequest request)
//{
//    quint32 data[BUFFER_SIZE / sizeof(quint32)];
//    data[0] = qToLittleEndian(request.m_lData.at(0).toUInt()); /* game Index */
//    data[1] = qToLittleEndian(request.m_lData.at(1).toUInt()); /* accept */
//    data[2] = qToLittleEndian(request.m_lData.at(2).toUInt()); /* acceptIndex */

//    QByteArray tmp = request.m_lData.at(3).toUtf8();
//    if (tmp.size() > (BUFFER_SIZE - 12))
//        tmp.resize(BUFFER_SIZE - 12);
//    char* pName = (char*)&data[3];
//    memcpy(pName, tmp.constData(), tmp.size());
//    pName[tmp.size()] = 0x00;

//    MessageProtocol msg(request.m_request, (char*)(&data[0]), 16 + tmp.size());
//    this->sendMessageRequest(&msg, request);
//}

qint32 cConTcpData::sendMessageRequest(MessageProtocol* msg, TcpDataConRequest* request)
{
    if (this->m_pDataTcpSocket == NULL)
        return -1;

    quint32     sendBytes       = 0;
    quint32     totalPacketSize = msg->getNetworkSize();
    const char* pData           = msg->getNetworkProtocol();

    do {
        quint32 currentSendSize;
        if ((totalPacketSize - sendBytes) > MAX_DATAGRAMM_SIZE)
            currentSendSize = MAX_DATAGRAMM_SIZE;
        else
            currentSendSize = totalPacketSize - sendBytes;


        qint64 rValue = this->m_pDataTcpSocket->write(pData + sendBytes, currentSendSize);

        if (rValue < 0) {
            request->m_result = ERROR_CODE_ERR_SEND;
            emit this->notifyLastRequestFinished(request);

            return rValue;
        }
        sendBytes += rValue;
        //        //                QThread::msleep(25);
    } while (sendBytes < totalPacketSize);

    this->m_pConTimeout->start();

    //    /* Only add when not sending request again */
    if (!this->m_bRequestLoginAgain) {
        this->m_lActualRequest.append(request);
    }

    return sendBytes;
}

void cConTcpData::removeActualRequest(TcpDataConRequest* request)
{
    for (int i = 0; i < this->m_lActualRequest.size(); i++) {
        if (this->m_lActualRequest[i] == request) {
            //            if (this->m_lActualRequest[i].m_lData != NULL)
            //                delete this->m_lActualRequest[i].m_lData;
            this->m_lActualRequest.removeAt(i);
        }
    }
}


TcpDataConRequest* cConTcpData::getActualRequest(quint32 req)
{
    for (int i = 0; i < this->m_lActualRequest.size(); i++) {
        if (this->m_lActualRequest[i]->m_request == req) {
            return this->m_lActualRequest[i];
        }
    }
    return NULL;
}

void cConTcpData::sendActualRequestsAgain(qint32 result)
{
    if (result == ERROR_CODE_SUCCESS) {
        qInfo().noquote() << "cConTcpData: Trying to send the requests again";
        for (int i = 0; i < this->m_lActualRequest.size(); i++) {
            this->startSendNewRequest(this->m_lActualRequest[i]);
        }
    } else {
        while (this->m_lActualRequest.size() > 0) {
            TcpDataConRequest* request = this->m_lActualRequest.last();
            request->m_result          = result;
            emit this->notifyLastRequestFinished(request);
            this->m_lActualRequest.removeLast();
        }
    }
}

void cConTcpData::startSendNewRequest(TcpDataConRequest* request)
{
    MessageProtocol msg(request->m_request, request->m_lData);
    this->sendMessageRequest(&msg, request);

    //    case OP_CODE_CMD_REQ::REQ_GET_USER_EVENTS:
    //        this->startSendGetUserEventsRequest(request);
    //        break;

    //    case OP_CODE_CMD_REQ::REQ_SET_USER_EVENTS:
    //        this->startSendSetUserEventsRequest(request);
    //        break;

    //    case OP_CODE_CMD_REQ::REQ_REMOVE_TICKET:
    //        if (request.m_lData.size() > 0)
    //            this->startSendRemoveSeasonTicket(request);
    //        break;

    //    case OP_CODE_CMD_REQ::REQ_STATE_CHANGE_SEASON_TICKET:
    //        this->startSendChangeTicketState(request);
    //        break;

    //    case OP_CODE_CMD_REQ::REQ_CHANGE_MEETING_INFO:
    //    case OP_CODE_CMD_REQ::REQ_CHANGE_AWAYTRIP_INFO:
    //        this->startSendChangeMeetingInfo(request);
    //        break;

    //    case OP_CODE_CMD_REQ::REQ_GET_MEETING_INFO:
    //    case OP_CODE_CMD_REQ::REQ_GET_AWAYTRIP_INFO:
    //        this->startSendGetMeetingInfo(request);
    //        break;

    //    case OP_CODE_CMD_REQ::REQ_ACCEPT_MEETING:
    //    case OP_CODE_CMD_REQ::REQ_ACCEPT_AWAYTRIP:
    //        this->startSendAcceptMeeting(request);
    //        break;

    //    default:
    //        return;
    //    }
}

qint32 cConTcpData::terminate()
{
    if (this->m_pConTimeout != NULL) {
        this->m_pConTimeout->deleteLater();
    }
    this->m_pConTimeout = NULL;

    if (this->m_pDataTcpSocket != NULL) {
        this->m_pDataTcpSocket->deleteLater();
    }
    this->m_pDataTcpSocket = NULL;

    return ERROR_CODE_SUCCESS;
}

cConTcpData::~cConTcpData()
{
}
