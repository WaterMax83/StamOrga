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
#include <QDebug>
#include <QtCore/QDateTime>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>

#include "../Common/General/globalfunctions.h"
#include "../Common/General/globaltiming.h"
#include "../Common/Network/messagecommand.h"
#include "../Common/Network/messageprotocol.h"
#include "cconmanager.h"
#include "cconsettings.h"

#define TIMER_DIFF_MSEC 10 * 1000

cConManager g_ConManager;

cConManager::cConManager(QObject* parent)
    : cGenDisposer(parent)
{
    this->m_pMainCon = NULL;
    //    this->m_lastSuccessTimeStamp = 0;
}

qint32 cConManager::initialize()
{
    qRegisterMetaType<TcpDataConRequest*>("DataConRequest*");

#if QT_VERSION < QT_VERSION_CHECK(5, 9, 0)
    this->m_hash = new QCryptographicHash(QCryptographicHash::Sha3_512);
#else
    this->m_hash = new QCryptographicHash(QCryptographicHash::Keccak_512);
#endif

    this->m_initialized = true;

    return ERROR_CODE_SUCCESS;
}


qint32 cConManager::startMainConnection(QString name, QString passw)
{
    if (!this->m_initialized)
        return ERROR_CODE_NOT_INITIALIZED;

    if (this->isMainConnectionActive())
        return ERROR_CODE_ALREADY_EXIST;

    this->stopDataConnection();

    if (this->m_pMainCon == NULL) {
        this->m_pMainCon = new cConTcpMain();
        qint32 rCode     = this->m_pMainCon->initialize(g_ConSettings.getIPAddr(), name);
        if (rCode != ERROR_CODE_SUCCESS)
            return rCode;
        connect(this->m_pMainCon, &cConTcpMain::connectionRequestFinished, this, &cConManager::slMainConReqFin);
        this->m_ctrlMainCon.Start(this->m_pMainCon, false);
    }

    this->m_mainConRequestUserName = name;
    this->m_mainConRequestPassWord = passw;
    this->m_mainConRequestDataPort = -1;
    this->m_bIsConnecting          = true;

    return ERROR_CODE_SUCCESS;
}

//qint32 cConManager::startGettingVersionInfo()
//{
//    DataConRequest req(OP_CODE_CMD_REQ::REQ_GET_VERSION);
//    this->sendNewRequest(req);
//    return ERROR_CODE_SUCCESS;
//}

//qint32 cConManager::startGettingUserProps()
//{
//    DataConRequest req(OP_CODE_CMD_REQ::REQ_GET_USER_PROPS);
//    this->sendNewRequest(req);
//    return ERROR_CODE_SUCCESS;
//}

//qint32 cConManager::startGettingUserEvents()
//{
//    DataConRequest req(OP_CODE_CMD_REQ::REQ_GET_USER_EVENTS);
//    this->sendNewRequest(req);
//    return ERROR_CODE_SUCCESS;
//}

//qint32 cConManager::startSettingUserEvents(qint64 eventID, qint32 status)
//{
//    DataConRequest req(OP_CODE_CMD_REQ::REQ_SET_USER_EVENTS);
//    req.m_lData.append(QString::number(eventID));
//    req.m_lData.append(QString::number(status));
//    this->sendNewRequest(req);
//    return ERROR_CODE_SUCCESS;
//}

//bool cConManager::startUpdatePassword(QString newPassWord)
//{
//    DataConRequest req(OP_CODE_CMD_REQ::REQ_USER_CHANGE_LOGIN);
//    req.m_lData.append(newPassWord);
//    this->sendNewRequest(req);
//    return true;
//}

//qint32 cConManager::startUpdateReadableName(QString name)
//{
//    DataConRequest req(OP_CODE_CMD_REQ::REQ_USER_CHANGE_READNAME);
//    req.m_lData.append(name);
//    this->sendNewRequest(req);
//    return ERROR_CODE_SUCCESS;
//}

//qint32 cConManager::startListGettingGames()
//{
//    DataConRequest req(OP_CODE_CMD_REQ::REQ_GET_GAMES_LIST);
//    this->sendNewRequest(req);
//    return ERROR_CODE_SUCCESS;
//}

//qint32 cConManager::startListGettingGamesInfo()
//{
//    DataConRequest req(OP_CODE_CMD_REQ::REQ_GET_GAMES_INFO_LIST);
//    this->sendNewRequest(req);
//    return ERROR_CODE_SUCCESS;
//}

//qint32 cConManager::startSetFixedGameTime(const quint32 gameIndex, const quint32 fixedTime)
//{
//    DataConRequest req(OP_CODE_CMD_REQ::REQ_SET_FIXED_GAME_TIME);
//    req.m_lData.append(QString::number(gameIndex));
//    req.m_lData.append(QString::number(fixedTime));
//    this->sendNewRequest(req);
//    return ERROR_CODE_SUCCESS;
//}

//qint32 cConManager::startRemoveSeasonTicket(quint32 index)
//{
//    DataConRequest req(OP_CODE_CMD_REQ::REQ_REMOVE_TICKET);
//    req.m_lData.append(QString::number(index));
//    this->sendNewRequest(req);
//    return ERROR_CODE_SUCCESS;
//}

//qint32 cConManager::startEditSeasonTicket(quint32 index, QString name, QString place, quint32 discount)
//{
//    DataConRequest req(OP_CODE_CMD_REQ::REQ_CHANGE_TICKET);
//    req.m_lData.append(QString::number(index));
//    req.m_lData.append(name);
//    req.m_lData.append(place);
//    req.m_lData.append(QString::number(discount));
//    this->sendNewRequest(req);
//    return ERROR_CODE_SUCCESS;
//}

//qint32 cConManager::startAddSeasonTicket(QString name, quint32 discount)
//{
//    DataConRequest req(OP_CODE_CMD_REQ::REQ_ADD_TICKET);
//    req.m_lData.append(QString::number(discount));
//    req.m_lData.append(name);
//    this->sendNewRequest(req);
//    return ERROR_CODE_SUCCESS;
//}

//qint32 cConManager::startListSeasonTickets()
//{
//    DataConRequest req(OP_CODE_CMD_REQ::REQ_GET_TICKETS_LIST);
//    this->sendNewRequest(req);
//    return ERROR_CODE_SUCCESS;
//}

//qint32 cConManager::startChangeSeasonTicketState(quint32 tickedIndex, quint32 gameIndex, quint32 state, QString name)
//{
//    DataConRequest req(OP_CODE_CMD_REQ::REQ_STATE_CHANGE_SEASON_TICKET);
//    req.m_lData.append(QString::number(tickedIndex));
//    req.m_lData.append(QString::number(gameIndex));
//    req.m_lData.append(QString::number(state));
//    req.m_lData.append(name);
//    this->sendNewRequest(req);
//    return ERROR_CODE_SUCCESS;
//}

//qint32 cConManager::startListAvailableTicket(quint32 gameIndex)
//{
//    /* check if update was more than one our ago, than update items */
//    //    qint64 oneHourAgo = QDateTime::currentDateTime().addSecs(-(60 * 60)).toMSecsSinceEpoch();
//    //    if (oneHourAgo > this->m_pGlobalData->getSeasonTicketLastLocalUpdate())
//    //        return this->startListSeasonTickets();

//    DataConRequest req(OP_CODE_CMD_REQ::REQ_GET_AVAILABLE_TICKETS);
//    req.m_lData.append(QString::number(gameIndex));
//    this->sendNewRequest(req);
//    return ERROR_CODE_SUCCESS;
//}

//qint32 cConManager::startChangeGame(const quint32 index, const quint32 sIndex,
//                                           const QString competition, const QString home,
//                                           const QString away, const QString date,
//                                           const QString score)
//{
//    if (sIndex > 34 || competition == "" || home == "" || away == "" || date == "") {
//        qWarning().noquote() << "Standart parameter for changing game are wrong";
//        return ERROR_CODE_WRONG_PARAMETER;
//    }
//    if (home.contains(";") || away.contains(";") || date.contains(";")) {
//        qWarning().noquote() << "String parameter for changing game contain semicolons";
//        return ERROR_CODE_WRONG_PARAMETER;
//    }

//    CompetitionIndex compIndex = getCompetitionIndex(competition);
//    if (compIndex == NO_COMPETITION)
//        return ERROR_CODE_WRONG_PARAMETER;

//    QString        val = QString("%1;%2;%3;%4;%5;%6;%7").arg(home, away, date, score).arg(index).arg(sIndex).arg(compIndex);
//    DataConRequest req(OP_CODE_CMD_REQ::REQ_CHANGE_GAME);
//    req.m_lData.append(val);
//    this->sendNewRequest(req);
//    return ERROR_CODE_SUCCESS;
//}

//qint32 cConManager::startSaveMeetingInfo(const quint32 gameIndex, const QString when, const QString where, const QString info,
//                                                const quint32 type)
//{
//    MeetingInfo* pInfo = this->m_pGlobalData->getMeetingInfo(type);

//    if (pInfo->when() != when || pInfo->where() != where || pInfo->info() != info) {
//        DataConRequest req;
//        if (type == MEETING_TYPE_MEETING)
//            req.m_request = OP_CODE_CMD_REQ::REQ_CHANGE_MEETING_INFO;
//        else
//            req.m_request = OP_CODE_CMD_REQ::REQ_CHANGE_AWAYTRIP_INFO;
//        req.m_lData.append(QString::number(gameIndex));
//        req.m_lData.append(when);
//        req.m_lData.append(where);
//        req.m_lData.append(info);
//        req.m_lData.append(QString::number(type));
//        this->sendNewRequest(req);

//        return ERROR_CODE_SUCCESS;
//    }
//    return ERROR_CODE_NO_ERROR;
//}

//qint32 cConManager::startLoadMeetingInfo(const quint32 gameIndex, const quint32 type)
//{
//    DataConRequest req;
//    if (type == MEETING_TYPE_MEETING)
//        req.m_request = OP_CODE_CMD_REQ::REQ_GET_MEETING_INFO;
//    else
//        req.m_request = OP_CODE_CMD_REQ::REQ_GET_AWAYTRIP_INFO;
//    req.m_lData.append(QString::number(gameIndex));
//    req.m_lData.append(QString::number(type));
//    this->sendNewRequest(req);

//    return ERROR_CODE_SUCCESS;
//}

//qint32 cConManager::startAcceptMeetingInfo(const quint32 gameIndex, const quint32 accept,
//                                                  const QString name, const quint32 type,
//                                                  const quint32 acceptIndex)
//{
//    DataConRequest req;
//    if (type == MEETING_TYPE_MEETING)
//        req.m_request = OP_CODE_CMD_REQ::REQ_ACCEPT_MEETING;
//    else
//        req.m_request = OP_CODE_CMD_REQ::REQ_ACCEPT_AWAYTRIP;
//    req.m_lData.append(QString::number(gameIndex));
//    req.m_lData.append(QString::number(accept));
//    req.m_lData.append(QString::number(acceptIndex));
//    req.m_lData.append(name);
//    req.m_lData.append(QString::number(type));
//    this->sendNewRequest(req);

//    return ERROR_CODE_SUCCESS;
//}

//qint32 cConManager::startChangeFanclubNews(const quint32 newsIndex, const QString header, const QString info)
//{
//    DataConRequest req(OP_CODE_CMD_REQ::REQ_CHANGE_NEWS_DATA);
//    req.m_lData.append(QString::number(newsIndex));
//    req.m_lData.append(header);
//    req.m_lData.append(info);
//    this->sendNewRequest(req);

//    return ERROR_CODE_SUCCESS;
//}

//qint32 cConManager::startListFanclubNews()
//{
//    DataConRequest req(OP_CODE_CMD_REQ::REQ_GET_NEWS_DATA_LIST);
//    this->sendNewRequest(req);

//    return ERROR_CODE_SUCCESS;
//}

//qint32 cConManager::startGetFanclubNewsItem(const quint32 newsIndex)
//{
//    DataConRequest req(OP_CODE_CMD_REQ::REQ_GET_NEWS_DATA_ITEM);
//    req.m_lData.append(QString::number(newsIndex));
//    this->sendNewRequest(req);

//    return ERROR_CODE_SUCCESS;
//}

//qint32 cConManager::startDeleteFanclubNewsItem(const quint32 newsIndex)
//{
//    DataConRequest req(OP_CODE_CMD_REQ::REQ_DEL_NEWS_DATA_ITEM);
//    req.m_lData.append(QString::number(newsIndex));
//    this->sendNewRequest(req);

//    return ERROR_CODE_SUCCESS;
//}

//qint32 cConManager::startStatisticsCommand(const QByteArray& command)
//{
//    DataConRequest req(OP_CODE_CMD_REQ::REQ_CMD_STATISTIC);
//    req.m_lData.append(QString(command));
//    this->sendNewRequest(req);

//    return ERROR_CODE_SUCCESS;
//}

/*
     * Answer function after connection with username
     */
void cConManager::slMainConReqFin(qint32 result, const QString msg, const QString salt, const QString random)
{
    //    qDebug() << QString("Resul = %1: %2").arg(result).arg(msg);
    if (result > ERROR_CODE_NO_ERROR) {
        qInfo().noquote() << QString("Trying login request with port %1").arg(result);
        this->m_mainConRequestDataPort = result;
        this->m_mainConRequestSalt     = salt;
        this->m_mainConRequestRandom   = random;
        this->startDataConnection();

        if (g_ConSettings.getUserName() != m_mainConRequestUserName)
            g_ConSettings.setUserName(m_mainConRequestUserName);

        //        QThread::msleep(20);

        //        this->sendLoginRequest(mainConRequestPassWord);
        //        this->m_lastSuccessTimeStamp = QDateTime::currentMSecsSinceEpoch();
    } else {
        //        qWarning().noquote() << QString("Error main connecting: %1").arg(msg);
        //        this->m_ctrlMainCon.Stop();
        //        this->m_pMainCon = NULL;
        //        this->stopDataConnection();
        this->m_bIsConnecting = false;
        emit this->sNotifyConnectionFinished(result, msg);

        //        if (result == ERROR_CODE_NO_USER)
        //            g_GlobalSettings->updateConnectionStatus(false);

        //        while (this->m_lErrorMainCon.size() > 0) {
        //            DataConRequest request = this->m_lErrorMainCon.last();
        //            request.m_result       = result;
        //            this->slDataConLastRequestFinished(request);
        //            this->m_lErrorMainCon.removeLast();
        //        }

        //        if (result == ERROR_CODE_NO_USER) {
        //        }
    }
    this->m_ctrlMainCon.Stop();
    this->m_pMainCon = NULL;
}

void cConManager::slotDataConnnectionFinished(qint32 result, const QString msg)
{
    //    if (this->m_bIsConnecting)
    //        emit this->sNotifyConnectionFinished(result, msg);
    //    this->m_bIsConnecting = false;

    //    this->stopDataConnection();
    if (result == ERROR_CODE_SUCCESS) {
        this->sendLoginRequest();
    } else {
        emit this->sNotifyConnectionFinished(result, msg);
        this->stopDataConnection();
    }
}

///*
// * Functions for login
// */
void cConManager::sendLoginRequest()
{
    QString passWord = this->m_mainConRequestPassWord;
    if (g_ConSettings.getSalt() == "")
        passWord = this->createHashValue(passWord, this->m_mainConRequestSalt);


    passWord = this->createHashValue(passWord, this->m_mainConRequestRandom);
    QJsonObject rootObj;
    rootObj.insert("passWord", passWord);

    TcpDataConRequest* req = new TcpDataConRequest();
    req->m_request         = OP_CODE_CMD_REQ::REQ_LOGIN_USER;
    req->m_lData           = QJsonDocument(rootObj).toJson(QJsonDocument::Compact);

    this->sendNewRequest(req);
}

void cConManager::sendNewRequest(TcpDataConRequest* request)
{
    if (this->isDataConnectionActive()) {
        emit this->signalStartSendNewRequest(request);
    } else {
        qInfo().noquote() << QString("Trying to restart connection from cConManager ") << QThread::currentThreadId();
        this->startMainConnection(g_ConSettings.getUserName(), g_ConSettings.getPassWord());
    }
    //    qint64 now = QDateTime::currentMSecsSinceEpoch();
    //    if ((now - this->m_lastSuccessTimeStamp) < (CON_RESET_TIMEOUT_MSEC - TIMER_DIFF_MSEC)) {
    //        this->startDataConnection();

    //        if ((now - this->m_lastSuccessTimeStamp) < (CON_LOGIN_TIMEOUT_MSEC - TIMER_DIFF_MSEC)) {
    //            emit this->sStartSendNewRequest(request);
    //            return;
    //        } else {
    //            this->m_lErrorMainCon.prepend(request);
    //            if (this->m_lErrorMainCon.size() == 1) {
    //                qInfo().noquote() << QString("Trying to reconnect from cConManager");
    //                this->m_pGlobalData->setbIsConnected(false);
    //                this->sendLoginRequest(this->m_pGlobalData->passWord());
    //            }
    //        }
    //    } else {
    //        this->m_lErrorMainCon.prepend(request);
    //        if (this->m_lErrorMainCon.size() == 1) {
    //            qInfo().noquote() << QString("Trying to restart connection from cConManager ") << QThread::currentThreadId();
    //            this->startMainConnection(this->m_pGlobalData->userName(), this->m_pGlobalData->passWord());
    //        }
    //    }
}


//void cConManager::slDataConLastRequestFinished(DataConRequest request)
//{
//    this->checkTimeoutResult(request.m_result);

//    switch (request.m_request) {
//    case OP_CODE_CMD_REQ::REQ_LOGIN_USER:
//        if (request.m_result == ERROR_CODE_SUCCESS) {
//            this->startGettingVersionInfo();
//            this->startGettingUserProps();
//            QThread::msleep(10);
//            this->m_pGlobalData->setbIsConnected(true);
//            this->checkTimeoutResult(request.m_result); // call again to set last successfull timer
//            while (this->m_lErrorMainCon.size() > 0) {
//                this->sendNewRequest(this->m_lErrorMainCon.last());
//                this->m_lErrorMainCon.removeLast();
//            }
//        } else {
//            qWarning().noquote() << QString("Error Login: %1").arg(getErrorCodeString(request.m_result));
//            while (this->m_lErrorMainCon.size() > 0) {
//                DataConRequest newReq = this->m_lErrorMainCon.last();
//                newReq.m_result       = request.m_result;
//                this->slDataConLastRequestFinished(newReq);
//                this->m_lErrorMainCon.removeLast();
//            }
//            if (request.m_result == ERROR_CODE_WRONG_PASSWORD)
//                g_GlobalSettings->updateConnectionStatus(false);
//        }

//        emit this->sNotifyConnectionFinished(request.m_result);
//        break;

//    case OP_CODE_CMD_REQ::REQ_GET_VERSION:
//        g_GlobalSettings->updateConnectionStatus(true);
//        emit this->sNotifyVersionRequest(request.m_result, request.m_returnData);
//        return;

//    case OP_CODE_CMD_REQ::REQ_GET_USER_PROPS:
//        if (request.m_result == ERROR_CODE_SUCCESS) {
//            this->startGettingUserEvents();
//        }
//        emit this->sNotifyCommandFinished(request.m_request, request.m_result);

//        break;

//    case OP_CODE_CMD_REQ::REQ_USER_CHANGE_LOGIN:
//        if (request.m_result == ERROR_CODE_SUCCESS) {
//            this->m_pGlobalData->setPassWord(request.m_returnData);
//            this->m_pGlobalData->saveGlobalUserSettings();
//        }

//        emit this->sNotifyUpdatePasswordRequest(request.m_result, request.m_returnData);
//        break;

//    case OP_CODE_CMD_REQ::REQ_USER_CHANGE_READNAME:
//        if (request.m_result == ERROR_CODE_SUCCESS) {
//            this->m_pGlobalData->setReadableName(request.m_returnData);
//            this->m_pGlobalData->saveGlobalUserSettings();
//        }

//        emit this->sNotifyCommandFinished(request.m_request, request.m_result);
//        break;

//    case OP_CODE_CMD_REQ::REQ_GET_GAMES_INFO_LIST: {

//        static quint32 retryGetGamesInfoCount = 0;
//        if (request.m_result == ERROR_CODE_UPDATE_LIST) {
//            if (retryGetGamesInfoCount < 3) {
//                this->startListGettingGames();
//                retryGetGamesInfoCount++;
//                return;
//            }
//        }
//        this->startGettingUserProps();
//        emit this->sNotifyCommandFinished(request.m_request, request.m_result);
//        retryGetGamesInfoCount = 0;
//        break;
//    }

//    case OP_CODE_CMD_REQ::REQ_REMOVE_TICKET:
//        this->m_pGlobalData->resetSeasonTicketLastServerUpdate();
//        emit this->sNotifyCommandFinished(request.m_request, request.m_result);
//        break;

//    case OP_CODE_CMD_REQ::REQ_STATE_CHANGE_SEASON_TICKET:
//        this->startGettingUserProps();
//        emit this->sNotifyCommandFinished(request.m_request, request.m_result);
//        break;

//    case OP_CODE_CMD_REQ::REQ_GET_AVAILABLE_TICKETS: {

//        static quint32 retryGetTicketCount = 0;
//        if (request.m_result == ERROR_CODE_UPDATE_LIST || request.m_result == ERROR_CODE_MISSING_TICKET) {
//            if (retryGetTicketCount < 3) {
//                this->startListSeasonTickets();
//                retryGetTicketCount++;
//                return;
//            }
//        }
//        emit this->sNotifyCommandFinished(request.m_request, request.m_result);
//        retryGetTicketCount = 0;
//        break;
//    }
//    case OP_CODE_CMD_REQ::REQ_GET_MEETING_INFO:
//    case OP_CODE_CMD_REQ::REQ_GET_AWAYTRIP_INFO:
//        emit this->sNotifyCommandFinished(request.m_request, request.m_result);
//        if (request.m_result == ERROR_CODE_NOT_FOUND)
//            return;
//        break;

//    case OP_CODE_CMD_REQ::REQ_CHANGE_NEWS_DATA: {
//        if (request.m_result != ERROR_CODE_SUCCESS)
//            emit this->sNotifyCommandFinished(request.m_request, request.m_result);
//        else {
//            this->m_pGlobalData->createNewNewsDataItem(request.m_returnData.toInt(), request.m_lData[1], request.m_lData[2]);
//            emit this->sNotifyCommandFinished(request.m_request, request.m_returnData.toInt());
//        }
//        break;
//    }
//    case OP_CODE_CMD_REQ::REQ_DEL_NEWS_DATA_ITEM:
//        this->m_pGlobalData->resetNewsDataLastServerUpdate();
//        emit this->sNotifyCommandFinished(request.m_request, request.m_result);
//        break;

//    default:
//        emit this->sNotifyCommandFinished(request.m_request, request.m_result);
//        break;
//    }

//    if (request.m_result != ERROR_CODE_SUCCESS)
//        qWarning().noquote() << QString("Error receiving Command 0x%1 has result \"%2\"")
//                                    .arg(QString::number(request.m_request, 16))
//                                    .arg(getErrorCodeString(request.m_result));
//}

//void cConManager::checkTimeoutResult(qint32 result)
//{
//    if (result == ERROR_CODE_TIMEOUT) {
//        this->stopDataConnection();
//        this->m_lastSuccessTimeStamp = 0;
//        emit this->sSendNewBindingPortRequest();
//    } else if (this->m_pGlobalData->bIsConnected())
//        this->m_lastSuccessTimeStamp = QDateTime::currentMSecsSinceEpoch();
//}

QString cConManager::createHashValue(const QString first, const QString second)
{
    this->m_hash->reset();
    QByteArray tmp = first.toUtf8();
    this->m_hash->addData(tmp.constData(), tmp.length());
    tmp = second.toUtf8();
    this->m_hash->addData(tmp.constData(), tmp.length());

    return QString(this->m_hash->result());
}

void cConManager::startDataConnection()
{
    if (this->isDataConnectionActive()) {
        //        this->m_pDataCon->setRandomLoginValue(mainConRequestRandom);
        return;
    }

    this->m_pDataCon = new cConTcpData();
    this->m_pDataCon->initialize(g_ConSettings.getIPAddr(), this->m_mainConRequestDataPort);

    connect(this->m_pDataCon, &cConTcpData::signalDataConnectionFinished,
            this, &cConManager::slotDataConnnectionFinished);
    connect(this, &cConManager::signalStartSendNewRequest,
            this->m_pDataCon, &cConTcpData::startSendNewRequest);
    //        connect(this->m_pDataCon, &cConTcpData::notifyLastRequestFinished,
    //                this, &cConManager::slDataConLastRequestFinished);

    this->m_ctrlDataCon.Start(this->m_pDataCon, false);

    QThread::msleep(25);
}

void cConManager::stopDataConnection()
{
    //    this->m_pGlobalData->setbIsConnected(false);

    if (!this->isDataConnectionActive())
        return;

    disconnect(this->m_pDataCon, &cConTcpData::signalDataConnectionFinished,
               this, &cConManager::slotDataConnnectionFinished);
    disconnect(this, &cConManager::signalStartSendNewRequest,
               this->m_pDataCon, &cConTcpData::startSendNewRequest);
    //    disconnect(this->m_pDataCon, &cConTcpData::notifyLastRequestFinished,
    //               this, &cConManager::slDataConLastRequestFinished);

    this->m_pDataCon->terminate();
    this->m_ctrlDataCon.Stop();

    QThread::msleep(50);
}


cConManager::~cConManager()
{
    if (this->m_hash)
        delete this->m_hash;

    //    if (this->isMainConnectionActive())
    //        this->m_ctrlMainCon.Stop();
}
