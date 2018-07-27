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

#ifndef CCONNETWORKACCESS_H
#define CCONNETWORKACCESS_H

#include <QObject>
#include <QtCore/QList>
#include <QtNetwork/QNetworkAccessManager>

#include "../../Common/General/backgroundcontroller.h"
#include "../Common/General/backgroundworker.h"

struct cConNADownload {
    QString    m_url;
    QString    m_redirectUrl;
    QByteArray m_data;
};

class cConNetworkAccess : public BackgroundWorker
{
    Q_OBJECT
public:
    explicit cConNetworkAccess(QObject* parent = nullptr);

    qint32 initialize() override;

    int DoBackgroundWork() override;

    qint32 getDownload(const QString url, QByteArray& data);

signals:
    void signalStartDownload(QString url);
    void signalDownloadFinished(QString url, qint32 statusCode);
    void signalDownloadProgress(qint64 current, qint64 max);

private slots:
    void slotStartDownload(QString url);
    void slotDownloadFinished(QNetworkReply* reply);
    void slotDownloadProgress(qint64 current, qint64 max);

private:
    BackgroundController   m_ctrlNetWork;
    QNetworkAccessManager* m_nam;
    QList<cConNADownload*> m_downloads;
};

extern cConNetworkAccess* g_ConNetworkAccess;

#endif // CCONNETWORKACCESS_H
