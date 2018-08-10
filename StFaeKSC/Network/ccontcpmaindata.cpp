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

#include "../Common/General/config.h"
#include "../Common/General/globalfunctions.h"
#include "../Common/Network/messagecommand.h"
#include "../General/globaldata.h"
#include "General/pushnotification.h"
#include "ccontcpmaindata.h"

extern GlobalData* g_GlobalData;

cConTcpMainData g_ConTcpMainData;

cConTcpMainData::cConTcpMainData()
{
}

qint32 cConTcpMainData::initialize(ListedUser* pListedUser)
{
    this->m_pListedUser = pListedUser;
    this->m_initialized = true;

    return ERROR_CODE_SUCCESS;
}

MessageProtocol* cConTcpMainData::getNewUserAcknowledge(const QString userName, const QHostAddress addr, const cConSslUsage sslUsage)
{
    QJsonObject rootObj;
    qint32      userIndex = this->m_pListedUser->getItemIndex(userName);
    if (userIndex > 0) {

        qInfo().noquote() << QString("Connected user %1").arg(userName);

        quint16 port = this->getFreeDataPort();
        rootObj.insert("port", port);

        rootObj.insert("salt", this->m_pListedUser->getSalt(userName));

        QString random = createRandomString(10);
        rootObj.insert("random", random);

        rootObj.insert("ack", ERROR_CODE_SUCCESS);

        TcpUserConnection* userCon           = new TcpUserConnection();
        userCon->m_userConData.m_dstDataPort = port;
        userCon->m_userConData.m_userName    = userName;
        userCon->m_userConData.m_userID      = userIndex;
        userCon->m_userConData.m_sender      = addr;
        userCon->m_userConData.m_randomLogin = random;
        userCon->m_pDataServer               = new cConTcpDataServer();
        userCon->m_pDataServer->initialize(&userCon->m_userConData, sslUsage);
        this->connect(userCon->m_pDataServer, &cConTcpDataServer::signalServerClosed, this, &cConTcpMainData::slotServerClosed);

        userCon->m_pctrlTcpDataServer = new BackgroundController();
        userCon->m_pctrlTcpDataServer->Start(userCon->m_pDataServer, false);

        QMutexLocker lock(&this->m_mutex);

        this->m_lTcpUserCons.append(userCon);

    } else {
        rootObj.insert("ack", ERROR_CODE_NO_USER);
        qInfo().noquote() << QString("Wrong user tried to connect: \"%1\"").arg(userName);
    }

    QByteArray data = QJsonDocument(rootObj).toJson(QJsonDocument::Compact);

    return new MessageProtocol(OP_CODE_CMD_RES::ACK_CONNECT_USER, data);
}

MessageProtocol* cConTcpMainData::getUserCheckLogin(UserConData* pUserCon, MessageProtocol* request)
{
    QByteArray  data    = QByteArray(request->getPointerToData());
    QJsonObject rootObj = QJsonDocument::fromJson(data).object();
    QString     passW   = rootObj.value("passWord").toString();

    qint32 result = this->m_pListedUser->userCheckPasswordHash(pUserCon->m_userName, passW, pUserCon->m_randomLogin);
    if (result == ERROR_CODE_SUCCESS) {
        pUserCon->m_bIsConnected = true;
        qInfo().noquote() << QString("User %1 logged in").arg(pUserCon->m_userName);
    } else {
        pUserCon->m_bIsConnected = false;
        qWarning().noquote() << QString("User %1 tried to login with wrong password").arg(pUserCon->m_userName);
    }

    return new MessageProtocol(OP_CODE_CMD_RES::ACK_LOGIN_USER, result);
}

MessageProtocol* cConTcpMainData::getUserCheckVersion(UserConData* pUserCon, MessageProtocol* request)
{
    QByteArray  data    = QByteArray(request->getPointerToData());
    QJsonObject rootObj = QJsonDocument::fromJson(data).object();

    quint32 intVersion = (quint32)rootObj.value("version").toDouble();
    QString remVersion = rootObj.value("sVersion").toString();
    qInfo().noquote() << QString("Version from %1 = %2:0x%3").arg(pUserCon->m_userName, remVersion).arg(intVersion, 8, 16);


    QJsonObject rootAns;
    rootAns.insert("ack", ERROR_CODE_SUCCESS);

//#define VERSION_TEST
#ifdef VERSION_TEST
#define ORGA_VERSION_I 0x0B0A0000 // VX.Y.Z => 0xXXYYZZZZ
#define ORGA_VERSION_S "VB.A.0"
    rootAns.insert("version", (qint32)ORGA_VERSION_I);
    rootAns.insert("sVersion", ORGA_VERSION_S);
#else
    rootAns.insert("version", (double)STAM_ORGA_VERSION_I);
    rootAns.insert("sVersion", STAM_ORGA_VERSION_S);
#endif

    pUserCon->m_version = remVersion;
    if (!pUserCon->m_guid.isEmpty())
        g_pushNotify->addNewVersionInformation(pUserCon->m_guid, remVersion);

    QByteArray answer = QJsonDocument(rootAns).toJson(QJsonDocument::Compact);

    return new MessageProtocol(OP_CODE_CMD_RES::ACK_GET_VERSION, answer);
}

void cConTcpMainData::slotServerClosed(quint16 destPort)
{
    QMutexLocker lock(&this->m_mutex);

    for (int i = 0; i < this->m_lTcpUserCons.count(); i++) {
        TcpUserConnection* pCon = this->m_lTcpUserCons.at(i);
        if (pCon->m_userConData.m_dstDataPort == destPort) {
            pCon->m_pDataServer->terminate();
            pCon->m_pctrlTcpDataServer->Stop(true);
            this->m_lTcpUserCons.removeAt(i);
            break;
        }
    }
}

MessageProtocol* cConTcpMainData::getUserProperties(UserConData* pUserCon, MessageProtocol* request)
{
    QByteArray  data(request->getPointerToData());
    QJsonObject rootObj = QJsonDocument::fromJson(data).object();
    QString     guid    = rootObj.value("guid").toString();
    QString     token   = rootObj.value("token").toString();
    qint32      os      = rootObj.value("os").toInt();
    bool        loadAll = rootObj.value("loadAll").toBool();

    pUserCon->m_guid = guid;
    g_pushNotify->addNewAppInformation(guid, token, os, pUserCon->m_userID);

    if (!pUserCon->m_version.isEmpty())
        g_pushNotify->addNewVersionInformation(guid, pUserCon->m_version);

    QJsonObject rootObjAns;
    rootObjAns.insert("ack", ERROR_CODE_SUCCESS);
    rootObjAns.insert("property", (double)this->m_pListedUser->getUserProperties(pUserCon->m_userName));
    rootObjAns.insert("index", pUserCon->m_userID);
    rootObjAns.insert("readableName", this->m_pListedUser->getReadableName(pUserCon->m_userID));
    rootObjAns.insert("loadAll", loadAll);

    if (loadAll) {
        QJsonArray arrTickets;
        if (g_GlobalData->requestGetAvailableTicketFromUser(pUserCon->m_userID, arrTickets) == ERROR_CODE_SUCCESS)
            rootObjAns.insert("tickets", arrTickets);

        QJsonArray eventsArr;
        if (g_GlobalData->getCurrentUserEvents(eventsArr, pUserCon->m_userID) == ERROR_CODE_SUCCESS)
            rootObjAns.insert("events", eventsArr);
    }

    QByteArray answer = QJsonDocument(rootObjAns).toJson(QJsonDocument::Compact);

    qInfo().noquote() << QString("User %1 getting user properties").arg(pUserCon->m_userName);

    return new MessageProtocol(OP_CODE_CMD_RES::ACK_GET_USER_PROPS, answer);
}

MessageProtocol* cConTcpMainData::getUserChangeReadableName(UserConData* pUserCon, MessageProtocol* request)
{
    QByteArray  data(request->getPointerToData());
    QJsonObject rootObj      = QJsonDocument::fromJson(data).object();
    QString     readableName = rootObj.value("reableName").toString();

    if (!this->m_pListedUser->userChangeReadName(pUserCon->m_userName, readableName))
        return new MessageProtocol(OP_CODE_CMD_RES::ACK_USER_CHANGE_READNAME, ERROR_CODE_COMMON);

    return new MessageProtocol(OP_CODE_CMD_RES::ACK_USER_CHANGE_READNAME, ERROR_CODE_SUCCESS);
}

MessageProtocol* cConTcpMainData::getUserChangePassword(UserConData* pUserCon, MessageProtocol* request)
{
    QByteArray  data(request->getPointerToData());
    QJsonObject rootObj         = QJsonDocument::fromJson(data).object();
    QString     newPassword     = rootObj.value("new").toString();
    QString     currentPassword = rootObj.value("current").toString();

    bool rValue;
    rValue = this->m_pListedUser->userCheckPasswordHash(pUserCon->m_userName, currentPassword, pUserCon->m_randomLogin);
    if (!rValue)
        return new MessageProtocol(OP_CODE_CMD_RES::ACK_USER_CHANGE_LOGIN, ERROR_CODE_WRONG_PASSWORD);

    rValue = this->m_pListedUser->userChangePasswordHash(pUserCon->m_userName, newPassword);
    if (rValue)
        return new MessageProtocol(OP_CODE_CMD_RES::ACK_USER_CHANGE_LOGIN, ERROR_CODE_SUCCESS);
    else
        return new MessageProtocol(OP_CODE_CMD_RES::ACK_USER_CHANGE_LOGIN, ERROR_CODE_COMMON);
}

MessageProtocol* cConTcpMainData::getSetUserEvent(UserConData* pUserCon, MessageProtocol* request)
{
    QByteArray  data(request->getPointerToData());
    QJsonObject rootObj = QJsonDocument::fromJson(data).object();
    qint64      eventID = (qint64)rootObj.value("eventID").toDouble();
    qint32      status  = rootObj.value("status").toInt();

    qint32 rCode = g_GlobalData->acceptUserEvent(eventID, pUserCon->m_userID, status);

    QJsonObject rootObjAns;
    rootObjAns.insert("ack", rCode);

    QByteArray answer = QJsonDocument(rootObjAns).toJson(QJsonDocument::Compact);

    return new MessageProtocol(OP_CODE_CMD_RES::ACK_SET_USER_EVENTS, answer);
}

quint16 cConTcpMainData::getFreeDataPort()
{
    QMutexLocker lock(&this->m_mutex);

    quint16 retPort = TCP_PORT + 5;
    do {
        bool bAlreadyUsed = false;
        foreach (TcpUserConnection* pUsrCon, this->m_lTcpUserCons) {
            if (pUsrCon->m_userConData.m_dstDataPort == retPort) {
                bAlreadyUsed = true;
                break;
            }
        }
        if (!bAlreadyUsed)
            return retPort;
        retPort++;
    } while (retPort < TCP_PORT + 1000);

    return 0;
}
