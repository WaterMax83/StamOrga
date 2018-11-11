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

#include <QtCore/QDateTime>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>

#include "../Common/General/config.h"
#include "../Common/General/globalfunctions.h"
#include "../Common/Network/messagecommand.h"
#include "../Data/cdatagamesmanager.h"
#include "../cstasettingsmanager.h"
#include "Connection/cconmanager.h"
#include "Connection/cconusersettings.h"
#include "cstaglobalsettings.h"
#ifdef STAMORGA_APP
#include "source/cadrpushnotifyinfohandler.h"
#endif

cStaGlobalSettings* g_StaGlobalSettings;

// clang-format off

#define SETTINGS_GROUP      "GLOBAL_SETTINGS"

#define SETT_ALREADY_CONNECTED      "AlreadyConnected"
#define SETT_SAVE_INFO_ON_APP       "SaveInfosOnApp"
#define SETT_LOAD_GAME_INFO         "LoadGameInfo"
#define SETT_USE_VERSION_POPUP      "UseVersionPopup"
#define SETT_CHANGE_DEFAULT_FONT    "ChangeDefaultFont"
#define SETT_DEBUG_IP               "DebugIP"
#define SETT_DEBUG_IP_WIFI          "DebugIPWifi"
#define SETT_ENABLE_NOTIFICATION    "EnableNotification"
#define SETT_KEEP_PAST_VALUE        "KeepPastValue"
#define SETT_USE_SSL                "UseSSL"

//#define USER_READABLE   "ReadableName"

// clang-format on


cStaGlobalSettings::cStaGlobalSettings(QObject* parent)
    : cGenDisposer(parent)
{
    this->m_initialized = false;
}


qint32 cStaGlobalSettings::initialize()
{
    QString value;
    bool    bValue;
    qint64  iValue;

    g_StaSettingsManager->getBoolValue(SETTINGS_GROUP, SETT_ALREADY_CONNECTED, bValue);
    this->m_bAlreadyConnected = bValue;
    g_StaSettingsManager->getBoolValue(SETTINGS_GROUP, SETT_SAVE_INFO_ON_APP, bValue, true);
    this->m_bSaveInfosOnApp = bValue;
    g_StaSettingsManager->getBoolValue(SETTINGS_GROUP, SETT_LOAD_GAME_INFO, bValue, true);
    this->m_bLoadGameInfo = bValue;
    g_StaSettingsManager->getBoolValue(SETTINGS_GROUP, SETT_USE_VERSION_POPUP, bValue, true);
    this->m_bUseVersionPopup = bValue;
    g_StaSettingsManager->getValue(SETTINGS_GROUP, SETT_CHANGE_DEFAULT_FONT, value, "Default");
    this->m_changeDefaultFont = value != "" ? value : "Default";
    g_StaSettingsManager->getValue(SETTINGS_GROUP, SETT_DEBUG_IP, value);
    this->m_debugIP = value;
    g_StaSettingsManager->getValue(SETTINGS_GROUP, SETT_DEBUG_IP_WIFI, value);
    this->m_debugIPWifi = value;
    g_StaSettingsManager->getInt64Value(SETTINGS_GROUP, SETT_ENABLE_NOTIFICATION, iValue, 0xFFFFFFFF);
    this->m_notificationEnabledValue = iValue;
    g_StaSettingsManager->getInt64Value(SETTINGS_GROUP, SETT_KEEP_PAST_VALUE, iValue, 5);
    this->m_iKeepPastItemsCount = iValue;
    g_StaSettingsManager->getBoolValue(SETTINGS_GROUP, SETT_USE_SSL, bValue, false);
    this->m_bUseSSL = bValue;

    this->m_bIpAddressWasSet = false;
    QHostInfo::lookupHost(SERVER_HOST_ADDRESS, this, SLOT(slotCallBackLookUpHost(QHostInfo)));

#ifdef STAMORGA_APP
    connect(qGuiApp, &QGuiApplication::applicationStateChanged, this, &cStaGlobalSettings::slotStateFromAppChanged);
#endif

    this->m_bDisableSSLFrom_Start = true;
    if (this->m_bUseSSL) {
        if (QSslSocket::supportsSsl()) {
            QFile file(":/certs/ca.crt");
            if (file.exists() && file.open(QIODevice::ReadOnly)) {
                qInfo().noquote() << QString("Using ssl with %1 certificate").arg(file.fileName());

                this->m_caCert                = QSslCertificate(file.readAll());
                this->m_bUseSSL               = true;
                this->m_bDisableSSLFrom_Start = false;
                file.close();
            }
        } else {
            this->setUseSSL(false);
            qWarning().noquote() << QString("Disable SSL because necessary ssl libraries are not installed");
        }
    }
    if (QSslSocket::supportsSsl())
        this->m_bCanUseSSL = true;
    else
        this->m_bCanUseSSL = false;

    this->m_initialized = true;

    return ERROR_CODE_SUCCESS;
}


bool cStaGlobalSettings::getSaveInfosOnApp()
{
    return this->m_bSaveInfosOnApp;
}

void cStaGlobalSettings::setSaveInfosOnApp(const bool save)
{
    if (this->m_bSaveInfosOnApp != save) {
        this->m_bSaveInfosOnApp = save;

        g_StaSettingsManager->setBoolValue(SETTINGS_GROUP, SETT_SAVE_INFO_ON_APP, save);
    }
}

bool cStaGlobalSettings::getLoadGameInfos()
{
    return this->m_bLoadGameInfo;
}

void cStaGlobalSettings::setLoadGameInfos(const bool load)
{
    if (this->m_bLoadGameInfo != load) {
        this->m_bLoadGameInfo = load;

        g_StaSettingsManager->setBoolValue(SETTINGS_GROUP, SETT_LOAD_GAME_INFO, load);
    }
}

bool cStaGlobalSettings::getUseVersionPopup()
{
    return this->m_bUseVersionPopup;
}

void cStaGlobalSettings::setUseVersionPopup(const bool use)
{
    if (this->m_bUseVersionPopup != use) {
        this->m_bUseVersionPopup = use;

        g_StaSettingsManager->setBoolValue(SETTINGS_GROUP, SETT_USE_VERSION_POPUP, use);
    }
}

/********************************************* IP *****************************************/
QString cStaGlobalSettings::getDebugIP()
{
    return this->m_debugIP;
}

void cStaGlobalSettings::setDebugIP(const QString ip)
{
    if (this->m_debugIP != ip) {
        this->m_debugIP = ip;

        g_StaSettingsManager->setValue(SETTINGS_GROUP, SETT_DEBUG_IP, ip);
    }
}

QString cStaGlobalSettings::getDebugIPWifi()
{
    return this->m_debugIPWifi;
}

void cStaGlobalSettings::setDebugIPWifi(const QString ip)
{
    if (this->m_debugIPWifi != ip) {
        this->m_debugIPWifi = ip;

        g_StaSettingsManager->setValue(SETTINGS_GROUP, SETT_DEBUG_IP_WIFI, ip);
    }
}

/******************************** FONT ****************************************************/
QString cStaGlobalSettings::getChangeDefaultFont()
{
    return this->m_changeDefaultFont;
}

void cStaGlobalSettings::setChangeDefaultFont(const QString font)
{
    if (this->m_changeDefaultFont != font) {
        this->m_changeDefaultFont = font;

        g_StaSettingsManager->setValue(SETTINGS_GROUP, SETT_CHANGE_DEFAULT_FONT, font);

        if (this->m_fontList != NULL)
            this->m_currentFontIndex = this->m_fontList->indexOf(font);
    }
}

qint32 cStaGlobalSettings::getCurrentFontIndex()
{
    return this->m_currentFontIndex;
}

void cStaGlobalSettings::setCurrentFontList(QStringList* list)
{
    this->m_fontList = list;
    if (this->m_fontList != NULL)
        this->m_currentFontIndex = this->m_fontList->indexOf(this->m_changeDefaultFont);
}

qint64 cStaGlobalSettings::getKeepPastItemsCount()
{
    return this->m_iKeepPastItemsCount;
}

void cStaGlobalSettings::setAlreadyConnected(const bool con)
{
    if (con != this->m_bAlreadyConnected) {
        this->m_bAlreadyConnected = con;
        this->updatePushNotification();

        g_StaSettingsManager->setBoolValue(SETTINGS_GROUP, SETT_ALREADY_CONNECTED, con);
    }
}

bool cStaGlobalSettings::getUseSSL()
{
    return this->m_bUseSSL && !this->m_bDisableSSLFrom_Start;
}

bool cStaGlobalSettings::getUseSSLSettings()
{
    return this->m_bUseSSL;
}

void cStaGlobalSettings::setUseSSL(bool useSSL)
{
    if (useSSL != this->m_bUseSSL) {
        this->m_bUseSSL = useSSL;

        g_StaSettingsManager->setBoolValue(SETTINGS_GROUP, SETT_USE_SSL, useSSL);
    }
}

bool cStaGlobalSettings::getCanUseSSL()
{
    return this->m_bCanUseSSL;
}

QSslCertificate cStaGlobalSettings::getSSLCaCertificate()
{
    return this->m_caCert;
}

#define NOT_OFFSET_NEWAPPV 0
#define NOT_OFFSET_NEWMEET 1
#define NOT_OFFSET_NEWTICK 3
#define NOT_OFFSET_FANCLUB 5
#define NOT_OFFSET_COMMENT 6

bool cStaGlobalSettings::isNotificationNewAppVersionEnabled()
{
    return (this->m_notificationEnabledValue & (1 << NOT_OFFSET_NEWAPPV)) ? true : false;
}
bool cStaGlobalSettings::isNotificationNewMeetingEnabled()
{
    return (this->m_notificationEnabledValue & (1 << NOT_OFFSET_NEWMEET)) ? true : false;
}
bool cStaGlobalSettings::isNotificationNewFreeTicketEnabled()
{
    return (this->m_notificationEnabledValue & (1 << NOT_OFFSET_NEWTICK)) ? true : false;
}
bool cStaGlobalSettings::isNotificationMeetingCommentEnabled()
{
    return (this->m_notificationEnabledValue & (1 << NOT_OFFSET_COMMENT)) ? true : false;
}
bool cStaGlobalSettings::isNotificationFanclubNewsEnabled()
{
    return (this->m_notificationEnabledValue & (1 << NOT_OFFSET_FANCLUB)) ? true : false;
}

void cStaGlobalSettings::setNotificationNewAppVersionEnabled(bool enable)
{
    this->m_notificationEnabledValue &= ~(1 << NOT_OFFSET_NEWAPPV);
    this->m_notificationEnabledValue |= (enable ? 1 : 0) << NOT_OFFSET_NEWAPPV;
    g_StaSettingsManager->setInt64Value(SETTINGS_GROUP, SETT_ENABLE_NOTIFICATION, this->m_notificationEnabledValue);
    this->updatePushNotification();
}
void cStaGlobalSettings::setNotificationNewMeetingEnabled(bool enable)
{
    this->m_notificationEnabledValue &= ~(1 << NOT_OFFSET_NEWMEET);
    this->m_notificationEnabledValue |= (enable ? 1 : 0) << NOT_OFFSET_NEWMEET;
    g_StaSettingsManager->setInt64Value(SETTINGS_GROUP, SETT_ENABLE_NOTIFICATION, this->m_notificationEnabledValue);
    this->updatePushNotification();
}

void cStaGlobalSettings::setNotificationNewFreeTicketEnabled(bool enable)
{
    this->m_notificationEnabledValue &= ~(1 << NOT_OFFSET_NEWTICK);
    this->m_notificationEnabledValue |= (enable ? 1 : 0) << NOT_OFFSET_NEWTICK;
    g_StaSettingsManager->setInt64Value(SETTINGS_GROUP, SETT_ENABLE_NOTIFICATION, this->m_notificationEnabledValue);
    this->updatePushNotification();
}
void cStaGlobalSettings::setNotificationMeetingCommentEnabled(bool enable)
{
    this->m_notificationEnabledValue &= ~(1 << NOT_OFFSET_COMMENT);
    this->m_notificationEnabledValue |= (enable ? 1 : 0) << NOT_OFFSET_COMMENT;
    g_StaSettingsManager->setInt64Value(SETTINGS_GROUP, SETT_ENABLE_NOTIFICATION, this->m_notificationEnabledValue);
    this->updatePushNotification();
}
void cStaGlobalSettings::setNotificationFanclubNewsEnabled(bool enable)
{
    this->m_notificationEnabledValue &= ~(1 << NOT_OFFSET_FANCLUB);
    this->m_notificationEnabledValue |= (enable ? 1 : 0) << NOT_OFFSET_FANCLUB;
    g_StaSettingsManager->setInt64Value(SETTINGS_GROUP, SETT_ENABLE_NOTIFICATION, this->m_notificationEnabledValue);
    this->updatePushNotification();
}


void cStaGlobalSettings::updatePushNotification()
{
#ifdef STAMORGA_APP
    if (this->m_bAlreadyConnected && this->isNotificationNewAppVersionEnabled())
        AdrPushNotifyInfoHandler::subscribeToTopic(NOTIFY_TOPIC_NEW_APP_VERSION);
    else
        AdrPushNotifyInfoHandler::unSubscribeFromTopic(NOTIFY_TOPIC_NEW_APP_VERSION);

    if (this->m_bAlreadyConnected && this->isNotificationNewMeetingEnabled())
        AdrPushNotifyInfoHandler::subscribeToTopic(NOTIFY_TOPIC_NEW_MEETING);
    else
        AdrPushNotifyInfoHandler::unSubscribeFromTopic(NOTIFY_TOPIC_NEW_MEETING);

    AdrPushNotifyInfoHandler::unSubscribeFromTopic(NOTIFY_TOPIC_CHANGE_MEETING);

    if (this->m_bAlreadyConnected && this->isNotificationNewFreeTicketEnabled())
        AdrPushNotifyInfoHandler::subscribeToTopic(NOTIFY_TOPIC_NEW_FREE_TICKET);
    else
        AdrPushNotifyInfoHandler::unSubscribeFromTopic(NOTIFY_TOPIC_NEW_FREE_TICKET);

    AdrPushNotifyInfoHandler::unSubscribeFromTopic(NOTIFY_TOPIC_NEW_AWAY_ACCEPT);

    if (this->m_bAlreadyConnected && this->isNotificationMeetingCommentEnabled())
        AdrPushNotifyInfoHandler::subscribeToTopic(NOTIFY_TOPIC_NEW_COMMENT);
    else
        AdrPushNotifyInfoHandler::unSubscribeFromTopic(NOTIFY_TOPIC_NEW_COMMENT);

    if (this->m_bAlreadyConnected && this->isNotificationFanclubNewsEnabled()) {
        if (g_ConUserSettings->userIsFanclubEnabled())
            AdrPushNotifyInfoHandler::subscribeToTopic(NOTIFY_TOPIC_NEW_FANCLUB_NEWS);
    } else
        AdrPushNotifyInfoHandler::unSubscribeFromTopic(NOTIFY_TOPIC_NEW_FANCLUB_NEWS);

    if (this->m_bAlreadyConnected) {
        AdrPushNotifyInfoHandler::subscribeToTopic(NOTIFY_TOPIC_GENERAL);
        AdrPushNotifyInfoHandler::subscribeToTopic(NOTIFY_TOPIC_GENERAL_BACKUP);
    } else {
        AdrPushNotifyInfoHandler::unSubscribeFromTopic(NOTIFY_TOPIC_GENERAL);
        AdrPushNotifyInfoHandler::unSubscribeFromTopic(NOTIFY_TOPIC_GENERAL_BACKUP);
    }
#endif
}

/* Under Android it takes too long to register for app changed at first start, so call it */
void cStaGlobalSettings::checkNewStateChangedAtStart()
{
    this->slotStateFromAppChanged(QGuiApplication::applicationState());
}

void cStaGlobalSettings::slotStateFromAppChanged(Qt::ApplicationState state)
{
    //    qDebug() << "State Change";

    if (state != Qt::ApplicationState::ApplicationActive)
        return;

    if (!this->getLoadGameInfos())
        return;

    if (g_ConUserSettings->getUserName() == "" || g_ConUserSettings->getPassWord() == "")
        return;

    g_DataGamesManager->stateChangeCheckUdpate();
}

void cStaGlobalSettings::slotCallBackLookUpHost(const QHostInfo& host)
{
    QString newChangedIP;
    if (host.addresses().size() > 0)
        newChangedIP = host.addresses().value(0).toString();

    if (this->getDebugIP() != "")
        newChangedIP = this->getDebugIP();
#ifdef QT_DEBUG
#ifdef Q_OS_ANDROID
//    if (g_GlobalSettings->debugIPWifi() != "") {
//        QNetworkConfigurationManager ncm;
//        QList<QNetworkConfiguration> nc = ncm.allConfigurations();
//        foreach (QNetworkConfiguration item, nc) {
//            if (item.bearerType() == QNetworkConfiguration::BearerWLAN) {
//                if (item.state() == QNetworkConfiguration::StateFlag::Active) {
//                    this->setIpAddr(g_GlobalSettings->debugIPWifi());
//                    newChangedIP = g_GlobalSettings->debugIPWifi();
//                }
//                //                 qDebug() << "Wifi " << item.name();
//                //                 qDebug() << "state " << item.state();
//            }
//        }
//    }
#endif // ANDROID
#endif // DEBUG

    if (newChangedIP.isEmpty())
        newChangedIP = g_ConUserSettings->getIPAddr();

    g_ConUserSettings->setIPAddr(newChangedIP);

    qInfo().noquote() << QString("Setting IP Address: %1").arg(newChangedIP);

    this->m_bIpAddressWasSet = true;
}
