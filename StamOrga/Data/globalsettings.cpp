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

GlobalSettings::GlobalSettings(QObject* parent)
    : QObject(parent)
{
}

void GlobalSettings::initialize(GlobalData* pGlobalData)
{
    this->m_pGlobalData = pGlobalData;

    this->m_pGlobalData->m_pMainUserSettings->beginGroup("GLOBAL_SETTINGS");

    this->setLastGamesLoadCount(this->m_pGlobalData->m_pMainUserSettings->value("LastGamesLoadCount", 5).toInt());
    this->setUseReadableName(this->m_pGlobalData->m_pMainUserSettings->value("UseReadableName", true).toBool());
    this->setDebugIP(this->m_pGlobalData->m_pMainUserSettings->value("DebugIP", "").toString());
    this->setDebugIPWifi(this->m_pGlobalData->m_pMainUserSettings->value("DebugIPWifi", "").toString());
    this->m_lastShownVersion = this->m_pGlobalData->m_pMainUserSettings->value("LastShownVersion", "").toString();

    this->m_pGlobalData->m_pMainUserSettings->endGroup();
}


void GlobalSettings::saveGlobalSettings()
{
    QMutexLocker lock(&this->m_pGlobalData->m_mutexUser);

    this->m_pGlobalData->m_pMainUserSettings->beginGroup("GLOBAL_SETTINGS");

    this->m_pGlobalData->m_pMainUserSettings->setValue("LastGamesLoadCount", this->m_ulastGamesLoadCount);
    this->m_pGlobalData->m_pMainUserSettings->setValue("UseReadableName", this->m_useReadableName);
    this->m_pGlobalData->m_pMainUserSettings->setValue("DebugIP", this->m_debugIP);
    this->m_pGlobalData->m_pMainUserSettings->setValue("DebugIPWifi", this->m_debugIPWifi);

    this->m_pGlobalData->m_pMainUserSettings->setValue("LastShownVersion", this->m_lastShownVersion);

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
    rValue.append("Mehr Informationen in der Spielübersicht<br>");
    rValue.append("Passwörter gehasht<br>");
    rValue.append("Versionshistorie<br>");
    rValue.append("Diverse Bug fixes<br>");

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
