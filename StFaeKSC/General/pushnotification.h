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
#include "globaldata.h"

enum PUSH_NOTIFY_TOPIC {
    PUSH_NOT_NEW_VERSION = 1,
    PUSH_NOT_NEW_MEETING = 2,
    PUSH_NOT_CHG_MEETING = 3,
    PUSH_NOT_NEW_TICKET  = 4
};


struct PushNotifyInfo {
    PUSH_NOTIFY_TOPIC m_topic;
    QString           m_header;
    QString           m_body;
    qint64            m_sendMessageID;
    qint64            m_sendTime;
    qint32            m_userID;
    quint32           m_internalIndex1;
    quint32           m_internalIndex2;
};

class PushNotification : public BackgroundWorker
{
    Q_OBJECT
public:
    explicit PushNotification(QObject* parent = 0);
    ~PushNotification();

    void initialize(GlobalData* pGlobalData);

    qint64 sendNewVersionNotification(const QString header, const QString body);
    qint64 sendNewMeetingNotification(const QString header, const QString body, const qint32 userID, const quint32 gameIndex);
    qint64 sendChangeMeetingNotification(const QString header, const QString body, const qint32 userID, const quint32 gameIndex);
    qint64 sendNewTicketNotification(const QString header, const QString body, const qint32 userID, const quint32 gameIndex, const quint32 ticketIndex);
    qint64 removeNewTicketNotification(const quint32 gameIndex, const quint32 ticketIndex);


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
    GlobalData*            m_pGlobalData;
    BackgroundController   m_ctrlBackground;
    QByteArray             m_fcmServerKey;
    QSettings*             m_pPushSettings = NULL;
    QNetworkAccessManager* m_nam           = NULL;
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
};

extern PushNotification* g_pushNotify;

#endif // PUSHNOTIFICATION_H
