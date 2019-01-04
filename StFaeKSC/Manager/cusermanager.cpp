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
#include "../General/pushnotification.h"
#include "cusermanager.h"

extern GlobalData* g_GlobalData;

cUserManager g_UserManager;

cUserManager::cUserManager(QObject* parent)
    : cGenDisposer(parent)
{
}


qint32 cUserManager::initialize()
{
    this->m_initialized = true;

    return ERROR_CODE_SUCCESS;
}

MessageProtocol* cUserManager::getUserCheckLogin(UserConData* pUserCon, MessageProtocol* request)
{
    QByteArray  data    = QByteArray(request->getPointerToData());
    QJsonObject rootObj = QJsonDocument::fromJson(data).object();
    QString     passW   = rootObj.value("passWord").toString();

    qint32 result = g_GlobalData->m_UserList.userCheckPasswordHash(pUserCon->m_userName, passW, pUserCon->m_randomLogin);
    if (result == ERROR_CODE_SUCCESS) {
        pUserCon->m_bIsConnected = true;
        qInfo().noquote() << QString("User %1 logged in").arg(pUserCon->m_userName);
    } else {
        pUserCon->m_bIsConnected = false;
        qWarning().noquote() << QString("User %1 tried to login with wrong password").arg(pUserCon->m_userName);
    }

    return new MessageProtocol(OP_CODE_CMD_RES::ACK_LOGIN_USER, result);
}

MessageProtocol* cUserManager::getUserCheckVersion(UserConData* pUserCon, MessageProtocol* request)
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

MessageProtocol* cUserManager::getUserProperties(UserConData* pUserCon, MessageProtocol* request)
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
    rootObjAns.insert("property", (double)g_GlobalData->m_UserList.getUserProperties(pUserCon->m_userID));
    rootObjAns.insert("index", pUserCon->m_userID);
    rootObjAns.insert("readableName", g_GlobalData->m_UserList.getReadableName(pUserCon->m_userID));
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

MessageProtocol* cUserManager::getUserChangeReadableName(UserConData* pUserCon, MessageProtocol* request)
{
    QByteArray  data(request->getPointerToData());
    QJsonObject rootObj      = QJsonDocument::fromJson(data).object();
    QString     readableName = rootObj.value("reableName").toString();

    if (!g_GlobalData->m_UserList.userChangeReadName(pUserCon->m_userID, readableName))
        return new MessageProtocol(OP_CODE_CMD_RES::ACK_USER_CHANGE_READNAME, ERROR_CODE_COMMON);

    return new MessageProtocol(OP_CODE_CMD_RES::ACK_USER_CHANGE_READNAME, ERROR_CODE_SUCCESS);
}

MessageProtocol* cUserManager::getUserChangePassword(UserConData* pUserCon, MessageProtocol* request)
{
    QByteArray  data(request->getPointerToData());
    QJsonObject rootObj         = QJsonDocument::fromJson(data).object();
    QString     newPassword     = rootObj.value("new").toString();
    QString     currentPassword = rootObj.value("current").toString();

    bool rValue;
    rValue = g_GlobalData->m_UserList.userCheckPasswordHash(pUserCon->m_userName, currentPassword, pUserCon->m_randomLogin);
    if (!rValue)
        return new MessageProtocol(OP_CODE_CMD_RES::ACK_USER_CHANGE_LOGIN, ERROR_CODE_WRONG_PASSWORD);

    rValue = g_GlobalData->m_UserList.userChangePasswordHash(pUserCon->m_userName, newPassword);
    if (rValue)
        return new MessageProtocol(OP_CODE_CMD_RES::ACK_USER_CHANGE_LOGIN, ERROR_CODE_SUCCESS);
    else
        return new MessageProtocol(OP_CODE_CMD_RES::ACK_USER_CHANGE_LOGIN, ERROR_CODE_COMMON);
}

MessageProtocol* cUserManager::getSetUserEvent(UserConData* pUserCon, MessageProtocol* request)
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

MessageProtocol* cUserManager::getUserCommandResponse(UserConData* pUserCon, MessageProtocol* request)
{
    QByteArray  data(request->getPointerToData());
    QJsonObject rootObj = QJsonDocument::fromJson(data).object();

    QString cmd    = rootObj.value("cmd").toString();
    QString type   = rootObj.value("type").toString();
    qint32  result = ERROR_CODE_SUCCESS;

    QJsonObject rootAns;
    if (cmd == "changeReadableName") {
        QString readableName = rootObj.value("readableName").toString();

        if (!g_GlobalData->m_UserList.userChangeReadName(pUserCon->m_userID, readableName))
            result = ERROR_CODE_COMMON;
        else
            result = ERROR_CODE_SUCCESS;
    } else if (cmd == "list") {
        qint32 numbOfUsers = g_GlobalData->m_UserList.getNumberOfInternalList();

        QJsonArray arrUsers;
        for (qint32 i = 0; i < numbOfUsers; i++) {
            UserStats* pUser = (UserStats*)g_GlobalData->m_UserList.getRequestConfigItemFromListIndex(i);
            if (pUser == NULL)
                continue;
            QJsonObject userObj;
            userObj.insert("user", pUser->m_itemName);
            userObj.insert("index", pUser->m_index);
            userObj.insert("readName", pUser->m_readName);
            userObj.insert("timestamp", pUser->m_timestamp);
            userObj.insert("online", pUser->m_lastOnline);
            userObj.insert("props", (double)g_GlobalData->m_UserList.getUserProperties(pUser->m_index));

            arrUsers.append(userObj);
        }
        rootAns.insert("timestamp", g_GlobalData->m_UserList.getLastUpdateTime());
        rootAns.insert("user", arrUsers);
        result = ERROR_CODE_SUCCESS;
    }

    rootAns.insert("ack", result);
    rootAns.insert("cmd", cmd);
    rootAns.insert("type", type);

    QByteArray answer = QJsonDocument(rootAns).toJson(QJsonDocument::Compact);

    Q_UNUSED(pUserCon);

    return new MessageProtocol(OP_CODE_CMD_RES::ACK_USER_COMMAND, answer);
}
