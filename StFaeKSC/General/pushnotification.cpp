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


// clang-format off
#define PUSH_GENERAL_GROUP	"General"
#define PUSH_NUMBER			"PushNumber"

#ifdef QT_DEBUG
#define WAIT_TIME_BEFORE_SEND      1 * 30 * 1000
#else
#define WAIT_TIME_BEFORE_SEND      5 * 60 * 1000
#endif
// clang-format on

PushNotification* g_pushNotify;

PushNotification::PushNotification(QObject* parent)
    : BackgroundWorker(parent)
{
    g_pushNotify = this;
}

void PushNotification::initialize(GlobalData* pGlobalData)
{
    this->m_pGlobalData = pGlobalData;

    this->SetWorkerName("PushNotify");

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

    this->m_restartSendTimer = new QTimer();
    this->m_restartSendTimer->setSingleShot(true);
    connect(this->m_restartSendTimer, &QTimer::timeout, this, &PushNotification::slotSendNewNotification);

    this->m_connectionTimeoutTimer = new QTimer();
    this->m_connectionTimeoutTimer->setSingleShot(true);
    this->m_connectionTimeoutTimer->setInterval(10000);
    connect(this->m_connectionTimeoutTimer, &QTimer::timeout, this, &PushNotification::slotConnectionTimeout);

    connect(this, &PushNotification::sendNewNotificationSignal, this, &PushNotification::slotSendNewNotification);

    connect(this->m_nam, &QNetworkAccessManager::finished, this, &PushNotification::finished);
    connect(this->m_nam, &QNetworkAccessManager::sslErrors, this, &PushNotification::sslErrors);

    this->m_lPushToSend.clear();
    this->m_lastPushNotify = NULL;

    /* Settings about the push notifycation, exclude this maybe later in another file */
    QString pushSettingsPath = getUserHomeConfigPath() + "/Settings/PushNotifySettings.ini";
    this->m_pPushSettings    = new QSettings(pushSettingsPath, QSettings::IniFormat);
    this->m_pPushSettings->setIniCodec(("UTF-8"));

    this->m_pPushSettings->beginGroup("PushHeader");

    quint32 savedVersion = this->m_pPushSettings->value("PushAppVersion", 0).toUInt();
    if (savedVersion < STAM_ORGA_VERSION_I) {
        this->m_pPushSettings->setValue("PushAppVersion", STAM_ORGA_VERSION_I);
        this->sendNewVersionNotification("Neue StamOrga Version", QString("Es gibt eine neue Version: %1").arg(STAM_ORGA_VERSION_S));
    }

    this->m_pPushSettings->endGroup();

    return ERROR_CODE_SUCCESS;
}

qint64 PushNotification::sendNewVersionNotification(const QString header, const QString body)
{
    PushNotifyInfo* push = new PushNotifyInfo();
    push->m_topic        = PUSH_NOTIFY_TOPIC::PUSH_NOT_NEW_VERSION;
    push->m_header       = header;
    push->m_body         = body;
    push->m_sendMessageID   = getNextInternalPushNumber();
    push->m_sendTime     = QDateTime::currentMSecsSinceEpoch();

    this->insertNewNotification(push);

    return push->m_sendMessageID;
}

qint64 PushNotification::sendNewMeetingNotification(const QString header, const QString body, const quint32 gameIndex)
{
    PushNotifyInfo* push   = new PushNotifyInfo();
    push->m_topic          = PUSH_NOTIFY_TOPIC::PUSH_NOT_NEW_MEETING;
    push->m_header         = header;
    push->m_body           = body;
    push->m_sendMessageID     = getNextInternalPushNumber();
    push->m_sendTime       = QDateTime::currentMSecsSinceEpoch() + WAIT_TIME_BEFORE_SEND; // 5min
    push->m_internalIndex1 = gameIndex;

    this->insertNewNotification(push);

    return push->m_sendMessageID;
}

qint64 PushNotification::sendChangeMeetingNotification(const QString header, const QString body, const quint32 gameIndex)
{
    this->m_notifyMutex.lock();

    foreach (PushNotifyInfo* p, this->m_lPushToSend) {
        if (p->m_topic == PUSH_NOTIFY_TOPIC::PUSH_NOT_NEW_MEETING || p->m_topic == PUSH_NOTIFY_TOPIC::PUSH_NOT_CHG_MEETING) {
            if (p->m_internalIndex1 == gameIndex)
                return -1;
        }
    }

    this->m_notifyMutex.unlock();

    PushNotifyInfo* push   = new PushNotifyInfo();
    push->m_topic          = PUSH_NOTIFY_TOPIC::PUSH_NOT_CHG_MEETING;
    push->m_header         = header;
    push->m_body           = body;
    push->m_sendMessageID     = getNextInternalPushNumber();
    push->m_sendTime       = QDateTime::currentMSecsSinceEpoch() + WAIT_TIME_BEFORE_SEND;
    push->m_internalIndex1 = gameIndex;

    this->insertNewNotification(push);

    return push->m_sendMessageID;
}

qint64 PushNotification::sendNewTicketNotification(const QString header, const QString body, const quint32 gameIndex, const quint32 ticketIndex)
{
    this->m_notifyMutex.lock();

    foreach (PushNotifyInfo* p, this->m_lPushToSend) {
        if (p->m_topic == PUSH_NOTIFY_TOPIC::PUSH_NOT_NEW_MEETING || p->m_topic == PUSH_NOTIFY_TOPIC::PUSH_NOT_CHG_MEETING) {
            if (p->m_internalIndex1 == gameIndex && p->m_internalIndex2 == ticketIndex)
                return -1;
        }
    }

    this->m_notifyMutex.unlock();

    PushNotifyInfo* push   = new PushNotifyInfo();
    push->m_topic          = PUSH_NOTIFY_TOPIC::PUSH_NOT_NEW_TICKET;
    push->m_header         = header;
    push->m_body           = body;
    push->m_sendMessageID     = getNextInternalPushNumber();
    push->m_sendTime       = QDateTime::currentMSecsSinceEpoch() + WAIT_TIME_BEFORE_SEND;
    push->m_internalIndex1 = gameIndex;
    push->m_internalIndex2 = ticketIndex;

    this->insertNewNotification(push);

    return push->m_sendMessageID;
}

qint64 PushNotification::removeNewTicketNotification(const quint32 gameIndex, const quint32 ticketIndex)
{
    QMutexLocker lock(&this->m_notifyMutex);

    for(int i=0; i<this->m_lPushToSend.count(); i++) {
        if (this->m_lPushToSend[i]->m_internalIndex1 == gameIndex && this->m_lPushToSend[i]->m_internalIndex2 == ticketIndex)
            this->m_lPushToSend.removeAt(i);
    }
    return -1;
}

void PushNotification::insertNewNotification(PushNotifyInfo* push)
{
    QMutexLocker lock(&this->m_notifyMutex);

    for (int i = 0; i < this->m_lPushToSend.count(); i++) {
        if (push->m_sendTime < this->m_lPushToSend[i]->m_sendTime) {
            this->m_lPushToSend.insert(i, push);

            emit this->sendNewNotificationSignal();
            return;
        }
    }

    this->m_lPushToSend.append(push);

    emit this->sendNewNotificationSignal();
}

void PushNotification::slotSendNewNotification()
{
    if (!this->m_notifyMutex.tryLock()) {
        this->m_restartSendTimer->start(5);
        return;
    }

    this->m_notifyMutex.unlock();
    QMutexLocker lock(&this->m_notifyMutex);

    if (this->m_lPushToSend.count() <= 0)
        return;

    if (this->m_lastPushNotify != NULL) {
        this->m_restartSendTimer->start(100);
        return;
    }

    qint64 now = QDateTime::currentMSecsSinceEpoch();
    if (this->m_lPushToSend.first()->m_sendTime <= now) {
        this->startSendNewPushNotify(this->m_lPushToSend.first());
        this->m_lPushToSend.removeFirst();
    } else {
        this->m_restartSendTimer->start(this->m_lPushToSend.first()->m_sendTime - now);
    }
}

void PushNotification::slotConnectionTimeout()
{
    QMutexLocker lock(&this->m_notifyMutex);

    if (this->m_lastPushNotify != NULL)
        delete this->m_lastPushNotify;
    this->m_lastPushNotify = NULL;

    this->slotSendNewNotification();
}

void PushNotification::startSendNewPushNotify(PushNotifyInfo* pushNotify)
{
    QNetworkRequest request(this->m_fcmServiceUrl);

    request.setRawHeader("Content-Type", "application/json");
    request.setRawHeader("Authorization", this->m_fcmServerKey.constData());

    QString sendTopic = this->getTopicStringFromIndex(pushNotify->m_topic);
#ifdef QT_DEBUG
    sendTopic.append("Debug");
#endif
    QByteArray jsonString = "{";
    jsonString.append(QString("\"to\" : \"/topics/%1\",").arg(sendTopic));
    jsonString.append("\"data\" : {");
    jsonString.append(QString("\"body\" : \"%1\",").arg(pushNotify->m_body));
    jsonString.append(QString("\"title\" : \"%1\",").arg(pushNotify->m_header));
    jsonString.append(QString("\"id\" : \"%1\",").arg(pushNotify->m_sendMessageID));
    jsonString.append("}");
    jsonString.append("}");

    qInfo().noquote() << QString("Send Notification %3 to \"%1\" about \"%2\"")
                         .arg(sendTopic, pushNotify->m_body)
                         .arg(pushNotify->m_sendMessageID);

    this->m_lastPushNotify = pushNotify;
    this->m_nam->post(request, jsonString);
    this->m_connectionTimeoutTimer->start();
}

void PushNotification::finished(QNetworkReply* reply)
{
    QMutexLocker lock(&this->m_notifyMutex);

    this->m_connectionTimeoutTimer->stop();

    qint64 sendNumber = -1;
    if (this->m_lastPushNotify != NULL)
        sendNumber = this->m_lastPushNotify->m_sendMessageID;

    if (reply->error() == QNetworkReply::NetworkError::NoError)
        qInfo().noquote() << QString("Finished Notification %2: %1").arg(QString(reply->readAll())).arg(sendNumber);
    else
        qWarning().noquote() << QString("Error finishing Notification to %3, %1:%2").arg(reply->error()).arg(reply->errorString()).arg(sendNumber);

    if (this->m_lastPushNotify != NULL)
        delete this->m_lastPushNotify;
    this->m_lastPushNotify = NULL;

    this->slotSendNewNotification();
}

void PushNotification::sslErrors(QNetworkReply* reply, QList<QSslError> errors)
{
    QMutexLocker lock(&this->m_notifyMutex);

    this->m_connectionTimeoutTimer->stop();

    qDebug() << "There was an error sending the notification";
    Q_UNUSED(reply);
    Q_UNUSED(errors);
    if (this->m_lastPushNotify != NULL)
        delete this->m_lastPushNotify;
    this->m_lastPushNotify = NULL;

    this->slotSendNewNotification();
}

QString PushNotification::getTopicStringFromIndex(const PUSH_NOTIFY_TOPIC topic)
{
    switch (topic) {
    case PUSH_NOT_NEW_VERSION:
        return NOTIFY_TOPIC_NEW_APP_VERSION;
    case PUSH_NOT_NEW_MEETING:
        return NOTIFY_TOPIC_NEW_MEETING;
    case PUSH_NOT_CHG_MEETING:
        return NOTIFY_TOPIC_CHANGE_MEETING;
    case PUSH_NOT_NEW_TICKET:
        return NOTIFY_TOPIC_NEW_FREE_TICKET;
    default:
        return NOTIFY_TOPIC_GENERAL;
    }
}


qint64 PushNotification::getNextInternalPushNumber()
{
    qint64 savedIndex;

    //QMutexLocker locker(&this->m_mConfigIniMutex);

    this->m_pPushSettings->beginGroup(PUSH_GENERAL_GROUP);
    savedIndex = this->m_pPushSettings->value(PUSH_NUMBER, 0).toLongLong();

    savedIndex++;

    this->m_pPushSettings->setValue(PUSH_NUMBER, savedIndex);
    this->m_pPushSettings->endGroup();

    return savedIndex;
}


PushNotification::~PushNotification()
{
    for (int i = this->m_lPushToSend.count() - 1; i >= 0; i++) {
        delete this->m_lPushToSend.at(i);
        this->m_lPushToSend.removeLast();
    }

    if (this->m_lastPushNotify != NULL)
        delete this->m_lastPushNotify;
    this->m_lastPushNotify = NULL;

    if (this->m_restartSendTimer != NULL)
        delete this->m_restartSendTimer;
    this->m_restartSendTimer = NULL;

    if (this->m_connectionTimeoutTimer != NULL)
        delete this->m_connectionTimeoutTimer;
    this->m_connectionTimeoutTimer = NULL;
}
