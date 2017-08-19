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

#include <QObject>
#include <QSettings>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QList>
#include <QMutex>

#include "../Common/General/backgroundcontroller.h"
#include "../Common/General/backgroundworker.h"
#include "globaldata.h"


struct PushNotifyInfo {
    QString m_topic;
    QString m_header;
    QString m_body;
};

class PushNotification : public BackgroundWorker
{
    Q_OBJECT
public:
    explicit PushNotification(QObject* parent = 0);
    ~PushNotification();

    void initialize(GlobalData* pGlobalData);

signals:

public slots:
    void slotSendNewNotification(const QString topic, const QString header, const QString body);
    void finished(QNetworkReply * reply);
    void sslErrors(QNetworkReply* reply, QList<QSslError> errors);

protected:
    int DoBackgroundWork();

private:
    GlobalData*            m_pGlobalData;
    BackgroundController   m_ctrlBackground;
    QByteArray             m_fcmServerKey;
    QSettings*             m_pPushSettings;
    QNetworkAccessManager  *m_nam;
    QUrl                   m_fcmServiceUrl;
    QList<PushNotifyInfo*> m_lPushToSend;
    PushNotifyInfo         m_lastPushNotify;
    QMutex                 m_sendMutex;

    void sendNewPushNotify(PushNotifyInfo *pushNotify);
};

#endif // PUSHNOTIFICATION_H
