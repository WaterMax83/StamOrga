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

#include <QtCore/QJsonArray>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>

#include "../Common/General/globalfunctions.h"
#include "../Common/Network/messagecommand.h"
#include "../Connection/cconmanager.h"
#include "../Connection/cconusersettings.h"
#include "../cstaglobalmanager.h"
#include "cdatausermanager.h"

extern cStaGlobalManager* g_GlobalManager;

cDataUserManager* g_DataUserManager;

cDataUserManager::cDataUserManager(QObject* parent)
    : cGenDisposer(parent)
{
}

qint32 cDataUserManager::initialize()
{
    qRegisterMetaType<UserInformation*>("UserInformation*");

    this->m_initialized = true;

    return ERROR_CODE_SUCCESS;
}

qint32 cDataUserManager::getUserInformationLength()
{
    QMutexLocker lock(&this->m_mutex);
    return this->m_lUser.size();
}

UserInformation* cDataUserManager::getUserInfoFromArrayIndex(int index)
{
    QMutexLocker lock(&this->m_mutex);

    if (index < this->m_lUser.size()) {
        return this->m_lUser.at(index);
    }
    return NULL;
}

qint32 cDataUserManager::startUpdateReadableName(QString name)
{
    if (!this->m_initialized)
        return ERROR_CODE_NOT_INITIALIZED;

    QJsonObject rootObj;
    rootObj.insert("type", "user");
    rootObj.insert("cmd", "changeReadableName");
    rootObj.insert("readableName", name);

    TcpDataConRequest* req = new TcpDataConRequest(OP_CODE_CMD_REQ::REQ_USER_COMMAND);
    req->m_lData           = QJsonDocument(rootObj).toJson(QJsonDocument::Compact);

    this->m_newReadableName = name;

    g_ConManager->sendNewRequest(req);
    return ERROR_CODE_SUCCESS;
}

qint32 cDataUserManager::startUpdatePassword(QString password)
{
    if (!this->m_initialized)
        return ERROR_CODE_NOT_INITIALIZED;

    QString newPassWord;
    if (password.length() > 0)
        newPassWord = g_ConUserSettings->createHashValue(password, g_ConUserSettings->getSalt());
    else
        newPassWord = this->m_newPassWord;
    QString     currentPassWord = g_ConUserSettings->createHashValue(g_ConUserSettings->getPassWord(), g_ConUserSettings->getRandomLoginValue());
    QJsonObject rootObj;
    rootObj.insert("type", "user");
    rootObj.insert("cmd", "updatePassword");
    rootObj.insert("new", newPassWord);
    rootObj.insert("current", currentPassWord);

    TcpDataConRequest* req = new TcpDataConRequest(OP_CODE_CMD_REQ::REQ_USER_CHANGE_LOGIN);
    req->m_lData           = QJsonDocument(rootObj).toJson(QJsonDocument::Compact);

    this->m_newPassWord = newPassWord;

    g_ConManager->sendNewRequest(req);
    return ERROR_CODE_SUCCESS;
}


qint32 cDataUserManager::startListUserOverview()
{
    if (!this->m_initialized)
        return ERROR_CODE_NOT_INITIALIZED;

    QJsonObject rootObj;
    rootObj.insert("type", "user");
    rootObj.insert("cmd", "list");

    TcpDataConRequest* req = new TcpDataConRequest(OP_CODE_CMD_REQ::REQ_USER_COMMAND);
    req->m_lData           = QJsonDocument(rootObj).toJson(QJsonDocument::Compact);

    g_ConManager->sendNewRequest(req);
    return ERROR_CODE_SUCCESS;
}

qint32 cDataUserManager::handleUserCommandResponse(MessageProtocol* msg)
{
    if (!this->m_initialized)
        return ERROR_CODE_NOT_INITIALIZED;

    QMutexLocker lock(&this->m_mutex);

    QByteArray  data(msg->getPointerToData());
    QJsonObject rootObj = QJsonDocument::fromJson(data).object();

    qint32  result  = rootObj.value("ack").toInt(ERROR_CODE_NOT_FOUND);
    QString command = rootObj.value("cmd").toString();

    if (result != ERROR_CODE_SUCCESS)
        return result;

    if (command == "changeReadableName") {
        g_ConUserSettings->setReadableName(this->m_newReadableName);

        this->m_newReadableName.clear();
    } else if (command == "list") {

        for (int i = 0; i < this->m_lUser.size(); i++)
            delete this->m_lUser[i];
        this->m_lUser.clear();

        QJsonArray arrUser = rootObj.value("user").toArray();

        //        this->m_stLastLocalUpdateTimeStamp = QDateTime::currentMSecsSinceEpoch();

        qint32 ownIndex = g_ConUserSettings->getUserIndex();

        for (int i = 0; i < arrUser.count(); i++) {
            QJsonObject      userObj = arrUser.at(i).toObject();
            UserInformation* pUser   = new UserInformation();

            pUser->m_index      = userObj.value("index").toInt();
            pUser->m_timestamp  = (qint64)userObj.value("timestamp").toDouble();
            pUser->m_lastOnline = (qint64)userObj.value("online").toDouble();
            pUser->m_user       = userObj.value("user").toString();
            pUser->m_readName   = userObj.value("readName").toString();
            if (pUser->m_readName.isEmpty())
                pUser->m_readName = "NoName";
            pUser->m_prop = (quint32)userObj.value("props").toDouble();
            if (pUser->m_index == ownIndex)
                pUser->m_owner = true;
            else
                pUser->m_owner = false;
            pUser->m_admin = USER_IS_ENABLED(pUser->m_prop, USER_ENABLE_ADMIN);
            if (pUser->m_admin)
                pUser->m_userType = "Admin";
            else
                pUser->m_userType = "Benutzer";

            g_GlobalManager->setQMLObjectOwnershipToCpp(pUser);
            this->m_lUser.append(pUser);
        }

        std::sort(this->m_lUser.begin(), this->m_lUser.end(), UserInformation::compareTimeStampFunctionAscending);

        //        this->m_stLastServerUpdateTimeStamp = timestamp;
    } else
        result = ERROR_CODE_NOT_IMPLEMENTED;

    return result;
}

qint32 cDataUserManager::handleUpdatePasswordResponse(MessageProtocol* msg)
{
    qint32 rValue = msg->getIntData();
    if (rValue == ERROR_CODE_SUCCESS)
        g_ConUserSettings->setPassWord(this->m_newPassWord);

    this->m_newPassWord.clear();

    return rValue;
}