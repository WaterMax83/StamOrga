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

    Q_INVOKABLE bool isVersionChangeAlreadyShown();
    Q_INVOKABLE QString getVersionChangeInfo();

    qint32 startGettingVersionInfo();
    qint32 handleVersionResponse(MessageProtocol* msg);

    void setAlreadyConnected(const bool con);

    QString     getRemoteVersion();
    Q_INVOKABLE QString getUpdateLink();
    Q_INVOKABLE QString getVersionInfo();

    Q_INVOKABLE QString getCurrentVersion();
    Q_INVOKABLE QString getCurrentVersionLink();

    Q_INVOKABLE bool isNotificationNewAppVersionEnabled();
    Q_INVOKABLE bool isNotificationNewMeetingEnabled();
    Q_INVOKABLE bool isNotificationChangedMeetingEnabled();
    Q_INVOKABLE bool isNotificationNewFreeTicketEnabled();
    Q_INVOKABLE bool isNotificationNewAwayAcceptEnabled();
    Q_INVOKABLE bool isNotificationFanclubNewsEnabled();
    Q_INVOKABLE void setNotificationNewAppVersionEnabled(bool enable);
    Q_INVOKABLE void setNotificationNewMeetingEnabled(bool enable);
    Q_INVOKABLE void setNotificationChangedMeetingEnabled(bool enable);
    Q_INVOKABLE void setNotificationNewFreeTicketEnabled(bool enabled);
    Q_INVOKABLE void setNotificationNewAwayAcceptEnabled(bool enable);
    Q_INVOKABLE void setNotificationFanclubNewsEnabled(bool enable);


    bool isIpAddressAlreadySet() { return this->m_bIpAddressWasSet; }

signals:

public slots:
    void slotStateFromAppChanged(Qt::ApplicationState state);
    void slotCallBackLookUpHost(const QHostInfo& host);

private:
    QString m_versionInfo;
    QString m_remoteVersion;
    QString m_updateLink;
    QString m_lastShownVersion;
    QString m_debugIP;
    QString m_debugIPWifi;
    bool    m_bSaveInfosOnApp;
    bool    m_bLoadGameInfo;
    bool    m_bUseVersionPopup;
    bool    m_bIpAddressWasSet;
    quint64 m_notificationEnabledValue;

    QString      m_changeDefaultFont;
    qint64       m_currentFontIndex;
    QStringList* m_fontList = NULL;

    bool m_bAlreadyConnected;

    void updatePushNotification(void);
};

extern cStaGlobalSettings* g_StaGlobalSettings;

#endif // CSTAGLOBALSETTINGS_H
