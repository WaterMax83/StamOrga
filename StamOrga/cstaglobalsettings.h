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

#ifndef CSTAGLOBALSETTINGS_H
#define CSTAGLOBALSETTINGS_H

#include <QObject>
#include <QtGui/QGuiApplication>
#include <QtNetwork/QHostInfo>
#include <QtNetwork/QSslSocket>

#include "../Common/General/cgendisposer.h"
#include "../Common/Network/messageprotocol.h"

class cStaGlobalSettings : public cGenDisposer
{
    Q_OBJECT
public:
    explicit cStaGlobalSettings(QObject* parent = nullptr);

    qint32 initialize() override;

    Q_INVOKABLE bool getSaveInfosOnApp();
    Q_INVOKABLE void setSaveInfosOnApp(const bool save);

    Q_INVOKABLE bool getLoadGameInfos();
    Q_INVOKABLE void setLoadGameInfos(const bool load);
    Q_INVOKABLE void checkNewStateChangedAtStart();

    Q_INVOKABLE bool getUseVersionPopup();
    Q_INVOKABLE void setUseVersionPopup(const bool use);

    Q_INVOKABLE QString getDebugIP();
    Q_INVOKABLE void setDebugIP(const QString ip);

    Q_INVOKABLE QString getDebugIPWifi();
    Q_INVOKABLE void setDebugIPWifi(const QString ip);

    Q_INVOKABLE QString getChangeDefaultFont();
    Q_INVOKABLE void setChangeDefaultFont(const QString font);

    Q_INVOKABLE qint32 getCurrentFontIndex();
    void setCurrentFontList(QStringList* list);

    Q_INVOKABLE qint64 getKeepPastItemsCount();

    bool             getUseSSL(void);
    Q_INVOKABLE bool getUseSSLSettings(void);
    Q_INVOKABLE void setUseSSL(bool useSSL);
    Q_INVOKABLE bool getCanUseSSL(void);
    QSslCertificate  getSSLCaCertificate(void);

    void setAlreadyConnected(const bool con);

    Q_INVOKABLE bool isNotificationNewAppVersionEnabled();
    Q_INVOKABLE bool isNotificationNewMeetingEnabled();
    Q_INVOKABLE bool isNotificationNewFreeTicketEnabled();
    Q_INVOKABLE bool isNotificationMeetingCommentEnabled();
    Q_INVOKABLE bool isNotificationFanclubNewsEnabled();
    Q_INVOKABLE void setNotificationNewAppVersionEnabled(bool enable);
    Q_INVOKABLE void setNotificationNewMeetingEnabled(bool enable);
    Q_INVOKABLE void setNotificationNewFreeTicketEnabled(bool enabled);
    Q_INVOKABLE void setNotificationMeetingCommentEnabled(bool enable);
    Q_INVOKABLE void setNotificationFanclubNewsEnabled(bool enable);


    bool isIpAddressAlreadySet() { return this->m_bIpAddressWasSet; }

    void updatePushNotification(void);

signals:

public slots:
    void slotStateFromAppChanged(Qt::ApplicationState state);
    void slotCallBackLookUpHost(const QHostInfo& host);

private:
    QString         m_debugIP;
    QString         m_debugIPWifi;
    bool            m_bSaveInfosOnApp;
    bool            m_bLoadGameInfo;
    bool            m_bUseVersionPopup;
    bool            m_bIpAddressWasSet;
    bool            m_bUseSSL;
    bool            m_bDisableSSLFrom_Start;
    bool            m_bCanUseSSL;
    QSslCertificate m_caCert;
    quint64         m_notificationEnabledValue;
    qint64          m_iKeepPastItemsCount;


    QString      m_changeDefaultFont;
    qint64       m_currentFontIndex;
    QStringList* m_fontList = NULL;

    bool m_bAlreadyConnected;
};

extern cStaGlobalSettings* g_StaGlobalSettings;

#endif // CSTAGLOBALSETTINGS_H
