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

#include <QtCore/QUuid>

#include "../Common/General/globalfunctions.h"
#include "../cstasettingsmanager.h"
#include "cdatappinfomanager.h"

cDatAppInfoManager g_DatAppInfoManager;

// clang-format off

#define APP_INFO_GROUP      "AppInfo"
#define APP_INFO_TOKEN      "FcmToken"
#define APP_INFO_GUID       "AppGuid"

// clang-format on

cDatAppInfoManager::cDatAppInfoManager(QObject* parent)
    : cGenDisposer(parent)
{
}

qint32 cDatAppInfoManager::initialize()
{
    QString value;

#ifdef Q_OS_ANDROID
    this->m_pushNotificationInfoHandler = new PushNotificationInformationHandler(this);
    connect(this->m_pushNotificationInfoHandler, &PushNotificationInformationHandler::fcmRegistrationTokenChanged,
            this, &GlobalData::slotNewFcmRegistrationToken);

    g_StaSettingsManager.getValue(APP_INFO_GROUP, APP_INFO_TOKEN, value);
    this->m_pushNotificationToken = value;
#endif

    g_StaSettingsManager.getValue(APP_INFO_GROUP, APP_INFO_GUID, value);
    this->m_AppInstanceGUID = value;

    if (this->m_AppInstanceGUID == "") {
        this->m_AppInstanceGUID = QUuid::createUuid().toString();
        g_StaSettingsManager.setValue(APP_INFO_GROUP, APP_INFO_GUID, this->m_AppInstanceGUID);
        qInfo().noquote() << QString("Create a new GUID for this instance %1").arg(this->m_AppInstanceGUID);
    }

    this->m_initialized = true;

    return ERROR_CODE_SUCCESS;
}

QString cDatAppInfoManager::getCurrentAppGUID()
{
    return this->m_AppInstanceGUID;
}

QString cDatAppInfoManager::getCurrentAppToken()
{
    return this->m_pushNotificationToken;
}
