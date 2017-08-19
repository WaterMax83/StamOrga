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
    this->m_fcmServerKey = QString("key=%1").arg(fcmKey.replace("\n", "")).toUtf8();

    this->m_nam = new QNetworkAccessManager(this);

    this->m_fcmServiceUrl = QUrl("https://fcm.googleapis.com/fcm/send");

    connect(this->m_pGlobalData, &GlobalData::sendNewNotification, this, &PushNotification::slotSendNewNotification);

    connect(this->m_nam, &QNetworkAccessManager::finished, this, &PushNotification::finished);
    connect(this->m_nam, &QNetworkAccessManager::sslErrors, this, &PushNotification::sslErrors);

    this->m_lPushToSend.clear();
    this->m_lastPushNotify = NULL;

    /* Settings about the push notifycation, exclude this maybe later in another file */
    QString pushSettingsPath = getUserHomeConfigPath() + "/Settings/PushNotifySettings.ini";
    this->m_pPushSettings    = new QSettings(pushSettingsPath);
    this->m_pPushSettings->setIniCodec(("UTF-8"));

    this->m_pPushSettings->beginGroup("PushHeader");

    quint32 savedVersion = this->m_pPushSettings->value("PushAppVersion", 0).toUInt();
    if (savedVersion < STAM_ORGA_VERSION_I) {
//        this->m_pPushSettings->setValue("PushAppVersion", STAM_ORGA_VERSION_I);
        this->slotSendNewNotification(NOTIFY_TOPIC_NEW_APP_VERSION, "Neue StamOrga Version", QString("Es gibt eine neue Version: %1").arg(STAM_ORGA_VERSION_S));
    }

    this->m_pPushSettings->endGroup();

    return ERROR_CODE_SUCCESS;
}


void PushNotification::slotSendNewNotification(const QString topic, const QString header, const QString body)
{
    QMutexLocker lock(&this->m_sendMutex);

    PushNotifyInfo *push = new PushNotifyInfo();
    push->m_topic = topic;
    push->m_header = header;
    push->m_body = body;

    if (this->m_lastPushNotify == NULL)
        this->sendNewPushNotify(push);
    else
        this->m_lPushToSend.append(push);
}

void PushNotification::sendNewPushNotify(PushNotifyInfo *pushNotify)
{
    QNetworkRequest request(this->m_fcmServiceUrl);

    request.setRawHeader("Content-Type", "application/json");
    request.setRawHeader("Authorization", this->m_fcmServerKey.constData());

    QString sendTopic = pushNotify->m_topic;
#ifdef QT_DEBUG
    sendTopic.append("Debug");
#endif
    QByteArray jsonString = "{";
    jsonString.append(QString("\"to\" : \"/topics/%1\",").arg(sendTopic));
    jsonString.append("\"data\" : {");
    jsonString.append(QString("\"body\" : \"%1\",").arg(pushNotify->m_body));
    jsonString.append(QString("\"title\" : \"%1\",").arg(pushNotify->m_header));
    jsonString.append("}");
    jsonString.append("}");

    this->m_lastPushNotify = pushNotify;
    this->m_nam->post(request, jsonString);
}

void PushNotification::finished(QNetworkReply * reply)
{
    QMutexLocker lock(&this->m_sendMutex);

    qDebug() << QString("Finish: %1, %2, %3").arg(reply->isFinished()).arg(reply->errorString()).arg(reply->readAll());
    this->m_lastPushNotify = NULL;

    if (this->m_lPushToSend.count() > 0) {
        this->sendNewPushNotify(this->m_lPushToSend.first());
        this->m_lPushToSend.removeFirst();
    }
}

void PushNotification::sslErrors(QNetworkReply* reply, QList<QSslError> errors)
{
    QMutexLocker lock(&this->m_sendMutex);

    qDebug() << "There was an error";
    Q_UNUSED(reply);
    Q_UNUSED(errors);
    this->m_lastPushNotify = NULL;
}


PushNotification::~PushNotification()
{
}
