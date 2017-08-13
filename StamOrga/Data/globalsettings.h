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

#ifndef GLOBALSETTINGS_H
#define GLOBALSETTINGS_H

#include <QObject>
#include <QSettings>
#include <QStringList>
#include <QtGui/QGuiApplication>

#include "globaldata.h"

class GlobalSettings : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString debugIP READ debugIP WRITE setDebugIP NOTIFY debugIPChanged)
    Q_PROPERTY(QString debugIPWifi READ debugIPWifi WRITE setDebugIPWifi NOTIFY debugIPWifiChanged)
    Q_PROPERTY(bool useReadableName READ useReadableName WRITE setUseReadableName NOTIFY useReadableNameChanged)
    Q_PROPERTY(bool loadGameInfo READ loadGameInfo WRITE setLoadGameInfo NOTIFY loadGameInfoChanged)
    Q_PROPERTY(bool saveInfosOnApp READ saveInfosOnApp WRITE setSaveInfosOnApp NOTIFY saveInfosOnAppChanged)
public:
    explicit GlobalSettings(QObject* parent = 0);

    void initialize(GlobalData* pGlobalData, QGuiApplication* app);

    Q_INVOKABLE void saveGlobalSettings();

    QString debugIP()
    {
        QMutexLocker lock(&this->m_mutex);
        return this->m_debugIP;
    }
    void setDebugIP(QString ip)
    {
        if (this->m_debugIP != ip) {
            {
                QMutexLocker lock(&this->m_mutex);
                this->m_debugIP = ip;
            }
            emit debugIPChanged();
        }
    }

    QString debugIPWifi()
    {
        QMutexLocker lock(&this->m_mutex);
        return this->m_debugIPWifi;
    }
    void setDebugIPWifi(QString ip)
    {
        if (this->m_debugIPWifi != ip) {
            {
                QMutexLocker lock(&this->m_mutex);
                this->m_debugIPWifi = ip;
            }
            emit debugIPWifiChanged();
        }
    }

    bool useReadableName() { return this->m_useReadableName; }
    void setUseReadableName(bool enable)
    {
        this->m_useReadableName = enable;
        emit this->useReadableNameChanged();
    }

    bool loadGameInfo() { return this->m_loadGameInfo; }
    void setLoadGameInfo(bool load)
    {
        this->m_loadGameInfo = load;
        emit this->loadGameInfoChanged();
    }

    bool saveInfosOnApp() { return this->m_saveInfosOnApp; }
    void setSaveInfosOnApp(bool save)
    {
        this->m_saveInfosOnApp = save;
        emit this->saveInfosOnAppChanged();
    }

    Q_INVOKABLE QString getChangeDefaultFont() { return this->m_changeDefaultFont; }
    Q_INVOKABLE void setChangeDefaultFont(QString font);

    Q_INVOKABLE qint64 getCurrentFontIndex() { return this->m_currentFontIndex; }
    void setCurrentFontList(QStringList* list);

    Q_INVOKABLE QString getCurrentVersion();

    Q_INVOKABLE QString getVersionChangeInfo();

    Q_INVOKABLE bool isVersionChangeAlreadyShown();

    Q_INVOKABLE void checkNewStateChangedAtStart();

signals:
    void debugIPChanged();
    void debugIPWifiChanged();
    void useReadableNameChanged();
    void loadGameInfoChanged();
    void saveInfosOnAppChanged();
    void sendAppStateChangedToActive(quint32 value);

public slots:
    void stateFromAppChanged(Qt::ApplicationState state);

private:
    GlobalData*  m_pGlobalData;
    bool         m_useReadableName;
    bool         m_loadGameInfo;
    bool         m_saveInfosOnApp;
    QString      m_debugIP;
    QString      m_debugIPWifi;
    QMutex       m_mutex;
    qint64       m_lastGameInfoUpdate;
    QString      m_changeDefaultFont;
    qint64       m_currentFontIndex;
    QStringList* m_fontList = NULL;

    QString m_lastShownVersion;
};


extern GlobalSettings* g_GlobalSettings;

#endif // GLOBALSETTINGS_H
