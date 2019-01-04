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
#include "../Manager/csmtpmanager.h"
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
        this->sendNewVersionNotification();
    }

    this->loadCurrentInteralList();

    return ERROR_CODE_SUCCESS;
}

qint64 PushNotification::sendNewGeneralTopicNotification(const QString header, const QString body, const QString bigText)
{
    if (!this->m_initialized)
        return ERROR_CODE_NOT_READY;

    PushNotifyInfo* push   = new PushNotifyInfo();
    push->m_topic          = PUSH_NOTIFY_TOPIC::PUSH_NOT_GEN_TOPIC;
    push->m_userEventTopic = PUSH_NOTIFY_TOPIC::PUSH_NOT_GEN_TOPIC;
    push->m_header         = header;
    push->m_body           = body;
    push->m_bigText        = bigText;
    push->m_sendMessageID  = getNextInternalPushNumber();
    push->m_sendTime       = QDateTime::currentMSecsSinceEpoch();
    push->m_userID         = -1;
    push->m_info           = "";

    this->insertNewNotification(push);

    return push->m_sendMessageID;
}

qint64 PushNotification::sendNewVersionNotification()
{
    if (!this->m_initialized)
        return ERROR_CODE_NOT_READY;

    PushNotifyInfo* push   = new PushNotifyInfo();
    push->m_topic          = PUSH_NOTIFY_TOPIC::PUSH_NOT_NEW_VERSION;
    push->m_userEventTopic = PUSH_NOTIFY_TOPIC::PUSH_NOT_NEW_VERSION;
    push->m_header         = "StamOrga Version";
    push->m_body           = QString("Neue Version %1").arg(STAM_ORGA_VERSION_S);
    push->m_bigText        = QString("Es gibt eine neue StamOrga Version %1,\ndie heruntergeladen werden kann.").arg(STAM_ORGA_VERSION_S);
    push->m_summary        = STAM_ORGA_VERSION_S;
    push->m_sendMessageID  = getNextInternalPushNumber();
    push->m_sendTime       = QDateTime::currentMSecsSinceEpoch();
    push->m_userID         = -1;
    push->m_info           = STAM_ORGA_VERSION_S;

    this->insertNewNotification(push);

    return push->m_sendMessageID;
}

#define BODY_GAME "%1 - %2"
#define BIG_TEXT_MEETING "Am %1 beim Spiel %2:%3 wurde ein neues Treffen von %4 angelegt."
#define BIG_TEXT_AWAYTRIP "Am %1 beim Spiel %2:%3 wurde eine neue Fahrt von %4 angelegt."

qint64 PushNotification::sendNewMeetingNotification(const GamesPlay* pGame, QString& bigText, const qint32 userID, const qint32 type)
{
    if (!this->m_initialized)
        return ERROR_CODE_NOT_READY;

    PushNotifyInfo* push = new PushNotifyInfo();
    push->m_body         = QString(BODY_GAME).arg(pGame->m_itemName, pGame->m_away);
    push->m_summary      = QDateTime::fromMSecsSinceEpoch(pGame->m_timestamp).toString("dd.MM.yy");
    if (IS_PLAY_ONLY_FANCLUB(pGame->m_options))
        push->m_topic = PUSH_NOTIFY_TOPIC::PUSH_NOT_NEW_FAN_NEWS;
    else
        push->m_topic      = PUSH_NOTIFY_TOPIC::PUSH_NOT_NEW_MEETING;
    push->m_userEventTopic = PUSH_NOTIFY_TOPIC::PUSH_NOT_NEW_MEETING;
    if (type == MEETING_TYPE_MEETING) {
        push->m_header = "Neues Treffen";
        bigText.prepend(QString(BIG_TEXT_MEETING).arg(push->m_summary, pGame->m_itemName, pGame->m_away, this->m_pGlobalData->m_UserList.getReadableName(userID)));
        bigText.append(QString("Kommst du auch?"));
    } else {
        push->m_header = "Neue Fahrt";
        bigText.prepend(QString(BIG_TEXT_AWAYTRIP).arg(push->m_summary, pGame->m_itemName, pGame->m_away, this->m_pGlobalData->m_UserList.getReadableName(userID)));
        bigText.append(QString("Kommst du mit?"));
    }
    if (pGame->m_competition == 7)
        bigText.replace("Spiel", "Event");
    push->m_bigText        = bigText;
    push->m_sendMessageID  = getNextInternalPushNumber();
    push->m_sendTime       = QDateTime::currentMSecsSinceEpoch() + WAIT_TIME_BEFORE_SEND; // 5min
    push->m_userID         = userID;
    push->m_internalIndex1 = pGame->m_index;
    push->m_info           = QString::number(pGame->m_index);

    this->insertNewNotification(push);

    return push->m_sendMessageID;
}

#define BIG_TEXT_CHG_MEETING "Am %1 beim Spiel %2:%3 wurde das Treffen von %4 angepasst."
#define BIG_TEXT_CHG_AWAYTRIP "Am %1 beim Spiel %2:%3 wurde die Fahrt von %4 angepasst."

qint64 PushNotification::sendChangeMeetingNotification(const GamesPlay* pGame, QString& bigText, const qint32 userID, const qint32 type)
{
    if (!this->m_initialized)
        return ERROR_CODE_NOT_READY;

    this->m_notifyMutex.lock();

    foreach (PushNotifyInfo* p, this->m_lPushToSend) {
        if (p->m_userEventTopic == PUSH_NOTIFY_TOPIC::PUSH_NOT_NEW_MEETING) {
            if (p->m_internalIndex1 == pGame->m_index) {
                p->m_bigText = bigText;
                this->m_notifyMutex.unlock();
                return -1;
            }
        }
    }

    this->m_notifyMutex.unlock();

    PushNotifyInfo* push = new PushNotifyInfo();
    push->m_body         = QString(BODY_GAME).arg(pGame->m_itemName, pGame->m_away);
    push->m_summary      = QDateTime::fromMSecsSinceEpoch(pGame->m_timestamp).toString("dd.MM.yy");
    if (IS_PLAY_ONLY_FANCLUB(pGame->m_options))
        push->m_topic = PUSH_NOTIFY_TOPIC::PUSH_NOT_NEW_FAN_NEWS;
    else
        push->m_topic      = PUSH_NOTIFY_TOPIC::PUSH_NOT_NEW_MEETING;
    push->m_userEventTopic = PUSH_NOTIFY_TOPIC::PUSH_NOT_NEW_MEETING;
    if (type == MEETING_TYPE_MEETING) {
        push->m_header = "Treffen";
        bigText.prepend(QString(BIG_TEXT_CHG_MEETING).arg(push->m_summary, pGame->m_itemName, pGame->m_away, this->m_pGlobalData->m_UserList.getReadableName(userID)));
        bigText.append(QString("Kommst du auch?"));
    } else {
        push->m_header = "Fahrt";
        bigText.prepend(QString(BIG_TEXT_CHG_AWAYTRIP).arg(push->m_summary, pGame->m_itemName, pGame->m_away, this->m_pGlobalData->m_UserList.getReadableName(userID)));
        bigText.append(QString("Kommst du mit?"));
    }
    if (pGame->m_competition == 7)
        bigText.replace("Spiel", "Event");
    push->m_bigText        = bigText;
    push->m_sendMessageID  = getNextInternalPushNumber();
    push->m_sendTime       = QDateTime::currentMSecsSinceEpoch() + WAIT_TIME_BEFORE_SEND;
    push->m_userID         = userID;
    push->m_internalIndex1 = pGame->m_index;
    push->m_info           = QString::number(pGame->m_index);

    this->insertNewNotification(push);

    return push->m_sendMessageID;
}

#define TICKET_BIGTEXT "Die Dauerkarte von %1 wurde beim Spiel %2:%3 am %4 freigegeben"

qint64 PushNotification::sendNewTicketNotification(const GamesPlay* pGame, const qint32 userID, const qint32 ticketIndex)
{
    if (!this->m_initialized)
        return ERROR_CODE_NOT_READY;

    TicketInfo* pTicket = (TicketInfo*)this->m_pGlobalData->m_SeasonTicket.getItem(ticketIndex);
    if (pGame == NULL || pTicket == NULL || pTicket->isTicketRemoved())
        return ERROR_CODE_NOT_FOUND;

    this->m_notifyMutex.lock();

    foreach (PushNotifyInfo* p, this->m_lPushToSend) {
        if (p->m_userEventTopic == PUSH_NOTIFY_TOPIC::PUSH_NOT_NEW_TICKET) {
            if (p->m_internalIndex1 == pGame->m_index && p->m_internalIndex2 == ticketIndex) {
                this->m_notifyMutex.unlock();
                return -1;
            }
        }
    }

    this->m_notifyMutex.unlock();

    PushNotifyInfo* push   = new PushNotifyInfo();
    push->m_topic          = PUSH_NOTIFY_TOPIC::PUSH_NOT_NEW_TICKET;
    push->m_userEventTopic = PUSH_NOTIFY_TOPIC::PUSH_NOT_NEW_TICKET;
    push->m_header         = "Neue Dauerkarte frei";
    push->m_body           = QString(BODY_GAME).arg(pGame->m_itemName, pGame->m_away);
    push->m_summary        = QDateTime::fromMSecsSinceEpoch(pGame->m_timestamp).toString("dd.MM.yy");
    push->m_bigText        = QString(TICKET_BIGTEXT).arg(pTicket->m_itemName, pGame->m_itemName, pGame->m_away, push->m_summary);
    push->m_sendMessageID  = getNextInternalPushNumber();
    push->m_sendTime       = QDateTime::currentMSecsSinceEpoch() + WAIT_TIME_BEFORE_SEND;
    push->m_userID         = userID;
    push->m_internalIndex1 = pGame->m_index;
    push->m_internalIndex2 = ticketIndex;
    push->m_info           = QString::number(pGame->m_index);

    this->insertNewNotification(push);

    return push->m_sendMessageID;
}

qint64 PushNotification::removeNewTicketNotification(const qint32 gameIndex, const qint32 ticketIndex)
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

qint64 PushNotification::sendNewFirstAwayAccept(const QString bigText, const qint32 userID, const qint32 gameIndex)
{
    if (!this->m_initialized)
        return ERROR_CODE_NOT_READY;

    GamesPlay* pGame = (GamesPlay*)this->m_pGlobalData->m_GamesList.getItem(gameIndex);
    if (pGame == NULL)
        return ERROR_CODE_NOT_FOUND;

    PushNotifyInfo* push = new PushNotifyInfo();
    if (IS_PLAY_ONLY_FANCLUB(pGame->m_options))
        push->m_topic = PUSH_NOTIFY_TOPIC::PUSH_NOT_NEW_FAN_NEWS;
    else
        push->m_topic      = PUSH_NOTIFY_TOPIC::PUSH_NOT_NEW_MEETING;
    push->m_userEventTopic = PUSH_NOTIFY_TOPIC::PUSH_NOT_NEW_MEETING;
    push->m_header         = "Neuer Auswärtsfahrer";
    push->m_body           = QString(BODY_GAME).arg(pGame->m_itemName, pGame->m_away);
    push->m_summary        = QDateTime::fromMSecsSinceEpoch(pGame->m_timestamp).toString("dd.MM.yy");
    push->m_bigText        = bigText;
    push->m_sendMessageID  = getNextInternalPushNumber();
    push->m_sendTime       = QDateTime::currentMSecsSinceEpoch() + WAIT_TIME_BEFORE_SEND;
    push->m_userID         = userID;
    push->m_info           = QString::number(gameIndex);

    this->insertNewNotification(push);

    return push->m_sendMessageID;
}

qint64 PushNotification::sendNewFanclubNewsNotification(const QString body, const QString bigText, const qint32 userID, const qint32 newsID)
{
    if (!this->m_initialized)
        return ERROR_CODE_NOT_READY;

    this->m_notifyMutex.lock();

    foreach (PushNotifyInfo* p, this->m_lPushToSend) {
        if (p->m_userEventTopic == PUSH_NOTIFY_TOPIC::PUSH_NOT_NEW_FAN_NEWS) {
            p->m_body    = body;
            p->m_bigText = bigText;
            this->m_notifyMutex.unlock();
            return -1;
        }
    }

    this->m_notifyMutex.unlock();

    PushNotifyInfo* push   = new PushNotifyInfo();
    push->m_topic          = PUSH_NOTIFY_TOPIC::PUSH_NOT_NEW_FAN_NEWS;
    push->m_userEventTopic = PUSH_NOTIFY_TOPIC::PUSH_NOT_NEW_FAN_NEWS;
    push->m_header         = "Fanclub Nachricht";
    push->m_body           = body;
    push->m_bigText        = bigText;
    push->m_summary        = this->m_pGlobalData->m_UserList.getReadableName(userID);
    push->m_sendMessageID  = getNextInternalPushNumber();
    push->m_sendTime       = QDateTime::currentMSecsSinceEpoch() + WAIT_TIME_BEFORE_SEND;
    push->m_userID         = userID;
    push->m_info           = QString::number(newsID);

    this->insertNewNotification(push);

    return push->m_sendMessageID;
}


#define COMMENT_BIGTEXT "%1 hat beim Spiel %2:%3 am %4 einen neuen Kommentar geschrieben:\n%5"

qint64 PushNotification::sendNewMeetingComment(const QString comment, const qint32 userID, const GamesPlay* pGame)
{
    if (!this->m_initialized)
        return ERROR_CODE_NOT_READY;

    QString         date   = QDateTime::fromMSecsSinceEpoch(pGame->m_timestamp).toString("dd.MM.yy");
    PushNotifyInfo* push   = new PushNotifyInfo();
    push->m_topic          = PUSH_NOTIFY_TOPIC::PUSH_NOT_NEW_COMMENT;
    push->m_userEventTopic = PUSH_NOTIFY_TOPIC::PUSH_NOT_NEW_COMMENT;
    push->m_header         = "Neuer Kommentar";
    push->m_body           = QString(BODY_GAME).arg(pGame->m_itemName, pGame->m_away);
    push->m_summary        = this->m_pGlobalData->m_UserList.getReadableName(userID);
    push->m_bigText        = QString(COMMENT_BIGTEXT).arg(push->m_summary, pGame->m_itemName, pGame->m_away, date, comment);
    push->m_sendMessageID  = getNextInternalPushNumber();
    push->m_sendTime       = QDateTime::currentMSecsSinceEpoch();
    push->m_userID         = userID;
    push->m_info           = QString::number(pGame->m_index);

    this->insertNewNotification(push);

    return push->m_sendMessageID;
}

qint64 PushNotification::sendNewStadiumWebPageNotification(const QString body, const qint32 userID, const qint32 webPageIndex)
{
    if (!this->m_initialized)
        return ERROR_CODE_NOT_READY;

    foreach (PushNotifyInfo* p, this->m_lPushToSend) {
        if (p->m_userEventTopic == PUSH_NOTIFY_TOPIC::PUSH_NOT_STADIUM_WEBPAGE) {
            if (p->m_internalIndex1 == webPageIndex) {
                p->m_body    = body;
                p->m_bigText = QString("Es gibt neue Änderungen bei %1").arg(body);
                this->m_notifyMutex.unlock();
                return -1;
            }
        }
    }

    PushNotifyInfo* push   = new PushNotifyInfo();
    push->m_topic          = PUSH_NOTIFY_TOPIC::PUSH_NOT_STADIUM_WEBPAGE;
    push->m_userEventTopic = PUSH_NOTIFY_TOPIC::PUSH_NOT_STADIUM_WEBPAGE;
    push->m_header         = "Bautagebuch";
    push->m_body           = body;
    push->m_summary        = this->m_pGlobalData->m_UserList.getReadableName(userID);
    push->m_bigText        = QString("Es gibt neue Änderungen bei %1").arg(body);
    push->m_sendMessageID  = getNextInternalPushNumber();
    push->m_sendTime       = QDateTime::currentMSecsSinceEpoch() + WAIT_TIME_BEFORE_SEND;
    push->m_userID         = userID;
    push->m_internalIndex1 = webPageIndex;
    push->m_info           = QString::number(webPageIndex);

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

    if (this->m_doNotUseSSLbecauseOfVersion && !this->m_lastPushNotify->m_info.isEmpty()) {
        QString sendTopic = this->getTopicStringFromIndex(this->m_lastPushNotify->m_userEventTopic);
        this->m_pGlobalData->addNewUserEvent(sendTopic, this->m_lastPushNotify->m_info, this->m_lastPushNotify->m_userID);
    }
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
    jsonString.append(QString("\"summary\" : \"%1\",").arg(pushNotify->m_summary));
    jsonString.append(QString("\"bigText\" : \"%1\",").arg(pushNotify->m_bigText));
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

    QString body = pushNotify->m_body;
    if (!pushNotify->m_bigText.isEmpty())
        body.append(QString("\n%1").arg(pushNotify->m_bigText));

    g_SmtpManager.sendNewEmail(pushNotify->m_header, body);
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

        if (this->m_lastPushNotify != NULL && !this->m_lastPushNotify->m_info.isEmpty()) {
            QString sendTopic = this->getTopicStringFromIndex(this->m_lastPushNotify->m_userEventTopic);
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

    qDebug().noquote() << "There was an error sending the notification";
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
    //    case PUSH_NOT_CHG_MEETING:
    //        return NOTIFY_TOPIC_CHANGE_MEETING;
    case PUSH_NOT_NEW_TICKET:
        return NOTIFY_TOPIC_NEW_FREE_TICKET;
    //    case PUSH_NOT_NEW_AWAY_ACCEPT:
    //        return NOTIFY_TOPIC_NEW_AWAY_ACCEPT;
    case PUSH_NOT_NEW_FAN_NEWS:
        return NOTIFY_TOPIC_NEW_FANCLUB_NEWS;
    case PUSH_NOT_NEW_COMMENT:
        return NOTIFY_TOPIC_NEW_COMMENT;
    case PUSH_NOT_STADIUM_WEBPAGE:
        return NOTIFY_TOPIC_STADIUM_WEBPAGE;
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

qint32 PushNotification::addNewAppInformation(const QString guid, const QString fcmToken,
                                              const qint32 system, const quint32 userIndex)
{
    qint64       timestamp = QDateTime::currentMSecsSinceEpoch();
    AppTokenUID* app;
    if ((app = this->appInfoExists(guid)) != NULL) {

        this->m_mInternalInfoMutex.lock();

        bool bChanged = false;
        if (app->m_fcmToken != fcmToken) {
            if (this->updateItemValue(app, APP_TOKEN_TOKEN, QVariant(fcmToken)))
                app->m_fcmToken = fcmToken;

            if (this->updateItemValue(app, ITEM_TIMESTAMP, QVariant(timestamp)))
                app->m_timestamp = timestamp;

            bChanged = true;
        }

        if (app->m_userIndex != userIndex) {
            if (this->updateItemValue(app, APP_TOKEN_USERINDEX, QVariant(userIndex)))
                app->m_userIndex = userIndex;

            if (this->updateItemValue(app, ITEM_TIMESTAMP, QVariant(timestamp)))
                app->m_timestamp = timestamp;

            bChanged = true;
        }

        this->m_mInternalInfoMutex.unlock();

        if (bChanged)
            this->sortItemListByTimeAscending();

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
    this->m_pConfigSettings->setValue(APP_TOKEN_VERSION, "");

    this->m_pConfigSettings->endArray();
    this->m_pConfigSettings->endGroup();
    this->m_pConfigSettings->sync();

    this->m_mConfigIniMutex.unlock();

    app = new AppTokenUID(guid, fcmToken, timestamp, userIndex, system, newIndex, "");

    this->addNewAppToken(app, false);

    this->sortItemListByTimeAscending();

    qInfo().noquote() << QString("Added new App Information: %1").arg(guid);
    return newIndex;
}

qint32 PushNotification::addNewVersionInformation(const QString guid, const QString version)
{
    qint64       timestamp = QDateTime::currentMSecsSinceEpoch();
    AppTokenUID* app;
    if ((app = this->appInfoExists(guid)) != NULL) {

        this->m_mInternalInfoMutex.lock();

        bool bChanged = false;
        if (app->m_version != version) {
            if (this->updateItemValue(app, APP_TOKEN_VERSION, QVariant(version)))
                app->m_version = version;

            if (this->updateItemValue(app, ITEM_TIMESTAMP, QVariant(timestamp)))
                app->m_timestamp = timestamp;

            bChanged = true;
        }

        this->m_mInternalInfoMutex.unlock();

        if (bChanged)
            this->sortItemListByTimeAscending();

        return app->m_index;
    }

    return ERROR_CODE_SUCCESS;
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
            QString version   = this->m_pConfigSettings->value(APP_TOKEN_VERSION, "").toString();

            AppTokenUID* app = new AppTokenUID(guid, token, timestamp, userIndex, system, index, version);

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

    this->sortItemListByTimeAscending();
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

QString PushNotification::showCurrentTokenInformation(const QString cmd)
{
    QString      rValue;
    QMutexLocker locker(&this->m_mInternalInfoMutex);

    //    qint32 maxSizeName = 0, maxSizePlace = 0;
    //    for (int i = 0; i < this->getNumberOfInternalList(); i++) {
    //        AppTokenUID* app = (AppTokenUID*)(this->getItemFromArrayIndex(i));
    //        if (app == NULL)
    //            continue;

    //        if (app->m_itemName.size() > maxSizeName)
    //            maxSizeName = app->m_itemName.size();

    //    }

    for (int i = 0; i < this->getNumberOfInternalList(); i++) {
        AppTokenUID* app = (AppTokenUID*)(this->getItemFromArrayIndex(i));
        if (app == NULL)
            continue;
        QString date    = QDateTime::fromMSecsSinceEpoch(app->m_timestamp).toString("dd.MM.yyyy hh:mm");
        QString token   = app->m_fcmToken == "" ? "Kein Token" : app->m_fcmToken;
        QString version = app->m_version == "" ? "Keine Version" : app->m_version;
        QString output  = QString("%1: %2").arg(app->m_index, 2, 10).arg(date);
        output.append(QString(" - %1").arg(this->m_pGlobalData->m_UserList.getItemName(app->m_userIndex)));
        if (cmd == "token")
            output.append(QString("\n   %1").arg(token));
        else if (cmd == "guid")
            output.append(QString("\n   %1").arg(app->m_guid));
        else if (cmd == "version")
            output.append(QString("\n   %1 %2").arg(version, -15).arg(app->m_oSystem));

        rValue.append(output);
        rValue.append("\n");
    }
    return rValue;
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
        this->m_pConfigSettings->setValue(APP_TOKEN_VERSION, pItem->m_version);
    }

    this->m_pConfigSettings->endArray();
    this->m_pConfigSettings->endGroup();

    this->m_mConfigIniMutex.unlock();

    qInfo().noquote() << QString("saved current App Token Guid List with %1 entries").arg(this->getNumberOfInternalList());
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
