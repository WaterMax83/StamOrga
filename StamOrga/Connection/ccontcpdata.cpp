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

//#include "../Common/General/config.h"
#include "../Common/General/globalfunctions.h"
#include "../Common/General/globaltiming.h"
//#include "../Common/Network/messagecommand.h"
//#include "../Common/Network/messageprotocol.h"
//#include "../Data/globalsettings.h"
#include "ccontcpdata.h"

cConTcpData::cConTcpData()
    : BackgroundWorker()
{
    this->SetWorkerName("cConTcpData");
    //    this->m_pGlobalData        = pData;
    //    this->m_bRequestLoginAgain = false;
    //#if QT_VERSION < QT_VERSION_CHECK(5, 9, 0)
    //    this->m_hash = new QCryptographicHash(QCryptographicHash::Sha3_512);
    //#else
    //    this->m_hash = new QCryptographicHash(QCryptographicHash::Keccak_512);
    //#endif
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
    //    qInfo().noquote() << "cConTcpData: Timeout from Data UdpServer";
    //    while (this->m_lActualRequest.size() > 0) {
    //        DataConRequest request = this->m_lActualRequest.last();
    //        request.m_result       = ERROR_CODE_TIMEOUT;
    //        emit this->notifyLastRequestFinished(request);
    //        this->m_lActualRequest.removeLast();
    //    }
    //    this->m_bRequestLoginAgain = false;

    qDebug() << "TimeOut";

    emit this->signalDataConnectionFinished(ERROR_CODE_TIMEOUT, "Timeout");
}

void cConTcpData::slotSocketDataError(QAbstractSocket::SocketError socketError)
{
    this->m_pConTimeout->stop();
    qCritical().noquote() << QString("Socket Error %1 - %2 ").arg(socketError).arg(this->m_pDataTcpSocket->errorString());

    emit this->signalDataConnectionFinished(ERROR_CODE_COMMON, this->m_pDataTcpSocket->errorString());
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
    //    while (this->m_pDataUdpSocket->hasPendingDatagrams()) {
    //        QHostAddress sender;
    //        quint16      port;
    //        QByteArray   datagram;
    //        datagram.resize(this->m_pDataUdpSocket->pendingDatagramSize());

    //        if (this->m_pDataUdpSocket->readDatagram(datagram.data(), datagram.size(), &sender, &port)) {
    //            if (this->m_pGlobalData->conDataPort() == port
    //                && this->m_hDataReceiver.toIPv4Address() == sender.toIPv4Address()) {

    //                this->m_messageBuffer.StoreNewData(datagram);
    //            }
    //        }
    //    }

    //    this->checkNewOncomingData();
}

/*
 * Function to check which data was received
 */
//void cConTcpData::checkNewOncomingData()
//{
//    MessageProtocol* msg;
//    while ((msg = this->m_messageBuffer.GetNextMessage()) != NULL) {

//        DataConRequest request = this->getActualRequest(msg->getIndex() & 0x00FFFFFF);
//        if (request.m_request == 0 && msg->getIndex() != OP_CODE_CMD_RES::ACK_NOT_LOGGED_IN)
//            continue;


//        switch (msg->getIndex()) {
//        case OP_CODE_CMD_RES::ACK_LOGIN_USER:
//            if (this->m_bRequestLoginAgain) {
//                this->sendActualRequestsAgain(this->m_pDataHandle->getHandleLoginResponse(msg));

//                this->m_bRequestLoginAgain = false;
//                continue;
//            } else {
//                request.m_result = this->m_pDataHandle->getHandleLoginResponse(msg);
//                if (request.m_result == ERROR_CODE_SUCCESS) {
//                    QString passWord = request.m_lData.at(0);
//                    QString salt     = request.m_lData.at(1);
//                    if (this->m_pGlobalData->getSalt() == "")
//                        passWord = this->createHashValue(passWord, salt);
//                    if (this->m_pGlobalData->passWord() != passWord || this->m_pGlobalData->getSalt() != salt) {
//                        this->m_pGlobalData->setPassWord(passWord);
//                        this->m_pGlobalData->setSalt(salt);
//                        this->m_pGlobalData->saveGlobalUserSettings();
//                    }
//                } else if (request.m_result == ERROR_CODE_WRONG_PASSWORD && this->m_pGlobalData->passWord() != "") {
//                    this->m_pGlobalData->setPassWord("");
//                    this->m_pGlobalData->setSalt("");
//                    this->m_pGlobalData->saveGlobalUserSettings();
//                }
//            }

//            break;

//        case OP_CODE_CMD_RES::ACK_GET_VERSION:
//            request.m_result = this->m_pDataHandle->getHandleVersionResponse(msg, &request.m_returnData);
//            break;

//        case OP_CODE_CMD_RES::ACK_GET_USER_PROPS:
//            request.m_result = this->m_pDataHandle->getHandleUserPropsResponse(msg);
//            break;

//        case OP_CODE_CMD_RES::ACK_GET_USER_EVENTS:
//            request.m_result = this->m_pDataHandle->getHandleUserEventsResponse(msg);
//            break;

//        case OP_CODE_CMD_RES::ACK_SET_USER_EVENTS:
//            request.m_result = msg->getIntData();
//            break;

//        case OP_CODE_CMD_RES::ACK_USER_CHANGE_LOGIN:
//            request.m_result = msg->getIntData();
//            if (request.m_lData.size() > 0 && request.m_result == ERROR_CODE_SUCCESS) {
//                request.m_returnData = this->createHashValue(request.m_lData.at(0), this->m_pGlobalData->getSalt());
//            }
//            break;

//        case OP_CODE_CMD_RES::ACK_USER_CHANGE_READNAME:
//            request.m_result = msg->getIntData();
//            if (request.m_lData.size() > 0)
//                request.m_returnData = request.m_lData.at(0);
//            break;

//        case OP_CODE_CMD_RES::ACK_NOT_LOGGED_IN:
//            if (!this->m_bRequestLoginAgain) {
//                DataConRequest conReq;
//                conReq.m_request = OP_CODE_CMD_REQ::REQ_LOGIN_USER;
//                conReq.m_lData.append(this->m_pGlobalData->passWord());
//                conReq.m_lData.append(this->m_pGlobalData->getSalt());
//                this->startSendLoginRequest(conReq);
//                this->m_bRequestLoginAgain = true;
//                delete msg;
//                continue;
//            }
//            break;

//        case OP_CODE_CMD_RES::ACK_GET_GAMES_LIST:
//            request.m_result = this->m_pDataHandle->getHandleGamesListResponse(msg);
//            break;

//        case OP_CODE_CMD_RES::ACK_GET_GAMES_INFO_LIST:
//            request.m_result = this->m_pDataHandle->getHandleGamesInfoListResponse(msg);
//            break;

//        case OP_CODE_CMD_RES::ACK_SET_FIXED_GAME_TIME:
//            request.m_result = msg->getIntData();
//            break;

//        case OP_CODE_CMD_RES::ACK_ADD_TICKET:
//            request.m_result = msg->getIntData();
//            break;

//        case OP_CODE_CMD_RES::ACK_REMOVE_TICKET:
//            request.m_result = msg->getIntData();
//            break;

//        case OP_CODE_CMD_RES::ACK_CHANGE_TICKET:
//            request.m_result = msg->getIntData();
//            break;

//        case OP_CODE_CMD_RES::ACK_GET_TICKETS_LIST:
//            request.m_result = this->m_pDataHandle->getHandleSeasonTicketListResponse(msg);
//            break;

//        case OP_CODE_CMD_RES::ACK_STATE_CHANGE_SEASON_TICKET:
//            request.m_result = this->m_pDataHandle->getHandleChangeTicketStateResponse(msg);
//            break;

//        case OP_CODE_CMD_RES::ACK_GET_AVAILABLE_TICKETS:
//            request.m_result = this->m_pDataHandle->getHandleAvailableTicketListResponse(msg, request.m_lData.at(0).toUInt());
//            break;

//        case OP_CODE_CMD_RES::ACK_CHANGE_GAME:
//            request.m_result = msg->getIntData();
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

//        case OP_CODE_CMD_RES::ACK_CHANGE_NEWS_DATA:
//            request.m_result = this->m_pDataHandle->getHandleFanclubNewsChangeResponse(msg, request.m_returnData);
//            break;

//        case OP_CODE_CMD_RES::ACK_GET_NEWS_DATA_LIST:
//            request.m_result = this->m_pDataHandle->getHandleFanclubNewsListResponse(msg);
//            break;

//        case OP_CODE_CMD_RES::ACK_GET_NEWS_DATA_ITEM:
//            request.m_result = this->m_pDataHandle->getHandleFanclubNewsItemResponse(msg);
//            break;

//        case OP_CODE_CMD_RES::ACK_DEL_NEWS_DATA_ITEM:
//            request.m_result = msg->getIntData();
//            break;

//        case OP_CODE_CMD_RES::ACK_CMD_STATISTIC:
//            request.m_result = this->m_pDataHandle->getHandleStatisticsCommandResponse(msg);
//            break;


//        default:
//            delete msg;
//            continue;
//        }
//        emit this->notifyLastRequestFinished(request);
//        this->removeActualRequest(request.m_request);
//        if (this->m_lActualRequest.size() == 0)
//            this->m_pConTimeout->stop();
//        delete msg;
//    }
//}

//void cConTcpData::startSendLoginRequest(DataConRequest request)
//{
//    QString passWord = request.m_lData.at(0);
//    QString salt     = request.m_lData.at(1);
//    if (this->m_pGlobalData->getSalt() == "")
//        passWord = this->createHashValue(passWord, salt);

//    passWord = this->createHashValue(passWord, this->m_randomLoginValue);
//    QByteArray  aPassw;
//    QDataStream wPassword(&aPassw, QIODevice::WriteOnly);
//    wPassword.setByteOrder(QDataStream::LittleEndian);
//    wPassword << quint16(passWord.toUtf8().size());
//    aPassw.append(passWord);
//    MessageProtocol msg(request.m_request, aPassw);
//    this->sendMessageRequest(&msg, request);
//}

//void cConTcpData::startSendVersionRequest(DataConRequest request)
//{
//    QByteArray  version;
//    QDataStream wVersion(&version, QIODevice::WriteOnly);
//    wVersion.setByteOrder(QDataStream::LittleEndian);
//    wVersion << (quint32)STAM_ORGA_VERSION_I;
//    QString sVersion = STAM_ORGA_VERSION_S;
//#ifdef Q_OS_WIN
//    sVersion.append("_Win");
//#elif defined(Q_OS_ANDROID)
//    sVersion.append("_Android");
//#elif defined(Q_OS_IOS)
//    sVersion.append(("_iOS");
//#endif
//    wVersion << quint16(sVersion.toUtf8().size());

//    version.append(sVersion);
//    MessageProtocol msg(request.m_request, version);
//    this->sendMessageRequest(&msg, request);
//}

//void cConTcpData::startSendUserPropsRequest(DataConRequest request)
//{
//    QJsonObject rootObj;
//    rootObj.insert("guid", this->m_pGlobalData->getCurrentAppGUID());
//    rootObj.insert("token", this->m_pGlobalData->getCurrentAppToken());
//#ifdef Q_OS_WIN
//    rootObj.insert("os", 1);
//#elif defined(Q_OS_ANDROID)
//    rootObj.insert("os", 2);
//#elif defined(Q_OS_IOS)
//    rootObj.insert("os", 3);
//#endif

//    QByteArray data = QJsonDocument(rootObj).toJson(QJsonDocument::Compact);

//    MessageProtocol msg(request.m_request, data);
//    this->sendMessageRequest(&msg, request);
//}

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

//void cConTcpData::startSendUpdPassRequest(DataConRequest request)
//{
//    QByteArray  passReq;
//    QDataStream wPassReq(&passReq, QIODevice::WriteOnly);
//    wPassReq.setByteOrder(QDataStream::LittleEndian);

//    QString newPassWord     = this->createHashValue(request.m_lData.at(0), this->m_pGlobalData->getSalt());
//    QString currentPassWord = this->createHashValue(this->m_pGlobalData->passWord(), this->m_randomLoginValue);

//    wPassReq << (qint16)currentPassWord.toUtf8().size();
//    passReq.append(currentPassWord.toUtf8());
//    wPassReq.device()->seek(passReq.length());

//    wPassReq << (qint16)newPassWord.toUtf8().size();
//    passReq.append(newPassWord.toUtf8());

//    MessageProtocol msg(request.m_request, passReq);
//    this->sendMessageRequest(&msg, request);
//}

//void cConTcpData::startSendReadableNameRequest(DataConRequest request)
//{
//    QString     name = request.m_lData.at(0);
//    QByteArray  readName;
//    QDataStream wReadName(&readName, QIODevice::WriteOnly);
//    wReadName.setByteOrder(QDataStream::LittleEndian);
//    wReadName << quint16(name.toUtf8().size());
//    readName.append(name);

//    MessageProtocol msg(request.m_request, readName);
//    this->sendMessageRequest(&msg, request);
//}

//void cConTcpData::startSendGamesListRequest(DataConRequest request)
//{
//    quint32 data[3];
//    qint64  timeStamp = this->m_pGlobalData->getGamePlayLastLocalUpdate();

//    if (timeStamp + TIMEOUT_UPDATE_GAMES > QDateTime::currentMSecsSinceEpoch() && this->m_pGlobalData->getGamePlayLength() > 0)
//        data[0] = qToLittleEndian(UpdateIndex::UpdateDiff);
//    else
//        data[0] = qToLittleEndian(UpdateIndex::UpdateAll);
//    timeStamp = qToLittleEndian(this->m_pGlobalData->getGamePlayLastServerUpdate());
//    memcpy(&data[1], &timeStamp, sizeof(qint64));

//    MessageProtocol msg(request.m_request, (char*)(&data[0]), sizeof(quint32) * 3);
//    this->sendMessageRequest(&msg, request);
//}

//void cConTcpData::startSendGamesInfoListRequest(DataConRequest request)
//{
//    quint32 data[2];
//    qint64  timeStamp = qToLittleEndian(this->m_pGlobalData->getGamePlayLastServerUpdate());
//    memcpy(&data[0], &timeStamp, sizeof(qint64));
//    MessageProtocol msg(request.m_request, (char*)&data[0], 8);
//    this->sendMessageRequest(&msg, request);
//}

//void cConTcpData::startSendSetGameTimeFixedRequest(DataConRequest request)
//{
//    quint32 data[2];
//    data[0] = request.m_lData.at(0).toUInt();
//    data[1] = request.m_lData.at(1).toUInt();
//    MessageProtocol msg(request.m_request, (char*)&data[0], 8);
//    this->sendMessageRequest(&msg, request);
//}

//void cConTcpData::startSendAddSeasonTicket(DataConRequest request)
//{
//    QString     name = request.m_lData.at(1);
//    QByteArray  seasonTicket;
//    QDataStream wSeasonTicket(&seasonTicket, QIODevice::WriteOnly);
//    wSeasonTicket.setByteOrder(QDataStream::LittleEndian);
//    wSeasonTicket << request.m_lData.at(0).toUInt();
//    wSeasonTicket << quint16(name.toUtf8().size());
//    seasonTicket.append(name);

//    MessageProtocol msg(request.m_request, seasonTicket);
//    this->sendMessageRequest(&msg, request);
//}

//void cConTcpData::startSendRemoveSeasonTicket(DataConRequest request)
//{
//    quint32         index = request.m_lData.at(0).toUInt();
//    MessageProtocol msg(request.m_request, index);
//    this->sendMessageRequest(&msg, request);
//}

//void cConTcpData::startSendEditSeasonTicket(DataConRequest request)
//{
//    quint32    index    = qToLittleEndian(request.m_lData.at(0).toUInt());
//    QByteArray name     = request.m_lData.at(1).toUtf8();
//    QByteArray place    = request.m_lData.at(2).toUtf8();
//    quint32    discount = qToLittleEndian(request.m_lData.at(3).toUInt());
//    char       data[200];
//    memset(&data[0], 0x0, 200);
//    memcpy(&data[0], &index, sizeof(quint32));
//    memcpy(&data[4], &discount, sizeof(quint32));
//    memcpy(&data[8], name.constData(), name.length());
//    memcpy(&data[9 + name.length()], place.constData(), place.length());

//    quint32 size = 8 + name.length() + place.length() + 2;

//    MessageProtocol msg(request.m_request, &data[0], size);
//    this->sendMessageRequest(&msg, request);
//}

//void cConTcpData::startSendSeasonTicketListRequest(DataConRequest request)
//{
//    quint32 data[3];
//    qint64  timeStamp = this->m_pGlobalData->getSeasonTicketLastLocalUpdate();
//    if (timeStamp + TIMEOUT_UPDATE_TICKETS > QDateTime::currentMSecsSinceEpoch() && this->m_pGlobalData->getSeasonTicketLength() > 0)
//        data[0] = qToLittleEndian(UpdateIndex::UpdateDiff);
//    else
//        data[0] = qToLittleEndian(UpdateIndex::UpdateAll);

//    timeStamp = qToLittleEndian(this->m_pGlobalData->getSeasonTicketLastServerUpdate());
//    memcpy(&data[1], &timeStamp, sizeof(qint64));

//    MessageProtocol msg(request.m_request, (char*)(&data[0]), sizeof(quint32) * 3);
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

//void cConTcpData::startSendAvailableTicketListRequest(DataConRequest request)
//{
//    quint32 data[3];
//    qint64  timeStamp = qToLittleEndian(this->m_pGlobalData->getSeasonTicketLastServerUpdate());

//    data[0] = request.m_lData.at(0).toUInt();
//    memcpy(&data[1], &timeStamp, sizeof(qint64));

//    MessageProtocol msg(request.m_request, (char*)(&data[0]), sizeof(quint32) * 3);
//    this->sendMessageRequest(&msg, request);
//}

//void cConTcpData::startSendChangeGameRequest(DataConRequest request)
//{
//    QByteArray data;
//    data.append(request.m_lData.at(0));
//    data.append(char(0x00));

//    MessageProtocol msg(request.m_request, data);
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

//void cConTcpData::startSendChangeNewsData(DataConRequest request)
//{
//    QByteArray header = request.m_lData.at(1).toUtf8();
//    QByteArray info   = qCompress(request.m_lData.at(2).toUtf8(), 9);

//    qInfo().noquote() << QString("Compressed news info from %1 to %2 Bytes").arg(request.m_lData.at(2).size()).arg(info.size());

//    int   totalSize = header.size() + info.size() + sizeof(quint32) * 2 + 2;
//    char* data      = new char[totalSize];
//    memset(data, 0x0, totalSize);
//    quint32 newsIndex = qToLittleEndian(request.m_lData.at(0).toUInt());
//    quint32 infoSize  = qToLittleEndian(info.size());
//    memcpy(data, &newsIndex, sizeof(quint32));
//    memcpy(&data[sizeof(quint32)], &infoSize, sizeof(quint32));
//    memcpy(&data[sizeof(quint32) * 2], header.constData(), header.size());
//    memcpy(&data[sizeof(quint32) * 2 + 1 + header.size()], info.constData(), info.size());

//    MessageProtocol msg(request.m_request, data, totalSize);
//    this->sendMessageRequest(&msg, request);
//}

//void cConTcpData::startSendGetNewsDataList(DataConRequest request)
//{
//    quint32 data[3];
//    qint64  timeStamp = this->m_pGlobalData->getNewsDataLastLocalUpdate();
//    if (timeStamp + TIMEOUT_UPDATE_NEWS > QDateTime::currentMSecsSinceEpoch() && this->m_pGlobalData->getNewsDataItemLength() > 0)
//        data[0] = qToLittleEndian(UpdateIndex::UpdateDiff);
//    else
//        data[0] = qToLittleEndian(UpdateIndex::UpdateAll);

//    timeStamp = qToLittleEndian(this->m_pGlobalData->getNewsDataLastServerUpdate());
//    memcpy(&data[1], &timeStamp, sizeof(qint64));

//    MessageProtocol msg(request.m_request, (char*)(&data[0]), sizeof(quint32) * 3);
//    this->sendMessageRequest(&msg, request);
//}

//void cConTcpData::startSendGetNewDataItem(DataConRequest request)
//{
//    quint32 newsIndex = qToLittleEndian(request.m_lData.at(0).toUInt());

//    MessageProtocol msg(request.m_request, newsIndex);
//    this->sendMessageRequest(&msg, request);
//}

//void cConTcpData::startSendDeleteNewDataItem(DataConRequest request)
//{
//    quint32 newsIndex = qToLittleEndian(request.m_lData.at(0).toUInt());

//    MessageProtocol msg(request.m_request, newsIndex);
//    this->sendMessageRequest(&msg, request);
//}

//void cConTcpData::startSendStatisticsCommand(DataConRequest request)
//{
//    QByteArray data = request.m_lData.at(0).toUtf8();

//    MessageProtocol msg(request.m_request, data);
//    this->sendMessageRequest(&msg, request);
//}

//qint32 cConTcpData::sendMessageRequest(MessageProtocol* msg, DataConRequest request)
//{
//    if (this->m_pDataUdpSocket == NULL)
//        return -1;

//    quint32     sendBytes       = 0;
//    quint32     totalPacketSize = msg->getNetworkSize();
//    const char* pData           = msg->getNetworkProtocol();

//    do {
//        quint32 currentSendSize;
//        if ((totalPacketSize - sendBytes) > MAX_DATAGRAMM_SIZE)
//            currentSendSize = MAX_DATAGRAMM_SIZE;
//        else
//            currentSendSize = totalPacketSize - sendBytes;


//        qint64 rValue = this->m_pDataUdpSocket->writeDatagram(pData + sendBytes,
//                                                              currentSendSize,
//                                                              this->m_hDataReceiver,
//                                                              this->m_pGlobalData->conDataPort());

//        if (rValue < 0) {
//            request.m_result = ERROR_CODE_ERR_SEND;
//            emit this->notifyLastRequestFinished(request);

//            return rValue;
//        }
//        sendBytes += rValue;
//        //                QThread::msleep(25);
//    } while (sendBytes < totalPacketSize);

//    this->m_pConTimeout->start();

//    /* Only add when not sending request again */
//    if (!this->m_bRequestLoginAgain) {
//        this->m_lActualRequest.append(request);
//    }

//    return sendBytes;
//}

//void cConTcpData::removeActualRequest(quint32 req)
//{
//    for (int i = 0; i < this->m_lActualRequest.size(); i++) {
//        if (this->m_lActualRequest[i].m_request == req) {
//            //            if (this->m_lActualRequest[i].m_lData != NULL)
//            //                delete this->m_lActualRequest[i].m_lData;
//            this->m_lActualRequest.removeAt(i);
//        }
//    }
//}


//DataConRequest cConTcpData::getActualRequest(quint32 req)
//{
//    for (int i = 0; i < this->m_lActualRequest.size(); i++) {
//        if (this->m_lActualRequest[i].m_request == req) {
//            return this->m_lActualRequest[i];
//        }
//    }
//    return DataConRequest(0);
//}

//QString cConTcpData::getActualRequestData(quint32 req, qint32 index)
//{
//    for (int i = 0; i < this->m_lActualRequest.size(); i++) {
//        if (this->m_lActualRequest[i].m_request == req) {
//            if (this->m_lActualRequest[i].m_lData.size() > index)
//                return this->m_lActualRequest[i].m_lData.at(index);
//        }
//    }
//    return NULL;
//}

//void cConTcpData::sendActualRequestsAgain(qint32 result)
//{
//    if (result == ERROR_CODE_SUCCESS) {
//        qInfo().noquote() << "cConTcpData: Trying to send the requests again";
//        for (int i = 0; i < this->m_lActualRequest.size(); i++) {
//            this->startSendNewRequest(this->m_lActualRequest[i]);
//        }
//    } else {
//        while (this->m_lActualRequest.size() > 0) {
//            DataConRequest request = this->m_lActualRequest.last();
//            request.m_result       = result;
//            emit this->notifyLastRequestFinished(request);
//            this->m_lActualRequest.removeLast();
//        }
//    }
//}

//void cConTcpData::startSendNewRequest(DataConRequest request)
//{
//    switch (request.m_request) {
//    case OP_CODE_CMD_REQ::REQ_LOGIN_USER:
//        this->startSendLoginRequest(request);
//        break;

//    case OP_CODE_CMD_REQ::REQ_GET_VERSION:
//        this->startSendVersionRequest(request);
//        break;

//    case OP_CODE_CMD_REQ::REQ_GET_USER_PROPS:
//        this->startSendUserPropsRequest(request);
//        break;

//    case OP_CODE_CMD_REQ::REQ_GET_USER_EVENTS:
//        this->startSendGetUserEventsRequest(request);
//        break;

//    case OP_CODE_CMD_REQ::REQ_SET_USER_EVENTS:
//        this->startSendSetUserEventsRequest(request);
//        break;

//    case OP_CODE_CMD_REQ::REQ_USER_CHANGE_LOGIN:
//        if (request.m_lData.size() > 0)
//            this->startSendUpdPassRequest(request);
//        break;

//    case OP_CODE_CMD_REQ::REQ_USER_CHANGE_READNAME:
//        if (request.m_lData.size() > 0)
//            this->startSendReadableNameRequest(request);
//        break;

//    case OP_CODE_CMD_REQ::REQ_GET_GAMES_LIST:
//        this->startSendGamesListRequest(request);
//        break;

//    case OP_CODE_CMD_REQ::REQ_GET_GAMES_INFO_LIST:
//        this->startSendGamesInfoListRequest(request);
//        break;

//    case OP_CODE_CMD_REQ::REQ_SET_FIXED_GAME_TIME:
//        this->startSendSetGameTimeFixedRequest(request);
//        break;

//    case OP_CODE_CMD_REQ::REQ_ADD_TICKET:
//        if (request.m_lData.size() > 1)
//            this->startSendAddSeasonTicket(request);
//        break;

//    case OP_CODE_CMD_REQ::REQ_REMOVE_TICKET:
//        if (request.m_lData.size() > 0)
//            this->startSendRemoveSeasonTicket(request);
//        break;

//    case OP_CODE_CMD_REQ::REQ_GET_TICKETS_LIST:
//        this->startSendSeasonTicketListRequest(request);
//        break;

//    case OP_CODE_CMD_REQ::REQ_CHANGE_TICKET:
//        this->startSendEditSeasonTicket(request);
//        break;

//    case OP_CODE_CMD_REQ::REQ_STATE_CHANGE_SEASON_TICKET:
//        this->startSendChangeTicketState(request);
//        break;

//    case OP_CODE_CMD_REQ::REQ_GET_AVAILABLE_TICKETS:
//        this->startSendAvailableTicketListRequest(request);
//        break;

//    case OP_CODE_CMD_REQ::REQ_CHANGE_GAME:
//        this->startSendChangeGameRequest(request);
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

//    case OP_CODE_CMD_REQ::REQ_CHANGE_NEWS_DATA:
//        this->startSendChangeNewsData(request);
//        break;

//    case OP_CODE_CMD_REQ::REQ_GET_NEWS_DATA_LIST:
//        this->startSendGetNewsDataList(request);
//        break;

//    case OP_CODE_CMD_REQ::REQ_GET_NEWS_DATA_ITEM:
//        this->startSendGetNewDataItem(request);
//        break;

//    case OP_CODE_CMD_REQ::REQ_DEL_NEWS_DATA_ITEM:
//        this->startSendDeleteNewDataItem(request);
//        break;

//    case OP_CODE_CMD_REQ::REQ_CMD_STATISTIC:
//        this->startSendStatisticsCommand(request);
//        break;

//    default:
//        return;
//    }
//}

//QString cConTcpData::createHashValue(const QString first, const QString second)
//{
//    this->m_hash->reset();
//    QByteArray tmp = first.toUtf8();
//    this->m_hash->addData(tmp.constData(), tmp.length());
//    tmp = second.toUtf8();
//    this->m_hash->addData(tmp.constData(), tmp.length());

//    QString hashPassword(this->m_hash->result());

//    return hashPassword;
//}

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

    //    if (this->m_pDataUdpSocket != NULL)
    //        delete this->m_pDataUdpSocket;

    //    if (this->m_pDataHandle != NULL)
    //        delete this->m_pDataHandle;

    //    delete this->m_hash;
}
