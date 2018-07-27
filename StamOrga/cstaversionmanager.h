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

#ifndef CDATAUPDATEMANAGER_H
#define CDATAUPDATEMANAGER_H

#include <QObject>

#include "../Common/General/cgendisposer.h"
#include "../Common/Network/messageprotocol.h"

class cStaVersionManager : public cGenDisposer
{
    Q_OBJECT
    Q_PROPERTY(qint32 versionUpdateIndex READ versionUpdateIndex NOTIFY versionUpdateIndexChanged)
    Q_PROPERTY(QString remoteVersion READ getRemoteVersion NOTIFY versionUpdateIndexChanged)
    Q_PROPERTY(double currentProgress READ currentProgress NOTIFY currentProgressChanged)
public:
    cStaVersionManager(QObject* parent = nullptr);

    qint32 initialize() override;

    Q_INVOKABLE qint32 startGettingVersionInfo();
    qint32 handleVersionResponse(MessageProtocol* msg);

    Q_INVOKABLE qint32 startDownloadCurrentVersion();
    Q_INVOKABLE qint32 startInstallCurrentVersion();

    Q_INVOKABLE bool isVersionChangeAlreadyShown();
    Q_INVOKABLE QString getVersionChangeInfo();

    QString     getRemoteVersion();
    Q_INVOKABLE QString getUpdateLink();
    Q_INVOKABLE QString getVersionInfo();

    Q_INVOKABLE QString getCurrentVersion();
    Q_INVOKABLE QString getCurrentVersionLink();

    double currentProgress() { return this->m_progress; }

    qint32 versionUpdateIndex() { return this->m_versionUpdateIndex; }

signals:
    void versionUpdateIndexChanged(void);
    void currentProgressChanged(void);
    void signalVersionDownloadFinished(qint32 result);


private slots:
    void slotDownloadProgress(qint64 current, qint64 max);
    void slotDownloadFinished(QString url, qint32 statusCode);

private:
    QString m_versionInfo;
    QString m_remoteVersion;
    QString m_updateLink;
    QString m_lastShownVersion;
    qint32  m_versionUpdateIndex;
    double  m_progress;

    QString m_downloadURL;
    QString m_downloadSavePath;
};

extern cStaVersionManager* g_StaVersionManager;

#endif // CDATAUPDATEMANAGER_H
