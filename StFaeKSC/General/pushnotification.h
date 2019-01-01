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


#ifndef PUSHNOTIFICATION_H
#define PUSHNOTIFICATION_H

#include <QList>
#include <QMutex>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QObject>
#include <QSettings>
#include <QTimer>

#include "../Common/General/backgroundcontroller.h"
#include "../Common/General/backgroundworker.h"
#include "../Data/configlist.h"
#include "globaldata.h"

enum PUSH_NOTIFY_TOPIC {
    PUSH_NOT_GEN_TOPIC       = 0,
    PUSH_NOT_NEW_VERSION     = 1,
    PUSH_NOT_NEW_MEETING     = 2,
    PUSH_NOT_CHG_MEETING     = 3,
    PUSH_NOT_NEW_TICKET      = 4,
    PUSH_NOT_NEW_AWAY_ACCEPT = 5,
    PUSH_NOT_NEW_FAN_NEWS    = 6,
    PUSH_NOT_NEW_COMMENT     = 7,
    PUSH_NOT_STADIUM_WEBPAGE = 8
};


struct PushNotifyInfo {
    PUSH_NOTIFY_TOPIC m_topic;
    PUSH_NOTIFY_TOPIC m_userEventTopic;
    QString           m_header;
    QString           m_body;
    QString           m_bigText;
    QString           m_summary;
    qint64            m_sendMessageID;
    qint64            m_sendTime;
    qint32            m_userID;
    qint32           m_internalIndex1;
    qint32           m_internalIndex2;

    /* For user Events */
    QString m_info;
};

class AppTokenUID : public ConfigItem
{
public:
    QString m_guid;
    QString m_fcmToken;
    quint32 m_userIndex;
    qint32  m_oSystem;
    QString m_version;

    AppTokenUID(QString guid, QString token, qint64 timestamp, quint32 userIndex,
                qint32 system, quint32 index, QString version)
    {
        this->m_itemName  = "";
        this->m_timestamp = timestamp;
        this->m_index     = index;

        this->m_userIndex = userIndex;
        this->m_guid      = guid;
        this->m_fcmToken  = token;
        this->m_oSystem   = system;
        this->m_version   = version;
    }
};

// clang-format off
#define APP_TOKEN_GUID      "Guid"
#define APP_TOKEN_TOKEN     "Token"
#define APP_TOKEN_USERINDEX "UserIndex"
#define APP_TOKEN_SYSTEM    "OS"
#define APP_TOKEN_VERSION   "Version"
// clang-format on

class PushNotification : public BackgroundWorker, public ConfigList
{
    Q_OBJECT
public:
    explicit PushNotification(QObject* parent = 0);
    ~PushNotification();

    void initialize(GlobalData* pGlobalData);

    qint32 addNewAppInformation(const QString guid, const QString fcmToken, const qint32 system, const quint32 userIndex);
    qint32 addNewVersionInformation(const QString guid, const QString version);

    QString showCurrentTokenInformation(const QString cmd);

    qint64 sendNewGeneralTopicNotification(const QString header, const QString body, const QString bigText);
    qint64 sendNewVersionNotification();
    qint64 sendNewMeetingNotification(const GamesPlay* pGame, QString& bigText, const qint32 userID, const qint32 type);
    qint64 sendChangeMeetingNotification(const GamesPlay* pGame, QString& bigText, const qint32 userID, const qint32 type);
    qint64 sendNewTicketNotification(const GamesPlay* pGame, const qint32 userID, const qint32 ticketIndex);
    qint64 removeNewTicketNotification(const qint32 gameIndex, const qint32 ticketIndex);
    qint64 sendNewFirstAwayAccept(const QString bigText, const qint32 userID, const qint32 gameIndex);
    qint64 sendNewFanclubNewsNotification(const QString body, const QString bigText, const qint32 userID, const qint32 newsID);
    qint64 sendNewMeetingComment(const QString comment, const qint32 userID, const GamesPlay* pGame);
    qint64 sendNewStadiumWebPageNotification(const QString body, const qint32 userID, const qint32 webPageIndex);

    virtual qint32 checkConsistency() { return -12; }

signals:
    void sendNewNotificationSignal(void);

public slots:
    void slotSendNewNotification();
    void slotConnectionTimeout();
    void finished(QNetworkReply* reply);
    void sslErrors(QNetworkReply* reply, QList<QSslError> errors);

protected:
    int DoBackgroundWork();

private:
    bool                   m_initialized;
    GlobalData*            m_pGlobalData;
    BackgroundController   m_ctrlBackground;
    QByteArray             m_fcmServerKey;
    QNetworkAccessManager* m_nam = NULL;
    QUrl                   m_fcmServiceUrl;
    QList<PushNotifyInfo*> m_lPushToSend;
    PushNotifyInfo*        m_lastPushNotify;
    QMutex                 m_notifyMutex;
    QTimer*                m_restartSendTimer            = NULL;
    QTimer*                m_connectionTimeoutTimer      = NULL;
    bool                   m_doNotUseSSLbecauseOfVersion = false;

    void startSendNewPushNotify(PushNotifyInfo* pushNotify);

    void insertNewNotification(PushNotifyInfo* push);

    QString getTopicStringFromIndex(const PUSH_NOTIFY_TOPIC topic);

    qint64 getNextInternalPushNumber();

    void loadCurrentInteralList();

    bool addNewAppToken(AppTokenUID* app, bool checkApp = true);

    AppTokenUID* appInfoExists(QString guid);

    void saveCurrentInteralList() override;
};

extern PushNotification* g_pushNotify;

#endif // PUSHNOTIFICATION_H
