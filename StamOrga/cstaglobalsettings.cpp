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
//#include <QtCore/QDebug>
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
#include "source/cadrpushnotifyinfohandler.h"

cStaGlobalSettings g_StaGlobalSettings;

// clang-format off

#define SETTINGS_GROUP      "GLOBAL_SETTINGS"

#define SETT_ALREADY_CONNECTED      "AlreadyConnected"
#define SETT_SAVE_INFO_ON_APP       "SaveInfosOnApp"
#define SETT_LOAD_GAME_INFO         "LoadGameInfo"
#define SETT_LAST_SHOWN_VERSION     "LastShownVersion"
#define SETT_USE_VERSION_POPUP      "UseVersionPopup"
#define SETT_CHANGE_DEFAULT_FONT    "ChangeDefaultFont"
#define SETT_DEBUG_IP               "DebugIP"
#define SETT_DEBUG_IP_WIFI          "DebugIPWifi"
#define SETT_ENABLE_NOTIFICATION    "EnableNotification"
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

    g_StaSettingsManager.getBoolValue(SETTINGS_GROUP, SETT_ALREADY_CONNECTED, bValue);
    this->m_bAlreadyConnected = bValue;
    g_StaSettingsManager.getBoolValue(SETTINGS_GROUP, SETT_SAVE_INFO_ON_APP, bValue, true);
    this->m_bSaveInfosOnApp = bValue;
    g_StaSettingsManager.getBoolValue(SETTINGS_GROUP, SETT_LOAD_GAME_INFO, bValue, true);
    this->m_bLoadGameInfo = bValue;
    g_StaSettingsManager.getBoolValue(SETTINGS_GROUP, SETT_USE_VERSION_POPUP, bValue, true);
    this->m_bUseVersionPopup = bValue;
    g_StaSettingsManager.getValue(SETTINGS_GROUP, SETT_LAST_SHOWN_VERSION, value);
    this->m_lastShownVersion = value;
    g_StaSettingsManager.getValue(SETTINGS_GROUP, SETT_CHANGE_DEFAULT_FONT, value, "Default");
    this->m_changeDefaultFont = value != "" ? value : "Default";
    g_StaSettingsManager.getValue(SETTINGS_GROUP, SETT_DEBUG_IP, value);
    this->m_debugIP = value;
    g_StaSettingsManager.getValue(SETTINGS_GROUP, SETT_DEBUG_IP_WIFI, value);
    this->m_debugIPWifi = value;
    g_StaSettingsManager.getInt64Value(SETTINGS_GROUP, SETT_ENABLE_NOTIFICATION, iValue, 0xFFFFFFFF);
    this->m_notificationEnabledValue = iValue;

    this->m_bIpAddressWasSet = false;
    QHostInfo::lookupHost("watermax83.ddns.net", this, SLOT(slotCallBackLookUpHost(QHostInfo)));

    connect(qGuiApp, &QGuiApplication::applicationStateChanged, this, &cStaGlobalSettings::slotStateFromAppChanged);

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

    if ((uVersion & 0xFFFFFF00) > (STAM_ORGA_VERSION_I & 0xFFFFFF00)) {
        this->m_versionInfo = QString("Deine Version: %2<br>Aktuelle Version: %1<br><br>").arg(this->m_remoteVersion, STAM_ORGA_VERSION_S);
        this->m_versionInfo.append(QString(STAM_ORGA_VERSION_LINK_WITH_TEXT).arg(this->m_remoteVersion.toLower(), this->m_remoteVersion));
        this->m_updateLink = QString(STAM_ORGA_VERSION_LINK).arg(this->m_remoteVersion.toLower());

        return ERROR_CODE_NEW_VERSION;
    }
    //    version->append(remVersion);
    return result;
}

bool cStaGlobalSettings::getSaveInfosOnApp()
{
    return this->m_bSaveInfosOnApp;
}

void cStaGlobalSettings::setSaveInfosOnApp(const bool save)
{
    if (this->m_bSaveInfosOnApp != save) {
        this->m_bSaveInfosOnApp = save;

        g_StaSettingsManager.setBoolValue(SETTINGS_GROUP, SETT_SAVE_INFO_ON_APP, save);
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

        g_StaSettingsManager.setBoolValue(SETTINGS_GROUP, SETT_LOAD_GAME_INFO, load);
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

        g_StaSettingsManager.setBoolValue(SETTINGS_GROUP, SETT_USE_VERSION_POPUP, use);
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

        g_StaSettingsManager.setValue(SETTINGS_GROUP, SETT_DEBUG_IP, ip);
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

        g_StaSettingsManager.setValue(SETTINGS_GROUP, SETT_DEBUG_IP_WIFI, ip);
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

        g_StaSettingsManager.setValue(SETTINGS_GROUP, SETT_CHANGE_DEFAULT_FONT, font);

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

bool cStaGlobalSettings::isVersionChangeAlreadyShown()
{
    if (this->m_lastShownVersion == STAM_ORGA_VERSION_S)
        return true;

    return false;
}

QString cStaGlobalSettings::getVersionChangeInfo()
{
    QString rValue;

    rValue.append("<b>V1.1.0:</b>(XX.XX.2018)<br>");
    rValue.append("- Umstellung UDP auf TCP<br>");
    rValue.append("- interne Struktur umgebaut<br>");
    rValue.append("- diverse Fehler beseitigt<br>");

    rValue.append("<br><b>V1.0.7:</b>(03.03.2018)<br>");
    rValue.append("- Statistic hinzugefügt<br>");
    rValue.append("- Icons in Drawer und Übersicht überarbeitet<br>");
    rValue.append("- Easteregg versteckt<br>");
    rValue.append("- Infos über eigene Karte und Reservierung in der Übersicht<br>");

    rValue.append("<br><b>V1.0.6:</b>(08.02.2018)<br>");
    rValue.append("- Infos über Fahrt in der Übersicht<br>");
    rValue.append("- Icons überarbeitet<br>");
    rValue.append("- Spiel als Favorit markieren<br>");

    rValue.append("<br><b>V1.0.5:</b>(16.12.2017)<br>");
    rValue.append("- Infos über letzte Neuigkeiten markieren<br>");
    rValue.append("- Framework Version aktualisiert<br>");
    rValue.append("- Fahrt bei Auswärtsspiel hinzugefügt<br>");
    rValue.append("- Versionsupdate über Liste<br>");

    rValue.append("<br><b>V1.0.4:</b>(24.10.2017)<br>");
    rValue.append("- Fanclub Nachrichten (Mitglieder)<br>");
    rValue.append("- neue Benachrichtigung \"Erster Auswärtsfahrer\" & \"Fanclub Nachricht\"<br>");
    rValue.append("- Schrift änderbar (Android)<br>");
    rValue.append("- Verbindungsfehler beim Start behoben<br>");

    rValue.append("<br><b>V1.0.3:</b>(25.08.2017)<br>");
    rValue.append("- Push Notifications (Android)<br>");
    rValue.append("- Schrift änderbar (Windows)<br>");
    rValue.append("- Infos über Ort bei Spieltagstickets<br>");
    rValue.append("- Optische Anpassungen<br>");

    rValue.append("<br><b>V1.0.2:</b>(31.07.2017)<br>");
    rValue.append("- Spielterminierung hinzugefügt<br>");
    rValue.append("- Spielliste in Aktuell/Vergangenheit aufgeteilt<br>");
    rValue.append("- Daten nur nach Bedarf vom Server laden<br>");
    rValue.append("- Fehler beseitigt (Einstellungen/Tickets/etc..)<br>");

    rValue.append("<br><b>V1.0.1:</b>(17.07.2017)<br>");
    rValue.append("- Mehr Informationen in der Spielübersicht<br>");
    rValue.append("- automatisches Laden der Spielinformationen<br>");
    rValue.append("- Dauerkarten editierbar<br>");
    rValue.append("- Passwörter vollständig gehasht<br>");
    rValue.append("- Versionshistorie hinzugefügt<br>");
    rValue.append("- Diverse Fehler beseitigt<br>");

    rValue.append("<br><b>V1.0.0:</b>(30.06.2017)<br>");
    rValue.append("Erste Version<br>");

    if (this->m_lastShownVersion != STAM_ORGA_VERSION_S) {
        this->m_lastShownVersion = STAM_ORGA_VERSION_S;

        g_StaSettingsManager.setValue(SETTINGS_GROUP, SETT_LAST_SHOWN_VERSION, this->m_lastShownVersion);
    }

    return rValue;
}


void cStaGlobalSettings::setAlreadyConnected(const bool con)
{
    if (con != this->m_bAlreadyConnected) {
        this->m_bAlreadyConnected = con;
        this->updatePushNotification();

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


QString cStaGlobalSettings::getCurrentVersion()
{
    return STAM_ORGA_VERSION_S;
}

QString cStaGlobalSettings::getCurrentVersionLink()
{
    return QString(STAM_ORGA_VERSION_LINK_WITH_TEXT).arg(QString(STAM_ORGA_VERSION_S).toLower(), STAM_ORGA_VERSION_S);
}

#define NOT_OFFSET_NEWAPPV 0
#define NOT_OFFSET_NEWMEET 1
#define NOT_OFFSET_CHGGAME 2
#define NOT_OFFSET_NEWTICK 3
#define NOT_OFFSET_NEWAWAY 4
#define NOT_OFFSET_FANCLUB 5

bool cStaGlobalSettings::isNotificationNewAppVersionEnabled()
{
    return (this->m_notificationEnabledValue & (1 << NOT_OFFSET_NEWAPPV)) ? true : false;
}
bool cStaGlobalSettings::isNotificationNewMeetingEnabled()
{
    return (this->m_notificationEnabledValue & (1 << NOT_OFFSET_NEWMEET)) ? true : false;
}
bool cStaGlobalSettings::isNotificationChangedMeetingEnabled()
{
    return (this->m_notificationEnabledValue & (1 << NOT_OFFSET_CHGGAME)) ? true : false;
}
bool cStaGlobalSettings::isNotificationNewFreeTicketEnabled()
{
    return (this->m_notificationEnabledValue & (1 << NOT_OFFSET_NEWTICK)) ? true : false;
}
bool cStaGlobalSettings::isNotificationNewAwayAcceptEnabled()
{
    return (this->m_notificationEnabledValue & (1 << NOT_OFFSET_NEWAWAY)) ? true : false;
}
bool cStaGlobalSettings::isNotificationFanclubNewsEnabled()
{
    return (this->m_notificationEnabledValue & (1 << NOT_OFFSET_FANCLUB)) ? true : false;
}

void cStaGlobalSettings::setNotificationNewAppVersionEnabled(bool enable)
{
    this->m_notificationEnabledValue &= ~(1 << NOT_OFFSET_NEWAPPV);
    this->m_notificationEnabledValue |= (enable ? 1 : 0) << NOT_OFFSET_NEWAPPV;
    g_StaSettingsManager.setInt64Value(SETTINGS_GROUP, SETT_ENABLE_NOTIFICATION, this->m_notificationEnabledValue);
    this->updatePushNotification();
}
void cStaGlobalSettings::setNotificationNewMeetingEnabled(bool enable)
{
    this->m_notificationEnabledValue &= ~(1 << NOT_OFFSET_NEWMEET);
    this->m_notificationEnabledValue |= (enable ? 1 : 0) << NOT_OFFSET_NEWMEET;
    g_StaSettingsManager.setInt64Value(SETTINGS_GROUP, SETT_ENABLE_NOTIFICATION, this->m_notificationEnabledValue);
    this->updatePushNotification();
}
void cStaGlobalSettings::setNotificationChangedMeetingEnabled(bool enable)
{
    this->m_notificationEnabledValue &= ~(1 << NOT_OFFSET_CHGGAME);
    this->m_notificationEnabledValue |= (enable ? 1 : 0) << NOT_OFFSET_CHGGAME;
    g_StaSettingsManager.setInt64Value(SETTINGS_GROUP, SETT_ENABLE_NOTIFICATION, this->m_notificationEnabledValue);
    this->updatePushNotification();
}
void cStaGlobalSettings::setNotificationNewFreeTicketEnabled(bool enable)
{
    this->m_notificationEnabledValue &= ~(1 << NOT_OFFSET_NEWTICK);
    this->m_notificationEnabledValue |= (enable ? 1 : 0) << NOT_OFFSET_NEWTICK;
    g_StaSettingsManager.setInt64Value(SETTINGS_GROUP, SETT_ENABLE_NOTIFICATION, this->m_notificationEnabledValue);
    this->updatePushNotification();
}
void cStaGlobalSettings::setNotificationNewAwayAcceptEnabled(bool enable)
{
    this->m_notificationEnabledValue &= ~(1 << NOT_OFFSET_NEWAWAY);
    this->m_notificationEnabledValue |= (enable ? 1 : 0) << NOT_OFFSET_NEWAWAY;
    g_StaSettingsManager.setInt64Value(SETTINGS_GROUP, SETT_ENABLE_NOTIFICATION, this->m_notificationEnabledValue);
    this->updatePushNotification();
}
void cStaGlobalSettings::setNotificationFanclubNewsEnabled(bool enable)
{
    this->m_notificationEnabledValue &= ~(1 << NOT_OFFSET_FANCLUB);
    this->m_notificationEnabledValue |= (enable ? 1 : 0) << NOT_OFFSET_FANCLUB;
    g_StaSettingsManager.setInt64Value(SETTINGS_GROUP, SETT_ENABLE_NOTIFICATION, this->m_notificationEnabledValue);
    this->updatePushNotification();
}


void cStaGlobalSettings::updatePushNotification()
{
    if (this->m_bAlreadyConnected && this->isNotificationNewAppVersionEnabled())
        AdrPushNotifyInfoHandler::subscribeToTopic(NOTIFY_TOPIC_NEW_APP_VERSION);
    else
        AdrPushNotifyInfoHandler::unSubscribeFromTopic(NOTIFY_TOPIC_NEW_APP_VERSION);

    if (this->m_bAlreadyConnected && this->isNotificationNewMeetingEnabled())
        AdrPushNotifyInfoHandler::subscribeToTopic(NOTIFY_TOPIC_NEW_MEETING);
    else
        AdrPushNotifyInfoHandler::unSubscribeFromTopic(NOTIFY_TOPIC_NEW_MEETING);

    if (this->m_bAlreadyConnected && this->isNotificationChangedMeetingEnabled())
        AdrPushNotifyInfoHandler::subscribeToTopic(NOTIFY_TOPIC_CHANGE_MEETING);
    else
        AdrPushNotifyInfoHandler::unSubscribeFromTopic(NOTIFY_TOPIC_CHANGE_MEETING);

    if (this->m_bAlreadyConnected && this->isNotificationNewFreeTicketEnabled())
        AdrPushNotifyInfoHandler::subscribeToTopic(NOTIFY_TOPIC_NEW_FREE_TICKET);
    else
        AdrPushNotifyInfoHandler::unSubscribeFromTopic(NOTIFY_TOPIC_NEW_FREE_TICKET);

    if (this->m_bAlreadyConnected && this->isNotificationNewAwayAcceptEnabled())
        AdrPushNotifyInfoHandler::subscribeToTopic(NOTIFY_TOPIC_NEW_AWAY_ACCEPT);
    else
        AdrPushNotifyInfoHandler::unSubscribeFromTopic(NOTIFY_TOPIC_NEW_AWAY_ACCEPT);

    if (this->m_bAlreadyConnected && this->isNotificationFanclubNewsEnabled()) {
        if (g_ConUserSettings.userIsFanclubEnabled())
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

    if (g_ConUserSettings.getUserName() == "" || g_ConUserSettings.getPassWord() == "")
        return;

    g_DataGamesManager.stateChangeCheckUdpate();
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
        newChangedIP = g_ConUserSettings.getIPAddr();

    g_ConUserSettings.setIPAddr(newChangedIP);

    qInfo().noquote() << QString("Setting IP Address: %1").arg(newChangedIP);

    this->m_bIpAddressWasSet = true;
}
