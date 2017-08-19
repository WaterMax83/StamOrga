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

#include <QFile>

#include "../Common/General/config.h"
#include "../Common/General/globalfunctions.h"
#include "pushnotification.h"

PushNotification::PushNotification(QObject* parent)
    : BackgroundWorker(parent)
{
}

void PushNotification::initialize(GlobalData* pGlobalData)
{
    this->m_pGlobalData = pGlobalData;

    this->m_ctrlBackground.Start(this, false);
}


int PushNotification::DoBackgroundWork()
{
    QString fcmServerKeyFile = getUserHomeConfigPath() + "/Settings/fcmServerKey.key";

    QFile fcmKeyFile(fcmServerKeyFile);

    if (!fcmKeyFile.exists()) {
        qWarning().noquote() << QString("Did not found FCM Server Key File: %1").arg(fcmServerKeyFile);
        return ERROR_CODE_NOT_FOUND;
    }

    if (!fcmKeyFile.open(QFile::ReadOnly)) {
        qWarning().noquote() << QString("Could not open FCM Server Key File: %1").arg(fcmServerKeyFile);
        return ERROR_CODE_NOT_POSSIBLE;
    }

    QString fcmKey(fcmKeyFile.readAll());
    this->m_fcmServerKey = fcmKey.replace("\n", "");

    connect(this->m_pGlobalData, &GlobalData::sendNewNotification, this, &PushNotification::slotSendNewNotification);

    /* Settings about the push notifycation, exclude this maybe later in another file */
    QString pushSettingsPath = getUserHomeConfigPath() + "/Settings/PushNotifySettings.ini";
    this->m_pPushSettings    = new QSettings(pushSettingsPath);
    this->m_pPushSettings->setIniCodec(("UTF-8"));

    this->m_pPushSettings->beginGroup("PushHeader");

    quint32 savedVersion = this->m_pPushSettings->value("PushAppVersion", 0).toUInt();
    if (savedVersion < STAM_ORGA_VERSION_I) {
        this->m_pPushSettings->setValue("PushAppVersion", STAM_ORGA_VERSION_I);
        this->slotSendNewNotification(NOTIFY_TOPIC_NEW_APP_VERSION, "Neue Version", QString("Es gibt eine neue Version: %1").arg(STAM_ORGA_VERSION_S));
    }

    this->m_pPushSettings->endGroup();

    return ERROR_CODE_SUCCESS;
}


void PushNotification::slotSendNewNotification(const QString topic, const QString header, const QString body)
{
    Q_UNUSED(topic);
    Q_UNUSED(header);
    Q_UNUSED(body);
}


PushNotification::~PushNotification()
{
}
