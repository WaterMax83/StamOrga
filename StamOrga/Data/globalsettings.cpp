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

#include <QtGui/QFontDatabase>

#include "../../Common/General/config.h"
#include "../../Common/General/globaltiming.h"
#include "globalsettings.h"
#include "source/pushnotification.h"

// clang-format off
#define SETT_USE_READABLE_NAME      "UseReadableName"
#define SETT_DEBUG_IP               "DebugIP"
#define SETT_DEBUG_IP_WIFI          "DebugIPWifi"
#define SETT_LAST_SHOWN_VERSION     "LastShownVersion"
#define SETT_LOAD_GAME_INFO         "LoadGameInfo"
#define SETT_SAVE_INFOS_ON_APP      "SaveInfosOnApp"
#define SETT_CHANGE_DEFAULT_FONT    "ChangeDefaultFont"
#define SETT_ENABLE_NOTIFICATION    "EnableNotification"

#define SETT_ALREADY_CONNECTED      "AlreadyConnected"
// clang-format on

GlobalSettings::GlobalSettings(QObject* parent)
    : QObject(parent)
{
    this->m_lastGameInfoUpdate = 0;
}

void GlobalSettings::initialize(GlobalData* pGlobalData, QGuiApplication* app)
{
    this->m_pGlobalData = pGlobalData;

    this->m_pGlobalData->m_pMainUserSettings->beginGroup("GLOBAL_SETTINGS");

    this->setUseReadableName(this->m_pGlobalData->m_pMainUserSettings->value(SETT_USE_READABLE_NAME, true).toBool());
    this->setLoadGameInfo(this->m_pGlobalData->m_pMainUserSettings->value(SETT_LOAD_GAME_INFO, true).toBool());
    this->setSaveInfosOnApp(this->m_pGlobalData->m_pMainUserSettings->value(SETT_SAVE_INFOS_ON_APP, true).toBool());
    this->setDebugIP(this->m_pGlobalData->m_pMainUserSettings->value(SETT_DEBUG_IP, "").toString());
    this->setDebugIPWifi(this->m_pGlobalData->m_pMainUserSettings->value(SETT_DEBUG_IP_WIFI, "").toString());
    this->m_lastShownVersion = this->m_pGlobalData->m_pMainUserSettings->value(SETT_LAST_SHOWN_VERSION, "").toString();
    this->setChangeDefaultFont(this->m_pGlobalData->m_pMainUserSettings->value(SETT_CHANGE_DEFAULT_FONT, "Default").toString());
    this->m_notificationEnabledValue = this->m_pGlobalData->m_pMainUserSettings->value(SETT_ENABLE_NOTIFICATION, 0x00000000FFFFFFFF).toULongLong();

    this->m_alreadyConnected = this->m_pGlobalData->m_pMainUserSettings->value(SETT_ALREADY_CONNECTED, false).toBool();

    this->m_pGlobalData->m_pMainUserSettings->endGroup();

    connect(app, &QGuiApplication::applicationStateChanged, this, &GlobalSettings::stateFromAppChanged);
}


void GlobalSettings::saveGlobalSettings()
{
    QMutexLocker lock(&this->m_pGlobalData->m_mutexUser);

    this->m_pGlobalData->m_pMainUserSettings->beginGroup("GLOBAL_SETTINGS");

    this->m_pGlobalData->m_pMainUserSettings->setValue(SETT_USE_READABLE_NAME, this->m_useReadableName);
    this->m_pGlobalData->m_pMainUserSettings->setValue(SETT_LOAD_GAME_INFO, this->m_loadGameInfo);
    this->m_pGlobalData->m_pMainUserSettings->setValue(SETT_SAVE_INFOS_ON_APP, this->m_saveInfosOnApp);
    this->m_pGlobalData->m_pMainUserSettings->setValue(SETT_DEBUG_IP, this->m_debugIP);
    this->m_pGlobalData->m_pMainUserSettings->setValue(SETT_DEBUG_IP_WIFI, this->m_debugIPWifi);
    this->m_pGlobalData->m_pMainUserSettings->setValue(SETT_LAST_SHOWN_VERSION, this->m_lastShownVersion);
    this->m_pGlobalData->m_pMainUserSettings->setValue(SETT_CHANGE_DEFAULT_FONT, this->m_changeDefaultFont);
    this->m_pGlobalData->m_pMainUserSettings->setValue(SETT_ENABLE_NOTIFICATION, this->m_notificationEnabledValue);

    this->m_pGlobalData->m_pMainUserSettings->endGroup();

    this->m_pGlobalData->m_pMainUserSettings->sync();

    this->updatePushNotification();
}

bool GlobalSettings::updateConnectionStatus(bool connected)
{
    if (connected != this->m_alreadyConnected) {
        this->m_alreadyConnected = connected;
        this->updatePushNotification();
        QMutexLocker lock(&this->m_pGlobalData->m_mutexUser);

        this->m_pGlobalData->m_pMainUserSettings->beginGroup("GLOBAL_SETTINGS");

        this->m_pGlobalData->m_pMainUserSettings->setValue(SETT_ALREADY_CONNECTED, this->m_alreadyConnected);

        this->m_pGlobalData->m_pMainUserSettings->endGroup();
        return true;
    }
    return false;
}

void GlobalSettings::updatePushNotification()
{
    if (this->m_alreadyConnected && this->isNotificationNewAppVersionEnabled())
        PushNotificationInformationHandler::subscribeToTopic(NOTIFY_TOPIC_NEW_APP_VERSION);
    else
        PushNotificationInformationHandler::unSubscribeFromTopic(NOTIFY_TOPIC_NEW_APP_VERSION);

    if (this->m_alreadyConnected && this->isNotificationNewMeetingEnabled())
        PushNotificationInformationHandler::subscribeToTopic(NOTIFY_TOPIC_NEW_MEETING);
    else
        PushNotificationInformationHandler::unSubscribeFromTopic(NOTIFY_TOPIC_NEW_MEETING);

    if (this->m_alreadyConnected && this->isNotificationChangedMeetingEnabled())
        PushNotificationInformationHandler::subscribeToTopic(NOTIFY_TOPIC_CHANGE_MEETING);
    else
        PushNotificationInformationHandler::unSubscribeFromTopic(NOTIFY_TOPIC_CHANGE_MEETING);

    if (this->m_alreadyConnected && this->isNotificationNewFreeTicketEnabled())
        PushNotificationInformationHandler::subscribeToTopic(NOTIFY_TOPIC_NEW_FREE_TICKET);
    else
        PushNotificationInformationHandler::unSubscribeFromTopic(NOTIFY_TOPIC_NEW_FREE_TICKET);

    if (this->m_alreadyConnected && this->isNotificationNewAwayAcceptEnabled())
        PushNotificationInformationHandler::subscribeToTopic(NOTIFY_TOPIC_NEW_AWAY_ACCEPT);
    else
        PushNotificationInformationHandler::unSubscribeFromTopic(NOTIFY_TOPIC_NEW_AWAY_ACCEPT);

    if (this->m_alreadyConnected && this->isNotificationFanclubNewsEnabled()) {
        if (this->m_pGlobalData->userIsFanclubEnabled())
            PushNotificationInformationHandler::subscribeToTopic(NOTIFY_TOPIC_NEW_FANCLUB_NEWS);
    } else
        PushNotificationInformationHandler::unSubscribeFromTopic(NOTIFY_TOPIC_NEW_FANCLUB_NEWS);

    if (this->m_alreadyConnected) {
        PushNotificationInformationHandler::subscribeToTopic(NOTIFY_TOPIC_GENERAL);
        PushNotificationInformationHandler::subscribeToTopic(NOTIFY_TOPIC_GENERAL_BACKUP);
    } else {
        PushNotificationInformationHandler::unSubscribeFromTopic(NOTIFY_TOPIC_GENERAL);
        PushNotificationInformationHandler::unSubscribeFromTopic(NOTIFY_TOPIC_GENERAL_BACKUP);
    }
}

void GlobalSettings::setChangeDefaultFont(QString font)
{
    this->m_changeDefaultFont = font;
    if (this->m_fontList == NULL)
        return;

    this->m_currentFontIndex = this->m_fontList->indexOf(font);
}

void GlobalSettings::setCurrentFontList(QStringList* list)
{
    this->m_fontList = list;
    if (this->m_fontList == NULL)
        return;

    this->m_currentFontIndex = this->m_fontList->indexOf(this->m_changeDefaultFont);
}

QString GlobalSettings::getCurrentVersion()
{
    return STAM_ORGA_VERSION_S;
}

QString GlobalSettings::getCurrentVersionLink()
{
    return QString(STAM_ORGA_VERSION_LINK).arg(QString(STAM_ORGA_VERSION_S).toLower(), STAM_ORGA_VERSION_S);
}

QString GlobalSettings::getVersionChangeInfo()
{
    QString rValue;

    rValue.append("<b>V1.0.4:</b>(XX.XX.XXXX)<br>");
    rValue.append("- Fanclub Nachrichten (Mitglieder)<br>");
    rValue.append("- neue Notification \"Erster Auswärtsfahrer\" & \"Fanclub Nachricht\"<br>");

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

        QMutexLocker lock(&this->m_pGlobalData->m_mutexUser);

        this->m_pGlobalData->m_pMainUserSettings->beginGroup("GLOBAL_SETTINGS");
        this->m_pGlobalData->m_pMainUserSettings->setValue("LastShownVersion", this->m_lastShownVersion);
        this->m_pGlobalData->m_pMainUserSettings->endGroup();
    }

    return rValue;
}

bool GlobalSettings::isVersionChangeAlreadyShown()
{
    if (this->m_lastShownVersion == STAM_ORGA_VERSION_S)
        return true;

    return false;
}

/* Under Android it takes too long to register for app changed at first start, so call it */
void GlobalSettings::checkNewStateChangedAtStart()
{
    this->stateFromAppChanged(QGuiApplication::applicationState());
}

void GlobalSettings::stateFromAppChanged(Qt::ApplicationState state)
{
    if (state != Qt::ApplicationState::ApplicationActive)
        return;

    if (!this->loadGameInfo())
        return;

    if (this->m_pGlobalData->userName() == "" || this->m_pGlobalData->passWord() == "")
        return;

    qint64 now = QDateTime::currentMSecsSinceEpoch();
    if ((now - this->m_lastGameInfoUpdate) < TIMEOUT_LOAD_GAMEINFO)
        return;

    if ((now - this->m_pGlobalData->m_gpLastLocalUpdateTimeStamp) < TIMEOUT_LOAD_GAMES)
        emit this->sendAppStateChangedToActive(1);
    else
        emit this->sendAppStateChangedToActive(2);

    this->m_lastGameInfoUpdate = now;
}

#define NOT_OFFSET_NEWAPPV 0
#define NOT_OFFSET_NEWMEET 1
#define NOT_OFFSET_CHGGAME 2
#define NOT_OFFSET_NEWTICK 3
#define NOT_OFFSET_NEWAWAY 4
#define NOT_OFFSET_FANCLUB 5

bool GlobalSettings::isNotificationNewAppVersionEnabled()
{
    return (this->m_notificationEnabledValue & (1 << NOT_OFFSET_NEWAPPV)) ? true : false;
}
bool GlobalSettings::isNotificationNewMeetingEnabled()
{
    return (this->m_notificationEnabledValue & (1 << NOT_OFFSET_NEWMEET)) ? true : false;
}
bool GlobalSettings::isNotificationChangedMeetingEnabled()
{
    return (this->m_notificationEnabledValue & (1 << NOT_OFFSET_CHGGAME)) ? true : false;
}
bool GlobalSettings::isNotificationNewFreeTicketEnabled()
{
    return (this->m_notificationEnabledValue & (1 << NOT_OFFSET_NEWTICK)) ? true : false;
}
bool GlobalSettings::isNotificationNewAwayAcceptEnabled()
{
    return (this->m_notificationEnabledValue & (1 << NOT_OFFSET_NEWAWAY)) ? true : false;
}
bool GlobalSettings::isNotificationFanclubNewsEnabled()
{
    return (this->m_notificationEnabledValue & (1 << NOT_OFFSET_FANCLUB)) ? true : false;
}

void GlobalSettings::setNotificationNewAppVersionEnabled(bool enable)
{
    this->m_notificationEnabledValue &= ~(1 << NOT_OFFSET_NEWAPPV);
    this->m_notificationEnabledValue |= (enable ? 1 : 0) << NOT_OFFSET_NEWAPPV;
    //    if (enable)
    //        PushNotificationInformationHandler::subscribeToTopic(NOTIFY_TOPIC_NEW_APP_VERSION);
    //    else
    //        PushNotificationInformationHandler::unSubscribeFromTopic(NOTIFY_TOPIC_NEW_APP_VERSION);
}
void GlobalSettings::setNotificationNewMeetingEnabled(bool enable)
{
    this->m_notificationEnabledValue &= ~(1 << NOT_OFFSET_NEWMEET);
    this->m_notificationEnabledValue |= (enable ? 1 : 0) << NOT_OFFSET_NEWMEET;
    //    if (enable)
    //        PushNotificationInformationHandler::subscribeToTopic(NOTIFY_TOPIC_NEW_MEETING);
    //    else
    //        PushNotificationInformationHandler::unSubscribeFromTopic(NOTIFY_TOPIC_NEW_MEETING);
}
void GlobalSettings::setNotificationChangedMeetingEnabled(bool enable)
{
    this->m_notificationEnabledValue &= ~(1 << NOT_OFFSET_CHGGAME);
    this->m_notificationEnabledValue |= (enable ? 1 : 0) << NOT_OFFSET_CHGGAME;
    //    if (enable)
    //        PushNotificationInformationHandler::subscribeToTopic(NOTIFY_TOPIC_CHANGE_MEETING);
    //    else
    //        PushNotificationInformationHandler::unSubscribeFromTopic(NOTIFY_TOPIC_CHANGE_MEETING);
}
void GlobalSettings::setNotificationNewFreeTicketEnabled(bool enable)
{
    this->m_notificationEnabledValue &= ~(1 << NOT_OFFSET_NEWTICK);
    this->m_notificationEnabledValue |= (enable ? 1 : 0) << NOT_OFFSET_NEWTICK;
    //    if (enable)
    //        PushNotificationInformationHandler::subscribeToTopic(NOTIFY_TOPIC_NEW_FREE_TICKET);
    //    else
    //        PushNotificationInformationHandler::unSubscribeFromTopic(NOTIFY_TOPIC_NEW_FREE_TICKET);
}
void GlobalSettings::setNotificationNewAwayAcceptEnabled(bool enable)
{
    this->m_notificationEnabledValue &= ~(1 << NOT_OFFSET_NEWAWAY);
    this->m_notificationEnabledValue |= (enable ? 1 : 0) << NOT_OFFSET_NEWAWAY;
    //    if (enable)
    //        PushNotificationInformationHandler::subscribeToTopic(NOTIFY_TOPIC_NEW_AWAY_ACCEPT);
    //    else
    //        PushNotificationInformationHandler::unSubscribeFromTopic(NOTIFY_TOPIC_NEW_AWAY_ACCEPT);
}
void GlobalSettings::setNotificationFanclubNewsEnabled(bool enable)
{
    this->m_notificationEnabledValue &= ~(1 << NOT_OFFSET_FANCLUB);
    this->m_notificationEnabledValue |= (enable ? 1 : 0) << NOT_OFFSET_FANCLUB;
    //    if (enable)
    //        PushNotificationInformationHandler::subscribeToTopic(NOTIFY_TOPIC_NEW_FANCLUB_NEWS);
    //    else
    //        PushNotificationInformationHandler::unSubscribeFromTopic(NOTIFY_TOPIC_NEW_FANCLUB_NEWS);
}
