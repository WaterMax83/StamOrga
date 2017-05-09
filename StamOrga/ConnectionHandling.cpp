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

#include "../Common/General/globalfunctions.h"
#include "../Common/General/globaltiming.h"
#include "../Common/Network/messagecommand.h"
#include "../Common/Network/messageprotocol.h"
#include "connectionhandling.h"

#define TIMER_DIFF_MSEC 10 * 1000

ConnectionHandling::ConnectionHandling(QObject* parent)
    : QObject(parent)
{
    this->m_pTimerConReset = new QTimer();
    this->m_pTimerConReset->setSingleShot(true);
    this->m_pTimerConReset->setInterval(CON_RESET_TIMEOUT_MSEC - TIMER_DIFF_MSEC);
    connect(this->m_pTimerConReset, &QTimer::timeout, this, &ConnectionHandling::slTimerConResetFired);

    this->m_pTimerLoginReset = new QTimer();
    this->m_pTimerLoginReset->setSingleShot(true);
    this->m_pTimerLoginReset->setInterval(CON_LOGIN_TIMEOUT_MSEC - TIMER_DIFF_MSEC);
    connect(this->m_pTimerLoginReset, &QTimer::timeout, this, &ConnectionHandling::slTimerConLoginFired);
}

QString mainConRequestPassWord;

qint32 ConnectionHandling::startMainConnection(QString name, QString passw)
{
    bool bChangedUserName = false;
    bool bChangedPassword = false;

    if (name != this->m_pGlobalData->userName())
        bChangedUserName = true;

    if (passw != this->m_pGlobalData->passWord())
        bChangedPassword = true;

    if (this->isMainConnectionActive()) {
        if (!bChangedUserName) {
            if (this->m_pGlobalData->bIsConnected() && !bChangedPassword) {
                emit this->sNotifyConnectionFinished(ERROR_CODE_SUCCESS);
                qDebug() << "Did not log in again, should already be succesfull";
                return ERROR_CODE_NO_ERROR;
            }
            this->startDataConnection();
            QThread::msleep(1);
            this->sendLoginRequest(passw);
            return ERROR_CODE_SUCCESS;
        }
        this->m_ctrlMainCon.Stop();
        this->stopDataConnection();
        QThread::msleep(2);
    } else
        this->stopDataConnection();

    this->m_pMainCon       = new MainConnection(this->m_pGlobalData, name);
    mainConRequestPassWord = passw;
    connect(this->m_pMainCon, &MainConnection::connectionRequestFinished, this, &ConnectionHandling::slMainConReqFin);
    this->m_ctrlMainCon.Start(this->m_pMainCon, false);


    return ERROR_CODE_SUCCESS;
}

qint32 ConnectionHandling::startGettingVersionInfo()
{
    DataConRequest req(OP_CODE_CMD_REQ::REQ_GET_VERSION);
    this->sendNewRequest(req);
    return ERROR_CODE_SUCCESS;
}

qint32 ConnectionHandling::startGettingUserProps()
{
    DataConRequest req(OP_CODE_CMD_REQ::REQ_GET_USER_PROPS);
    this->sendNewRequest(req);
    return ERROR_CODE_SUCCESS;
}

bool ConnectionHandling::startUpdatePassword(QString newPassWord)
{
    DataConRequest req(OP_CODE_CMD_REQ::REQ_USER_CHANGE_LOGIN);
    req.m_lData.append(newPassWord);
    this->sendNewRequest(req);
    return true;
}

qint32 ConnectionHandling::startUpdateReadableName(QString name)
{
    DataConRequest req(OP_CODE_CMD_REQ::REQ_USER_CHANGE_READNAME);
    req.m_lData.append(name);
    this->sendNewRequest(req);
    return ERROR_CODE_SUCCESS;
}

qint32 ConnectionHandling::startGettingGamesList()
{
    DataConRequest req(OP_CODE_CMD_REQ::REQ_GET_GAMES_LIST);
    this->sendNewRequest(req);
    return ERROR_CODE_SUCCESS;
}

qint32 ConnectionHandling::startSeasonTicketRemove(quint32 index)
{
    DataConRequest req(OP_CODE_CMD_REQ::REQ_REMOVE_TICKET);
    req.m_lData.append(QString::number(index));
    this->sendNewRequest(req);
    return ERROR_CODE_SUCCESS;
}

qint32 ConnectionHandling::startSeasonTicketNewPlace(quint32 index, QString place)
{
    DataConRequest req(OP_CODE_CMD_REQ::REQ_NEW_TICKET_PLACE);
    req.m_lData.append(QString::number(index));
    req.m_lData.append(place);
    this->sendNewRequest(req);
    return ERROR_CODE_SUCCESS;
}

qint32 ConnectionHandling::startSeasonTicketAdd(QString name, quint32 discount)
{
    DataConRequest req(OP_CODE_CMD_REQ::REQ_ADD_TICKET);
    req.m_lData.append(QString::number(discount));
    req.m_lData.append(name);
    this->sendNewRequest(req);
    return ERROR_CODE_SUCCESS;
}

qint32 ConnectionHandling::startGettingSeasonTicketList()
{
    DataConRequest req(OP_CODE_CMD_REQ::REQ_GET_TICKETS_LIST);
    this->sendNewRequest(req);
    return ERROR_CODE_SUCCESS;
}


/*
 * Answer function after connection with username
 */
void ConnectionHandling::slMainConReqFin(qint32 result, const QString& msg)
{
    if (result > ERROR_CODE_NO_ERROR) {
        this->m_pGlobalData->setConDataPort((quint16)result);
        this->startDataConnection();
        QThread::msleep(2);
        this->sendLoginRequest(mainConRequestPassWord);
        this->m_pTimerConReset->start();
    } else {
        qWarning() << "Error main connecting: " << msg;
        emit this->sNotifyConnectionFinished(result);
        this->m_ctrlMainCon.Stop();
        this->stopDataConnection();

        while (this->m_lErrorMainCon.size() > 0) {
            DataConRequest request = this->m_lErrorMainCon.last();
            request.m_result       = result;
            this->slDataConLastRequestFinished(request);
            this->m_lErrorMainCon.removeLast();
        }
    }
}

/*
 * Functions for login
 */
void ConnectionHandling::sendLoginRequest(QString password)
{
    this->startDataConnection(); // call it every time, if it is already started it just returns

    DataConRequest req;
    req.m_request = OP_CODE_CMD_REQ::REQ_LOGIN_USER;
    req.m_lData.append(password);

    emit this->sStartSendNewRequest(req);
}

void ConnectionHandling::sendNewRequest(DataConRequest request)
{
    if (this->isMainConnectionActive()) {
        this->startDataConnection();

        emit this->sStartSendNewRequest(request);
    } else {
        this->m_lErrorMainCon.prepend(request);
        this->startMainConnection(this->m_pGlobalData->userName(), this->m_pGlobalData->passWord());
    }
}


void ConnectionHandling::slDataConLastRequestFinished(DataConRequest request)
{
    switch (request.m_request) {
    case OP_CODE_CMD_REQ::REQ_LOGIN_USER:
        if (request.m_result == ERROR_CODE_SUCCESS) {
            this->m_pGlobalData->setbIsConnected(true);
            while (this->m_lErrorMainCon.size() > 0) {
                this->sendNewRequest(this->m_lErrorMainCon.last());
                this->m_lErrorMainCon.removeLast();
            }
        } else {
            qWarning().noquote() << QString("Error Login: %1").arg(getErrorCodeString(request.m_result));
            while (this->m_lErrorMainCon.size() > 0) {
                DataConRequest newReq = this->m_lErrorMainCon.last();
                newReq.m_result       = request.m_result;
                this->slDataConLastRequestFinished(newReq);
                this->m_lErrorMainCon.removeLast();
            }
        }

        emit this->sNotifyConnectionFinished(request.m_result);
        break;

    case OP_CODE_CMD_REQ::REQ_GET_VERSION:
        emit this->sNotifyVersionRequest(request.m_result, request.m_returnData);
        break;

    case OP_CODE_CMD_REQ::REQ_GET_USER_PROPS:
        if (request.m_result == ERROR_CODE_SUCCESS)
            this->m_pGlobalData->SetUserProperties(request.m_returnData.toUInt());
        emit this->sNotifyUserPropertiesRequest(request.m_result);
        break;

    case OP_CODE_CMD_REQ::REQ_USER_CHANGE_LOGIN:
        if (request.m_result == ERROR_CODE_SUCCESS) {
            this->m_pGlobalData->setPassWord(request.m_returnData);
            this->m_pGlobalData->saveGlobalUserSettings();
        }

        emit this->sNotifyUpdatePasswordRequest(request.m_result, request.m_returnData);
        break;

    case OP_CODE_CMD_REQ::REQ_USER_CHANGE_READNAME:
        if (request.m_result == ERROR_CODE_SUCCESS) {
            this->m_pGlobalData->setReadableName(request.m_returnData);
            this->m_pGlobalData->saveGlobalUserSettings();
        }

        emit this->sNotifyUpdateReadableNameRequest(request.m_result);
        break;

    case OP_CODE_CMD_REQ::REQ_GET_GAMES_LIST:
        emit this->sNotifyGamesListRequest(request.m_result);
        break;

    case OP_CODE_CMD_REQ::REQ_ADD_TICKET:
        emit this->sNotifySeasonTicketAddRequest(request.m_result);
        break;

    case OP_CODE_CMD_REQ::REQ_REMOVE_TICKET:
        emit this->sNotifySeasonTicketRemoveRequest(request.m_result);
        break;

    case OP_CODE_CMD_REQ::REQ_GET_TICKETS_LIST:
        emit this->sNotifySeasonTicketListRequest(request.m_result);
        break;

    case OP_CODE_CMD_REQ::REQ_NEW_TICKET_PLACE:
        emit this->sNotifySeasonTicketNewPlace(request.m_result);
        break;
    }

    this->checkTimeoutResult(request.m_result);
}

void ConnectionHandling::checkTimeoutResult(qint32 result)
{
    if (result == ERROR_CODE_TIMEOUT) {
        this->m_ctrlMainCon.Stop();
        this->stopDataConnection();
    } else if (this->m_pGlobalData->bIsConnected())
        this->m_pTimerLoginReset->start(); // restart Timer
}

void ConnectionHandling::startDataConnection()
{
    if (this->isDataConnectionActive())
        return;

    this->m_pDataCon = new DataConnection(this->m_pGlobalData);

    connect(this, &ConnectionHandling::sStartSendNewRequest,
            this->m_pDataCon, &DataConnection::startSendNewRequest);
    connect(this->m_pDataCon, &DataConnection::notifyLastRequestFinished,
            this, &ConnectionHandling::slDataConLastRequestFinished);

    this->m_ctrlDataCon.Start(this->m_pDataCon, false);
}

void ConnectionHandling::stopDataConnection()
{
    this->m_pGlobalData->setbIsConnected(false);

    if (!this->isDataConnectionActive())
        return;

    disconnect(this, &ConnectionHandling::sStartSendNewRequest,
               this->m_pDataCon, &DataConnection::startSendNewRequest);
    disconnect(this->m_pDataCon, &DataConnection::notifyLastRequestFinished,
               this, &ConnectionHandling::slDataConLastRequestFinished);

    this->m_ctrlDataCon.Stop();
}

void ConnectionHandling::slTimerConResetFired()
{
    this->m_ctrlMainCon.Stop();
}

void ConnectionHandling::slTimerConLoginFired()
{
    this->m_pGlobalData->setbIsConnected(false);
}

ConnectionHandling::~ConnectionHandling()
{
    if (this->isDataConnectionActive())
        this->stopDataConnection();

    if (this->isMainConnectionActive())
        this->m_ctrlMainCon.Stop();
}
