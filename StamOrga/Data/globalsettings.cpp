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

#include "globalsettings.h"
#include "../../Common/General/config.h"
#include "../../Common/General/globaltiming.h"

// clang-format off
#define SETT_LOAD_LAST_GAMES        "LastGamesLoadCount"
#define SETT_USE_READABLE_NAME      "UseReadableName"
#define SETT_DEBUG_IP               "DebugIP"
#define SETT_DEBUG_IP_WIFI          "DebugIPWifi"
#define SETT_LAST_SHOWN_VERSION     "LastShownVersion"
#define SETT_LOAD_GAME_INFO         "LoadGameInfo"
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

    this->setLastGamesLoadCount(this->m_pGlobalData->m_pMainUserSettings->value(SETT_LOAD_LAST_GAMES, 5).toInt());
    this->setUseReadableName(this->m_pGlobalData->m_pMainUserSettings->value(SETT_USE_READABLE_NAME, true).toBool());
    this->setLoadGameInfo(this->m_pGlobalData->m_pMainUserSettings->value(SETT_LOAD_GAME_INFO, true).toBool());
    this->setDebugIP(this->m_pGlobalData->m_pMainUserSettings->value(SETT_DEBUG_IP, "").toString());
    this->setDebugIPWifi(this->m_pGlobalData->m_pMainUserSettings->value(SETT_DEBUG_IP_WIFI, "").toString());
    this->m_lastShownVersion = this->m_pGlobalData->m_pMainUserSettings->value(SETT_LAST_SHOWN_VERSION, "").toString();

    this->m_pGlobalData->m_pMainUserSettings->endGroup();

    connect(app, &QGuiApplication::applicationStateChanged, this, &GlobalSettings::stateFromAppChanged);
}


void GlobalSettings::saveGlobalSettings()
{
    QMutexLocker lock(&this->m_pGlobalData->m_mutexUser);

    this->m_pGlobalData->m_pMainUserSettings->beginGroup("GLOBAL_SETTINGS");

    this->m_pGlobalData->m_pMainUserSettings->setValue(SETT_LOAD_LAST_GAMES, this->m_ulastGamesLoadCount);
    this->m_pGlobalData->m_pMainUserSettings->setValue(SETT_USE_READABLE_NAME, this->m_useReadableName);
    this->m_pGlobalData->m_pMainUserSettings->setValue(SETT_LOAD_GAME_INFO, this->m_loadGameInfo);
    this->m_pGlobalData->m_pMainUserSettings->setValue(SETT_DEBUG_IP, this->m_debugIP);
    this->m_pGlobalData->m_pMainUserSettings->setValue(SETT_DEBUG_IP_WIFI, this->m_debugIPWifi);

    this->m_pGlobalData->m_pMainUserSettings->setValue(SETT_LAST_SHOWN_VERSION, this->m_lastShownVersion);

    this->m_pGlobalData->m_pMainUserSettings->endGroup();

    this->m_pGlobalData->m_pMainUserSettings->sync();
}

QString GlobalSettings::getCurrentVersion()
{
    return STAM_ORGA_VERSION_S;
}

QString GlobalSettings::getVersionChangeInfo()
{
    QString rValue;

    rValue.append("<b>V1.0.1:</b><br>");
    rValue.append("- Mehr Informationen in der Spielübersicht<br>");
    rValue.append("- automatisches Laden der Spielinformationen<br>");
    rValue.append("- Dauerkarten editierbar<br>");
    rValue.append("- Passwörter vollständig gehasht<br>");
    rValue.append("- Versionshistorie hinzugefügt<br>");
    rValue.append("- Diverse Bug fixes<br>");

    rValue.append("<br><b>V1.0.0:</b><br>");
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
    qDebug() << QString("State changed %1 %2").arg(state).arg(this->loadGameInfo());
    if (state != Qt::ApplicationState::ApplicationActive)
        return;

    if (!this->loadGameInfo())
        return;

    if (this->m_pGlobalData->userName() == "" || this->m_pGlobalData->passWord() == "")
        return;

    qint64 now = QDateTime::currentMSecsSinceEpoch();
    if ((now - this->m_lastGameInfoUpdate) < TIMEOUT_LOAD_GAMEINFO)
        return;

    if ((now - this->m_pGlobalData->m_gpLastTimeStamp) < TIMEOUT_LOAD_GAMES)
        emit this->sendAppStateChangedToActive(1);
    else
        emit this->sendAppStateChangedToActive(2);

    this->m_lastGameInfoUpdate = now;
}
