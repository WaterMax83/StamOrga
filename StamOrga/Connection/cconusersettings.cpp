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
#include "../Common/Network/messagecommand.h"
#include "../Data/cdatappinfomanager.h"
#include "../Data/cdataticketmanager.h"
#include "../cstasettingsmanager.h"
#include "cconmanager.h"
#include "cconusersettings.h"

// clang-format off

#define USER_GROUP      "USER_LOGIN"

#define USER_IPADDR     "IPAddress"
#define USER_USERNAME   "UserName"
#define USER_PASSWORD   "Password"
#define USER_SALT       "Salt"
#define USER_READABLE   "ReadableName"

// clang-format on


cConUserSettings g_ConUserSettings;

cConUserSettings::cConUserSettings(QObject* parent)
    : cGenDisposer(parent)
{
}

qint32 cConUserSettings::initialize()
{
    QString value;

    g_StaSettingsManager.getValue(USER_GROUP, USER_IPADDR, value);
    this->m_ipAddr = value;
    g_StaSettingsManager.getValue(USER_GROUP, USER_USERNAME, value);
    this->m_userName = value;
    g_StaSettingsManager.getValue(USER_GROUP, USER_PASSWORD, value);
    this->m_passWord = value;
    g_StaSettingsManager.getValue(USER_GROUP, USER_SALT, value);
    this->m_salt = value;
    g_StaSettingsManager.getValue(USER_GROUP, USER_READABLE, value);
    this->m_readableName = value;

#if QT_VERSION < QT_VERSION_CHECK(5, 9, 0)
    this->m_hash = new QCryptographicHash(QCryptographicHash::Sha3_512);
#else
    this->m_hash = new QCryptographicHash(QCryptographicHash::Keccak_512);
#endif


    this->m_initialized = true;

    return ERROR_CODE_SUCCESS;
}

QString cConUserSettings::getIPAddr()
{
    return this->m_ipAddr;
}


void cConUserSettings::setIPAddr(const QString ipaddr)
{
    if (this->m_ipAddr != ipaddr) {
        this->m_ipAddr = ipaddr;
        g_StaSettingsManager.setValue(USER_GROUP, USER_IPADDR, ipaddr);
    }
}

QString cConUserSettings::getUserName()
{
    return this->m_userName;
}

void cConUserSettings::setUserName(const QString name)
{
    if (this->m_userName != name) {
        this->m_userName = name;
        g_StaSettingsManager.setValue(USER_GROUP, USER_USERNAME, name);
    }
}

QString cConUserSettings::getPassWord()
{
    return this->m_passWord;
}

void cConUserSettings::setPassWord(const QString password)
{
    if (this->m_passWord != password) {
        this->m_passWord = password;
        g_StaSettingsManager.setValue(USER_GROUP, USER_PASSWORD, password);
    }
}

QString cConUserSettings::getReadableName()
{
    return this->m_readableName;
}

void cConUserSettings::setReadableName(const QString name)
{
    if (this->m_readableName != name) {
        this->m_readableName = name;
        g_StaSettingsManager.setValue(USER_GROUP, USER_READABLE, name);
    }
}


QString cConUserSettings::getSalt()
{
    return this->m_salt;
}

void cConUserSettings::setSalt(const QString salt)
{
    if (this->m_salt != salt) {
        this->m_salt = salt;
        g_StaSettingsManager.setValue(USER_GROUP, USER_SALT, salt);
    }
}

qint32 cConUserSettings::getUserIndex()
{
    return this->m_userIndex;
}

void cConUserSettings::setUserIndex(const qint32 userIndex)
{
    if (this->m_userIndex != userIndex) {
        this->m_userIndex = userIndex;
#ifdef Q_OS_ANDROID
        Push_NotificationInformationHandler::setUserIndexForTopics(QString::number(userIndex));
#endif
    }
}

quint32 cConUserSettings::getUserProperties()
{
    return this->m_userProperties;
}

void cConUserSettings::setUserProperties(const quint32 userProperties)
{
    //    if (this->m_userProperties != userProperties) {
    this->m_userProperties = userProperties;
    //    }
}

qint32 cConUserSettings::startGettingUserProps()
{
    QJsonObject rootObj;
    rootObj.insert("guid", g_DatAppInfoManager.getCurrentAppGUID());
    rootObj.insert("token", g_DatAppInfoManager.getCurrentAppToken());
#ifdef Q_OS_WIN
    rootObj.insert("os", 1);
#elif defined(Q_OS_ANDROID)
    rootObj.insert("os", 2);
#elif defined(Q_OS_IOS)
    rootObj.insert("os", 3);
#endif

    TcpDataConRequest* req = new TcpDataConRequest(OP_CODE_CMD_REQ::REQ_GET_USER_PROPS);
    req->m_lData           = QJsonDocument(rootObj).toJson(QJsonDocument::Compact);

    g_ConManager.sendNewRequest(req);
    return ERROR_CODE_SUCCESS;
}

qint32 cConUserSettings::handleUserPropsResponse(MessageProtocol* msg)
{
    QByteArray  data(msg->getPointerToData());
    QJsonObject rootObj = QJsonDocument::fromJson(data).object();

    qint32  rValue       = rootObj.value("ack").toInt(ERROR_CODE_MISSING_PARAMETER);
    qint32  index        = rootObj.value("index").toInt(-1);
    quint32 properties   = (quint32)rootObj.value("property").toDouble(0);
    QString readableName = rootObj.value("readableName").toString();

    //    qInfo().noquote() << rootObj.value("tickets");

    this->setUserIndex(index);
    this->setReadableName(readableName);
    if (rValue == ERROR_CODE_SUCCESS) {
        if (this->getUserProperties() != properties) {
            this->setUserProperties(properties);
            qInfo().noquote() << QString("Setting user properties to 0x%1").arg(QString::number(properties, 16));
        }
    } else
        this->setUserProperties(0);


    SeasonTicketItem* seasonTicket;
    int               i = 0;
    while ((seasonTicket = g_DataTicketManager.getSeasonTicketFromArrayIndex(i++)) != NULL) {
        seasonTicket->checkTicketOwn(index);
    }

#ifdef Q_OS_ANDROID
    if (rootObj.contains("tickets")) {
        GameUserData* pGameUserData = this->m_pGlobalData->getGameUserDataHandler();
        pGameUserData->clearTicketGameList();

        QJsonArray arrTickets = rootObj.value("tickets").toArray();
        for (int i = 0; i < arrTickets.count(); i++) {
            QJsonObject ticket    = arrTickets.at(i).toObject();
            qint32      gameIndex = ticket.value("gameIndex").toInt(-1);
            if (ticket.value("type").toString("") == "reserved")
                pGameUserData->setTicketGameIndex(gameIndex, TICKET_STATE_RESERVED);
            else if (ticket.value("type").toString("") == "free")
                pGameUserData->setTicketGameIndex(gameIndex, TICKET_STATE_FREE);
        }
    }
#endif

    return rValue;
}

qint32 cConUserSettings::startUpdateReadableName(QString name)
{
    QJsonObject rootObj;
    rootObj.insert("reableName", name);

    TcpDataConRequest* req = new TcpDataConRequest(OP_CODE_CMD_REQ::REQ_USER_CHANGE_READNAME);
    req->m_lData           = QJsonDocument(rootObj).toJson(QJsonDocument::Compact);

    this->m_newReadableName = name;

    g_ConManager.sendNewRequest(req);
    return ERROR_CODE_SUCCESS;
}

qint32 cConUserSettings::handleUpdateReadableNameResponse(MessageProtocol* msg)
{
    qint32 rValue = msg->getIntData();
    if (rValue == ERROR_CODE_SUCCESS)
        this->setReadableName(this->m_newReadableName);

    this->m_newReadableName.clear();
    return rValue;
}

qint32 cConUserSettings::startUpdatePassword(QString password)
{
    QString newPassWord;
    if (password.length() > 0)
        newPassWord = this->createHashValue(password, this->getSalt());
    else
        newPassWord = this->m_newPassWord;
    QString     currentPassWord = this->createHashValue(this->getPassWord(), this->m_currentRandomValue);
    QJsonObject rootObj;
    rootObj.insert("new", newPassWord);
    rootObj.insert("current", currentPassWord);

    TcpDataConRequest* req = new TcpDataConRequest(OP_CODE_CMD_REQ::REQ_USER_CHANGE_LOGIN);
    req->m_lData           = QJsonDocument(rootObj).toJson(QJsonDocument::Compact);

    this->m_newPassWord = newPassWord;

    g_ConManager.sendNewRequest(req);
    return ERROR_CODE_SUCCESS;
}

qint32 cConUserSettings::handleUpdatePasswordResponse(MessageProtocol* msg)
{
    qint32 rValue = msg->getIntData();
    if (rValue == ERROR_CODE_SUCCESS)
        this->setPassWord(this->m_newPassWord);

    this->m_newPassWord.clear();

    return rValue;
}

QString cConUserSettings::createHashValue(const QString first, const QString second)
{
    this->m_hash->reset();
    QByteArray tmp = first.toUtf8();
    this->m_hash->addData(tmp.constData(), tmp.length());
    tmp = second.toUtf8();
    this->m_hash->addData(tmp.constData(), tmp.length());

    return QString(this->m_hash->result());
}

cConUserSettings::~cConUserSettings()
{
    if (this->m_hash)
        delete this->m_hash;
}
