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
#define PUSH_APP_VERSION    "PushAppVersion"
#define PUSH_DEACTIVATE     "Deactivate"
#define PUSH_NUMBER			"PushNumber"

#ifdef QT_DEBUG
#define WAIT_TIME_BEFORE_SEND      1 * 20 * 1000
#else
#define WAIT_TIME_BEFORE_SEND      5 * 60 * 1000
#endif
// clang-format on

PushNotification* g_pushNotify;

PushNotification::PushNotification(QObject* parent)
    : BackgroundWorker(parent)
{
    g_pushNotify        = this;
    this->m_initialized = false;
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

    this->m_fcmServiceUrl = QUrl("https://fcm.googleapis.com/fcm/send");

    this->m_restartSendTimer = new QTimer();
    this->m_restartSendTimer->setSingleShot(true);
    connect(this->m_restartSendTimer, &QTimer::timeout, this, &PushNotification::slotSendNewNotification);

    this->m_connectionTimeoutTimer = new QTimer();
    this->m_connectionTimeoutTimer->setSingleShot(true);
    this->m_connectionTimeoutTimer->setInterval(10000);
    connect(this->m_connectionTimeoutTimer, &QTimer::timeout, this, &PushNotification::slotConnectionTimeout);

    connect(this, &PushNotification::sendNewNotificationSignal, this, &PushNotification::slotSendNewNotification);

    this->m_nam = new QNetworkAccessManager(this);
    connect(this->m_nam, &QNetworkAccessManager::finished, this, &PushNotification::finished);
    connect(this->m_nam, &QNetworkAccessManager::sslErrors, this, &PushNotification::sslErrors);

    this->m_lPushToSend.clear();
    this->m_lastPushNotify = NULL;

    this->m_mConfigIniMutex.lock();

    /* Settings about the push notifycation, exclude this maybe later in another file */
    QString pushSettingsPath = getUserHomeConfigPath() + "/Settings/PushNotifySettings.ini";
    this->m_pConfigSettings  = new QSettings(pushSettingsPath, QSettings::IniFormat);
    this->m_pConfigSettings->setIniCodec(("UTF-8"));

    this->m_pConfigSettings->beginGroup(PUSH_GENERAL_GROUP);

    if (this->m_pConfigSettings->value(PUSH_DEACTIVATE, 0).toInt() != 0)
        this->m_doNotUseSSLbecauseOfVersion = true;

    quint32 savedVersion = this->m_pConfigSettings->value(PUSH_APP_VERSION, 0).toUInt();

    this->m_pConfigSettings->endGroup();

    this->m_mConfigIniMutex.unlock();

    this->m_initialized = true;

    if (savedVersion < STAM_ORGA_VERSION_I) {
        this->m_mConfigIniMutex.lock();
        this->m_pConfigSettings->beginGroup(PUSH_GENERAL_GROUP);
        this->m_pConfigSettings->setValue(PUSH_APP_VERSION, STAM_ORGA_VERSION_I);
        this->m_pConfigSettings->endGroup();
        this->m_mConfigIniMutex.unlock();
        this->sendNewVersionNotification(QString("Es gibt eine neue Version: %1").arg(STAM_ORGA_VERSION_S));
    }

    this->loadCurrentInteralList();

    return ERROR_CODE_SUCCESS;
}

qint64 PushNotification::sendNewVersionNotification(const QString body)
{
    if (!this->m_initialized)
        return ERROR_CODE_NOT_READY;

    PushNotifyInfo* push  = new PushNotifyInfo();
    push->m_topic         = PUSH_NOTIFY_TOPIC::PUSH_NOT_NEW_VERSION;
    push->m_header        = "Neue StamOrga Version";
    push->m_body          = body;
    push->m_sendMessageID = getNextInternalPushNumber();
    push->m_sendTime      = QDateTime::currentMSecsSinceEpoch();
    push->m_userID        = -1;
    push->m_info          = STAM_ORGA_VERSION_S;

    this->insertNewNotification(push);

    return push->m_sendMessageID;
}

qint64 PushNotification::sendNewMeetingNotification(const QString body, const qint32 userID, const quint32 gameIndex)
{
    if (!this->m_initialized)
        return ERROR_CODE_NOT_READY;

    PushNotifyInfo* push   = new PushNotifyInfo();
    push->m_topic          = PUSH_NOTIFY_TOPIC::PUSH_NOT_NEW_MEETING;
    push->m_header         = "Neues Treffen angelegt";
    push->m_body           = body;
    push->m_sendMessageID  = getNextInternalPushNumber();
    push->m_sendTime       = QDateTime::currentMSecsSinceEpoch() + WAIT_TIME_BEFORE_SEND; // 5min
    push->m_userID         = userID;
    push->m_internalIndex1 = gameIndex;

    this->insertNewNotification(push);

    return push->m_sendMessageID;
}

qint64 PushNotification::sendChangeMeetingNotification(const QString body, const qint32 userID, const quint32 gameIndex)
{
    if (!this->m_initialized)
        return ERROR_CODE_NOT_READY;

    this->m_notifyMutex.lock();

    foreach (PushNotifyInfo* p, this->m_lPushToSend) {
        if (p->m_topic == PUSH_NOTIFY_TOPIC::PUSH_NOT_NEW_MEETING || p->m_topic == PUSH_NOTIFY_TOPIC::PUSH_NOT_CHG_MEETING) {
            if (p->m_internalIndex1 == gameIndex) {
                this->m_notifyMutex.unlock();
                return -1;
            }
        }
    }

    this->m_notifyMutex.unlock();

    PushNotifyInfo* push   = new PushNotifyInfo();
    push->m_topic          = PUSH_NOTIFY_TOPIC::PUSH_NOT_CHG_MEETING;
    push->m_header         = "Treffen wurde verändert";
    push->m_body           = body;
    push->m_sendMessageID  = getNextInternalPushNumber();
    push->m_sendTime       = QDateTime::currentMSecsSinceEpoch() + WAIT_TIME_BEFORE_SEND;
    push->m_userID         = userID;
    push->m_internalIndex1 = gameIndex;

    this->insertNewNotification(push);

    return push->m_sendMessageID;
}

qint64 PushNotification::sendNewTicketNotification(const QString body, const qint32 userID, const quint32 gameIndex, const quint32 ticketIndex)
{
    if (!this->m_initialized)
        return ERROR_CODE_NOT_READY;

    this->m_notifyMutex.lock();

    foreach (PushNotifyInfo* p, this->m_lPushToSend) {
        if (p->m_topic == PUSH_NOTIFY_TOPIC::PUSH_NOT_NEW_MEETING || p->m_topic == PUSH_NOTIFY_TOPIC::PUSH_NOT_CHG_MEETING) {
            if (p->m_internalIndex1 == gameIndex && p->m_internalIndex2 == ticketIndex) {
                this->m_notifyMutex.unlock();
                return -1;
            }
        }
    }

    this->m_notifyMutex.unlock();

    PushNotifyInfo* push   = new PushNotifyInfo();
    push->m_topic          = PUSH_NOTIFY_TOPIC::PUSH_NOT_NEW_TICKET;
    push->m_header         = "Neue Karte frei";
    push->m_body           = body;
    push->m_sendMessageID  = getNextInternalPushNumber();
    push->m_sendTime       = QDateTime::currentMSecsSinceEpoch() + WAIT_TIME_BEFORE_SEND;
    push->m_userID         = userID;
    push->m_internalIndex1 = gameIndex;
    push->m_internalIndex2 = ticketIndex;

    this->insertNewNotification(push);

    return push->m_sendMessageID;
}

qint64 PushNotification::removeNewTicketNotification(const quint32 gameIndex, const quint32 ticketIndex)
{
    if (!this->m_initialized)
        return ERROR_CODE_NOT_READY;

    QMutexLocker lock(&this->m_notifyMutex);

    for (int i = 0; i < this->m_lPushToSend.count(); i++) {
        if (this->m_lPushToSend[i]->m_internalIndex1 == gameIndex && this->m_lPushToSend[i]->m_internalIndex2 == ticketIndex)
            this->m_lPushToSend.removeAt(i);
    }
    return -1;
}

qint64 PushNotification::sendNewFirstAwayAccept(const QString body, const qint32 userID)
{
    if (!this->m_initialized)
        return ERROR_CODE_NOT_READY;

    PushNotifyInfo* push  = new PushNotifyInfo();
    push->m_topic         = PUSH_NOTIFY_TOPIC::PUSH_NOT_NEW_AWAY_ACCEPT;
    push->m_header        = "Neuer Auswärtsfahrer";
    push->m_body          = body;
    push->m_sendMessageID = getNextInternalPushNumber();
    push->m_sendTime      = QDateTime::currentMSecsSinceEpoch() + WAIT_TIME_BEFORE_SEND;
    push->m_userID        = userID;

    this->insertNewNotification(push);

    return push->m_sendMessageID;
}

qint64 PushNotification::sendNewFanclubNewsNotification(const QString body, const qint32 userID)
{
    if (!this->m_initialized)
        return ERROR_CODE_NOT_READY;

    this->m_notifyMutex.lock();

    foreach (PushNotifyInfo* p, this->m_lPushToSend) {
        if (p->m_topic == PUSH_NOTIFY_TOPIC::PUSH_NOT_NEW_FAN_NEWS) {
            this->m_notifyMutex.unlock();
            return -1;
        }
    }

    this->m_notifyMutex.unlock();

    PushNotifyInfo* push  = new PushNotifyInfo();
    push->m_topic         = PUSH_NOTIFY_TOPIC::PUSH_NOT_NEW_FAN_NEWS;
    push->m_header        = "Fanclub Nachricht";
    push->m_body          = body;
    push->m_sendMessageID = getNextInternalPushNumber();
    push->m_sendTime      = QDateTime::currentMSecsSinceEpoch() + WAIT_TIME_BEFORE_SEND;
    push->m_userID        = userID;

    this->insertNewNotification(push);

    return push->m_sendMessageID;
}

void PushNotification::insertNewNotification(PushNotifyInfo* push)
{
    if (!this->m_initialized)
        return;

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
    if (!this->m_initialized)
        return;

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

    qint64 sendNumber = -1;
    if (this->m_lastPushNotify != NULL) {
        sendNumber = this->m_lastPushNotify->m_sendMessageID;
        delete this->m_lastPushNotify;
    }
    this->m_lastPushNotify = NULL;

    qInfo().noquote() << QString("Connection timeout for Notification %1").arg(sendNumber);

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
    jsonString.append(QString("\"m_id\" : \"%1\",").arg(pushNotify->m_sendMessageID));
    jsonString.append(QString("\"u_id\" : \"%1\",").arg(pushNotify->m_userID));
    jsonString.append("}");
    jsonString.append("}");

    qInfo().noquote() << QString("Send Notification %3 to \"%1\" about \"%2\" with size %4")
                             .arg(sendTopic, pushNotify->m_body)
                             .arg(pushNotify->m_sendMessageID)
                             .arg(jsonString.length());

    this->m_lastPushNotify = pushNotify;
    if (!this->m_doNotUseSSLbecauseOfVersion)
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

    if (reply->error() == QNetworkReply::NetworkError::NoError) {
        qInfo().noquote() << QString("Finished Notification %2: %1").arg(QString(reply->readAll())).arg(sendNumber);

        if (!this->m_lastPushNotify->m_info.isEmpty()) {
            QString sendTopic = this->getTopicStringFromIndex(this->m_lastPushNotify->m_topic);
            this->m_pGlobalData->addNewUserEvent(sendTopic, this->m_lastPushNotify->m_info, this->m_lastPushNotify->m_userID);
        }
    } else
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
    case PUSH_NOT_NEW_AWAY_ACCEPT:
        return NOTIFY_TOPIC_NEW_AWAY_ACCEPT;
    case PUSH_NOT_NEW_FAN_NEWS:
        return NOTIFY_TOPIC_NEW_FANCLUB_NEWS;
    default:
        return NOTIFY_TOPIC_GENERAL;
    }
}


qint64 PushNotification::getNextInternalPushNumber()
{
    qint64 savedIndex;

    QMutexLocker locker(&this->m_mConfigIniMutex);

    this->m_pConfigSettings->beginGroup(PUSH_GENERAL_GROUP);
    savedIndex = this->m_pConfigSettings->value(PUSH_NUMBER, 0).toLongLong();
    savedIndex++;
    this->m_pConfigSettings->setValue(PUSH_NUMBER, savedIndex);
    this->m_pConfigSettings->endGroup();

    return savedIndex;
}

qint32 PushNotification::addNewAppInformation(QString guid, QString fcmToken, qint32 system, quint32 userIndex)
{
    qint64       timestamp = QDateTime::currentMSecsSinceEpoch();
    AppTokenUID* app;
    if ((app = this->appInfoExists(guid)) != NULL) {

        this->m_mInternalInfoMutex.lock();

        if (app->m_fcmToken != fcmToken) {
            if (this->updateItemValue(app, APP_TOKEN_TOKEN, QVariant(fcmToken)))
                app->m_fcmToken = fcmToken;

            if (this->updateItemValue(app, ITEM_TIMESTAMP, QVariant(timestamp)))
                app->m_timestamp = timestamp;
        }

        if (app->m_userIndex != userIndex) {
            if (this->updateItemValue(app, APP_TOKEN_USERINDEX, QVariant(userIndex)))
                app->m_userIndex = userIndex;

            if (this->updateItemValue(app, ITEM_TIMESTAMP, QVariant(timestamp)))
                app->m_timestamp = timestamp;
        }

        this->m_mInternalInfoMutex.unlock();
        return app->m_index;
    }

    int newIndex = this->getNextInternalIndex();

    this->m_mConfigIniMutex.lock();

    this->m_pConfigSettings->beginGroup(GROUP_LIST_ITEM);
    this->m_pConfigSettings->beginWriteArray(CONFIG_LIST_ARRAY);
    this->m_pConfigSettings->setArrayIndex(this->getNumberOfInternalList());

    this->m_pConfigSettings->setValue(ITEM_TIMESTAMP, timestamp);
    this->m_pConfigSettings->setValue(ITEM_INDEX, newIndex);

    this->m_pConfigSettings->setValue(APP_TOKEN_GUID, guid);
    this->m_pConfigSettings->setValue(APP_TOKEN_TOKEN, fcmToken);
    this->m_pConfigSettings->setValue(APP_TOKEN_SYSTEM, system);
    this->m_pConfigSettings->setValue(APP_TOKEN_USERINDEX, userIndex);

    this->m_pConfigSettings->endArray();
    this->m_pConfigSettings->endGroup();
    this->m_pConfigSettings->sync();

    this->m_mConfigIniMutex.unlock();

    app = new AppTokenUID(guid, fcmToken, timestamp, userIndex, system, newIndex);

    this->addNewAppToken(app, false);

    qInfo().noquote() << QString("Added new App Information: %1").arg(guid);
    return newIndex;
}

AppTokenUID* PushNotification::appInfoExists(QString guid)
{
    QMutexLocker locker(&this->m_mInternalInfoMutex);

    for (int i = 0; i < this->getNumberOfInternalList(); i++) {
        AppTokenUID* app = (AppTokenUID*)(this->getItemFromArrayIndex(i));
        if (app == NULL)
            continue;
        if (app->m_guid == guid)
            return app;
    }
    return NULL;
}

void PushNotification::loadCurrentInteralList()
{
    /* Check wheter we have to save data after reading again */
    bool bProblems = false;
    {
        QMutexLocker locker(&this->m_mConfigIniMutex);

        this->m_pConfigSettings->beginGroup(GROUP_LIST_ITEM);
        int sizeOfGames = this->m_pConfigSettings->beginReadArray(CONFIG_LIST_ARRAY);

        for (int i = 0; i < sizeOfGames; i++) {
            this->m_pConfigSettings->setArrayIndex(i);
            qint64  timestamp = this->m_pConfigSettings->value(ITEM_TIMESTAMP, 0x0).toLongLong();
            quint32 index     = this->m_pConfigSettings->value(ITEM_INDEX, 0).toInt();

            QString guid      = this->m_pConfigSettings->value(APP_TOKEN_GUID, "").toString();
            QString token     = this->m_pConfigSettings->value(APP_TOKEN_TOKEN, "").toString();
            qint32  system    = this->m_pConfigSettings->value(APP_TOKEN_SYSTEM, -1).toInt();
            quint32 userIndex = this->m_pConfigSettings->value(APP_TOKEN_USERINDEX, 0).toUInt();

            AppTokenUID* app = new AppTokenUID(guid, token, timestamp, userIndex, system, index);

            if (!this->addNewAppToken(app))
                bProblems = true;
        }
        this->m_pConfigSettings->endArray();
        this->m_pConfigSettings->endGroup();
    }

    for (int i = 0; i < this->m_lAddItemProblems.size(); i++) {
        bProblems        = true;
        AppTokenUID* app = (AppTokenUID*)(this->getProblemItemFromArrayIndex(i));
        if (app == NULL)
            continue;
        app->m_index = this->getNextInternalIndex();
        this->addNewAppToken(app);
    }
    this->m_lAddItemProblems.clear();

    if (bProblems)
        this->saveCurrentInteralList();
}

bool PushNotification::addNewAppToken(AppTokenUID* app, bool checkApp)
{
    if (checkApp) {
        if (app->m_index == 0 || itemExists(app->m_index)) {
            qWarning().noquote() << QString("AppToken \"%1\" with index \"%2\" already exists, saving with new index").arg(app->m_guid).arg(app->m_index);
            this->addNewConfigItem(app, &this->m_lAddItemProblems);
            return false;
        }
    }

    this->addNewConfigItem(app, &this->m_lInteralList);
    return true;
}

void PushNotification::showCurrentTokenInformation()
{
    QMutexLocker locker(&this->m_mInternalInfoMutex);

    for (int i = 0; i < this->getNumberOfInternalList(); i++) {
        AppTokenUID* app = (AppTokenUID*)(this->getItemFromArrayIndex(i));
        if (app == NULL)
            continue;
        QString date  = QDateTime::fromMSecsSinceEpoch(app->m_timestamp).toString("dd.MM.yyyy hh:mm");
        QString token = app->m_fcmToken == "" ? "Kein Token" : app->m_fcmToken;
        std::cout << QString("%1: ").arg(app->m_index).toStdString()
                  << this->m_pGlobalData->m_UserList.getItemName(app->m_userIndex).toStdString()
                  << (QString(" - %1 - %3 - %2").arg(date, app->m_guid).arg(app->m_oSystem)).toStdString()
                  << std::endl
                  << token.toStdString() << std::endl;
    }
}

void PushNotification::saveCurrentInteralList()
{
    this->m_mConfigIniMutex.lock();

    this->m_pConfigSettings->beginGroup(GROUP_LIST_ITEM);
    this->m_pConfigSettings->remove(""); // clear all elements

    this->m_pConfigSettings->beginWriteArray(CONFIG_LIST_ARRAY);
    for (int i = 0; i < this->getNumberOfInternalList(); i++) {
        AppTokenUID* pItem = (AppTokenUID*)(this->getItemFromArrayIndex(i));
        if (pItem == NULL)
            continue;

        this->m_pConfigSettings->setArrayIndex(i);

        this->m_pConfigSettings->setValue(ITEM_TIMESTAMP, pItem->m_timestamp);
        this->m_pConfigSettings->setValue(ITEM_INDEX, pItem->m_index);

        this->m_pConfigSettings->setValue(APP_TOKEN_GUID, pItem->m_guid);
        this->m_pConfigSettings->setValue(APP_TOKEN_TOKEN, pItem->m_fcmToken);
        this->m_pConfigSettings->setValue(APP_TOKEN_SYSTEM, pItem->m_oSystem);
        this->m_pConfigSettings->setValue(APP_TOKEN_USERINDEX, pItem->m_userIndex);
    }

    this->m_pConfigSettings->endArray();
    this->m_pConfigSettings->endGroup();

    this->m_mConfigIniMutex.unlock();

    qDebug().noquote() << QString("saved current App Token Guid List with %1 entries").arg(this->getNumberOfInternalList());
}


PushNotification::~PushNotification()
{
    for (int i = this->m_lPushToSend.count() - 1; i >= 0; i++) {
        delete this->m_lPushToSend.at(i);
        this->m_lPushToSend.removeLast();
    }

    if (this->m_nam != NULL)
        delete this->m_nam;
    this->m_nam = NULL;

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
