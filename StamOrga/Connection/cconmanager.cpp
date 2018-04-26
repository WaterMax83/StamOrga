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
#include "../Data/cdatagamesmanager.h"
#include "../Data/cdataticketmanager.h"
#include "../cstaglobalsettings.h"
#include "cconmanager.h"
#include "cconusersettings.h"

cConManager* g_ConManager;

cConManager::cConManager(QObject* parent)
    : cGenDisposer(parent)
{
    this->m_pMainCon = NULL;
}

qint32 cConManager::initialize()
{
    qRegisterMetaType<TcpDataConRequest*>("DataConRequest*");

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
        qint32 rCode     = this->m_pMainCon->initialize(g_ConUserSettings->getIPAddr(), name);
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
        g_ConUserSettings->setRandomLoginValue(random);
        this->startDataConnection();

        if (g_ConUserSettings->getUserName() != m_mainConRequestUserName)
            g_ConUserSettings->setUserName(m_mainConRequestUserName);

        //        QThread::msleep(20);

        //        this->sendLoginRequest(mainConRequestPassWord);
        //        this->m_lastSuccessTimeStamp = QDateTime::currentMSecsSinceEpoch();
    } else {
        qWarning().noquote() << QString("Error main connecting: %1").arg(msg);
        this->stopDataConnection();
        this->m_bIsConnecting = false;
        emit this->signalNotifyConnectionFinished(result, msg);

        if (result == ERROR_CODE_NO_USER)
            g_StaGlobalSettings->setAlreadyConnected(false);

        while (this->m_lRequestConError.size() > 0) {
            TcpDataConRequest* request = this->m_lRequestConError.last();
            request->m_result          = result;
            this->slotDataConLastRequestFinished(request);
            this->m_lRequestConError.removeLast();
        }
    }
    this->m_ctrlMainCon.Stop();
    this->m_pMainCon = NULL;
}

void cConManager::slotDataConnnectionFinished(qint32 result, const QString msg)
{
    if (result == ERROR_CODE_SUCCESS) {
        this->sendLoginRequest();
    } else {
        if (this->m_bIsConnecting)
            emit this->signalNotifyConnectionFinished(result, msg);
        this->stopDataConnection();
    }
}

/*
 * Functions for login
 */
void cConManager::sendLoginRequest()
{
    if (!this->m_initialized)
        return;

    QString passWord = this->m_mainConRequestPassWord;
    if (g_ConUserSettings->getSalt() == "")
        passWord = g_ConUserSettings->createHashValue(passWord, this->m_mainConRequestSalt);


    passWord = g_ConUserSettings->createHashValue(passWord, this->m_mainConRequestRandom);
    QJsonObject rootObj;
    rootObj.insert("passWord", passWord);

    TcpDataConRequest* req = new TcpDataConRequest(OP_CODE_CMD_REQ::REQ_LOGIN_USER);
    req->m_lData           = QJsonDocument(rootObj).toJson(QJsonDocument::Compact);

    this->sendNewRequest(req);
}

void cConManager::sendNewRequest(TcpDataConRequest* request)
{
    if (!this->m_initialized)
        return;

    if (this->isDataConnectionActive()) {
        emit this->signalStartSendNewRequest(request);
    } else {

        this->m_lRequestConError.prepend(request);
        if (this->m_lRequestConError.size() == 1) {
            qInfo().noquote() << QString("Trying to restart connection from cConManager ");
            this->startMainConnection(g_ConUserSettings->getUserName(), g_ConUserSettings->getPassWord());
        }
    }
}


void cConManager::slotDataConLastRequestFinished(TcpDataConRequest* request)
{
    this->checkTimeoutResult(request->m_result);

    switch (request->m_request) {
    case OP_CODE_CMD_REQ::REQ_LOGIN_USER:
        if (request->m_result == ERROR_CODE_SUCCESS) {
            g_StaGlobalSettings->startGettingVersionInfo();
            g_ConUserSettings->startGettingUserProps(false);

            while (this->m_lRequestConError.size() > 0) {
                TcpDataConRequest* request = this->m_lRequestConError.last();
                if (request->m_request != OP_CODE_CMD_REQ::REQ_USER_CHANGE_LOGIN)
                    this->sendNewRequest(request);
                else
                    g_ConUserSettings->startUpdatePassword("");
                this->m_lRequestConError.removeLast();
            }
            QString passWord = this->m_mainConRequestPassWord;
            if (g_ConUserSettings->getSalt() == "")
                passWord = g_ConUserSettings->createHashValue(passWord, this->m_mainConRequestSalt);

            g_ConUserSettings->setSalt(this->m_mainConRequestSalt);
            g_ConUserSettings->setPassWord(passWord);

            emit this->signalNotifyConnectionFinished(request->m_result, "");
        } else {
            qWarning().noquote() << QString("Error Login: %1").arg(request->m_result);
            while (this->m_lRequestConError.size() > 0) {
                TcpDataConRequest* req = this->m_lRequestConError.last();
                req->m_result          = request->m_result;
                this->slotDataConLastRequestFinished(req);
                this->m_lRequestConError.removeLast();
            }
            if (request->m_result == ERROR_CODE_WRONG_PASSWORD)
                g_StaGlobalSettings->setAlreadyConnected(false);

            g_ConUserSettings->setSalt("");
            g_ConUserSettings->setPassWord("");
            emit this->signalNotifyConnectionFinished(request->m_result, getErrorCodeString(request->m_result));
            this->stopDataConnection();
        }

        this->m_bIsConnecting = false;
        break;

    case OP_CODE_CMD_REQ::REQ_GET_GAMES_INFO_LIST: {

        static quint32 retryGetGamesInfoCount = 0;
        if (request->m_result == ERROR_CODE_UPDATE_LIST) {
            if (retryGetGamesInfoCount < 3) {
                g_DataGamesManager->startListGames();
                retryGetGamesInfoCount++;
                delete request;
                return;
            }
        }
        emit this->signalNotifyCommandFinished(request->m_request, request->m_result);
        retryGetGamesInfoCount = 0;
        break;
    }

    case OP_CODE_CMD_REQ::REQ_GET_AVAILABLE_TICKETS: {

        static quint32 retryGetTicketCount = 0;
        if (request->m_result == ERROR_CODE_UPDATE_LIST || request->m_result == ERROR_CODE_MISSING_TICKET) {
            if (retryGetTicketCount < 3) {
                g_DataTicketManager->startListSeasonTickets();
                retryGetTicketCount++;
                delete request;
                return;
            }
        }
        emit this->signalNotifyCommandFinished(request->m_request, request->m_result);
        retryGetTicketCount = 0;
        break;
    }
    case OP_CODE_CMD_REQ::REQ_GET_MEETING_INFO:
    case OP_CODE_CMD_REQ::REQ_GET_AWAYTRIP_INFO:
        emit this->signalNotifyCommandFinished(request->m_request, request->m_result);
        if (request->m_result == ERROR_CODE_NOT_FOUND)
            return;
        break;

    default:
        emit this->signalNotifyCommandFinished(request->m_request, request->m_result);
        break;
    }

    if (request->m_result != ERROR_CODE_SUCCESS)
        qWarning().noquote() << QString("Error receiving Command 0x%1 has result \"%2\"")
                                    .arg(request->m_request, 8, 16)
                                    .arg(getErrorCodeString(request->m_result));

    delete request;
}

void cConManager::checkTimeoutResult(qint32 result)
{
    if (result == ERROR_CODE_TIMEOUT) {
        this->stopDataConnection();
    }
}

void cConManager::startDataConnection()
{
    if (this->isDataConnectionActive()) {
        return;
    }

    this->m_pDataCon = new cConTcpData();
    this->m_pDataCon->initialize(g_ConUserSettings->getIPAddr(), this->m_mainConRequestDataPort);

    connect(this->m_pDataCon, &cConTcpData::signalDataConnectionFinished,
            this, &cConManager::slotDataConnnectionFinished);
    connect(this, &cConManager::signalStartSendNewRequest,
            this->m_pDataCon, &cConTcpData::startSendNewRequest);
    connect(this->m_pDataCon, &cConTcpData::notifyLastRequestFinished,
            this, &cConManager::slotDataConLastRequestFinished);

    this->m_ctrlDataCon.Start(this->m_pDataCon, false);

    QThread::msleep(25);
}

void cConManager::stopDataConnection()
{
    this->m_bIsConnecting = false;

    if (!this->isDataConnectionActive())
        return;

    disconnect(this->m_pDataCon, &cConTcpData::signalDataConnectionFinished,
               this, &cConManager::slotDataConnnectionFinished);
    disconnect(this, &cConManager::signalStartSendNewRequest,
               this->m_pDataCon, &cConTcpData::startSendNewRequest);
    disconnect(this->m_pDataCon, &cConTcpData::notifyLastRequestFinished,
               this, &cConManager::slotDataConLastRequestFinished);

    this->m_pDataCon->terminate();
    this->m_ctrlDataCon.Stop();

    QThread::msleep(50);
}


cConManager::~cConManager()
{
}
