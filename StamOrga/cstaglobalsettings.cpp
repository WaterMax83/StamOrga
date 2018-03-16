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
#include "../cstasettingsmanager.h"
#include "Connection/cconmanager.h"
#include "cstaglobalsettings.h"

cStaGlobalSettings g_StaGlobalSettings;

// clang-format off

#define SETTINGS_GROUP      "GLOBAL_SETTINGS"

#define SETT_ALREADY_CONNECTED     "AlreadyConnected"
//#define USER_USERNAME   "UserName"
//#define USER_PASSWORD   "Password"
//#define USER_SALT       "Salt"
//#define USER_READABLE   "ReadableName"

// clang-format on


cStaGlobalSettings::cStaGlobalSettings(QObject* parent)
    : cGenDisposer(parent)
{
    this->m_initialized = false;
}


qint32 cStaGlobalSettings::initialize()
{
    //    QString value;
    bool bValue;

    g_StaSettingsManager.getBoolValue(SETTINGS_GROUP, SETT_ALREADY_CONNECTED, bValue);
    this->m_bAlreadyConnected = bValue;
    //    g_StaSettingsManager.getValue(SETTINGS_GROUP, USER_USERNAME, value);
    //    this->m_userName = value;
    //    g_StaSettingsManager.getValue(SETTINGS_GROUP, USER_PASSWORD, value);
    //    this->m_passWord = value;
    //    g_StaSettingsManager.getValue(SETTINGS_GROUP, USER_SALT, value);
    //    this->m_salt = value;
    //    g_StaSettingsManager.getValue(SETTINGS_GROUP, USER_READABLE, value);
    //    this->m_readableName = value;

    this->m_initialized = true;

    return ERROR_CODE_SUCCESS;
}

qint32 cStaGlobalSettings::startGettingVersionInfo()
{
    QJsonObject rootObj;
    rootObj.insert("version", (double)STAM_ORGA_VERSION_I);
    QString sVersion = STAM_ORGA_VERSION_S;
#ifdef Q_OS_WIN
    sVersion.append("_Win");
#elif defined(Q_OS_ANDROID)
    sVersion.append("_Android");
#elif defined(Q_OS_IOS)
    sVersion.append(("_iOS");
#endif
    rootObj.insert("sVersion", sVersion);

    TcpDataConRequest* req = new TcpDataConRequest(OP_CODE_CMD_REQ::REQ_GET_VERSION);
    req->m_lData           = QJsonDocument(rootObj).toJson(QJsonDocument::Compact);

    g_ConManager.sendNewRequest(req);
    return ERROR_CODE_SUCCESS;
}

qint32 cStaGlobalSettings::handleVersionResponse(MessageProtocol* msg)
{
    QByteArray  data    = QByteArray(msg->getPointerToData());
    QJsonObject rootObj = QJsonDocument::fromJson(data).object();

    qint32  result        = rootObj.value("ack").toInt(ERROR_CODE_NOT_FOUND);
    quint32 uVersion      = (quint32)rootObj.value("version").toDouble();
    this->m_remoteVersion = rootObj.value("sVersion").toString();

    qInfo().noquote() << QString("Version from server %1:0x%2").arg(this->m_remoteVersion, QString::number(uVersion, 16));

    this->setAlreadyConnected(true);
    if ((uVersion & 0xFFFFFF00) > (STAM_ORGA_VERSION_I & 0xFFFFFF00)) {
        this->m_versionInfo = QString("Deine Version: %2<br>Aktuelle Version: %1<br><br>").arg(this->m_remoteVersion, STAM_ORGA_VERSION_S);
        this->m_versionInfo.append(QString(STAM_ORGA_VERSION_LINK_WITH_TEXT).arg(this->m_remoteVersion.toLower(), this->m_remoteVersion));
        this->m_updateLink = QString(STAM_ORGA_VERSION_LINK).arg(this->m_remoteVersion.toLower());

        return ERROR_CODE_NEW_VERSION;
    }
    //    version->append(remVersion);
    return result;
}


void cStaGlobalSettings::setAlreadyConnected(const bool con)
{
    if (con != this->m_bAlreadyConnected) {
        this->m_bAlreadyConnected = con;
        //        this->updatePushNotification();

        g_StaSettingsManager.setBoolValue(SETTINGS_GROUP, SETT_ALREADY_CONNECTED, con);
    }
}

QString cStaGlobalSettings::getRemoteVersion()
{
    return this->m_remoteVersion;
}
QString cStaGlobalSettings::getUpdateLink()
{
    return this->m_updateLink;
}
QString cStaGlobalSettings::getVersionInfo()
{
    return this->m_versionInfo;
}
