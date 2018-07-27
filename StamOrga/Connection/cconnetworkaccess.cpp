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

#include <QtNetwork/QNetworkReply>

#include "General/globalfunctions.h"
#include "cconnetworkaccess.h"

cConNetworkAccess* g_ConNetworkAccess;

cConNetworkAccess::cConNetworkAccess(QObject* parent)
    : BackgroundWorker(parent)
{
}

qint32 cConNetworkAccess::initialize()
{
    this->m_ctrlNetWork.Start(this, false);

    return ERROR_CODE_SUCCESS;
}

int cConNetworkAccess::DoBackgroundWork()
{
    this->m_nam = new QNetworkAccessManager();

    connect(this, &cConNetworkAccess::signalStartDownload, this, &cConNetworkAccess::slotStartDownload);
    connect(this->m_nam, &QNetworkAccessManager::finished, this, &cConNetworkAccess::slotDownloadFinished);

    this->m_initialized = true;

    return ERROR_CODE_SUCCESS;
}

void cConNetworkAccess::slotStartDownload(QString url)
{
    qInfo() << "Start to download " << url;
    cConNADownload* pDownload = new cConNADownload();
    pDownload->m_url          = url;
    this->m_downloads.append(pDownload);

    this->m_nam->get(QNetworkRequest(QUrl(url)));
}

void cConNetworkAccess::slotDownloadFinished(QNetworkReply* reply)
{
    if (reply->error()) {
        for (int i = 0; i < this->m_downloads.size(); i++) {
            cConNADownload* pDownload = this->m_downloads.at(i);
            if (reply->url().toString() == pDownload->m_url || reply->url().toString() == pDownload->m_redirectUrl) {
                emit this->signalDownloadFinished(pDownload->m_url, ERROR_CODE_COMMON);
                delete pDownload;
                this->m_downloads.removeAt(i);
                break;
            }
        }
        qCritical().noquote() << QString("Error downloading file %1: %2").arg(reply->url().toString(), reply->errorString());
    } else {
        int statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
        if (statusCode == 301 || statusCode == 302 || statusCode == 303
            || statusCode == 305 || statusCode == 307 || statusCode == 308) {
            qInfo() << "Redirecting Status Code for Download: " << statusCode;

            foreach (cConNADownload* pDownload, this->m_downloads) {
                if (reply->url().toString() == pDownload->m_url || reply->url().toString() == pDownload->m_redirectUrl) {
                    pDownload->m_redirectUrl = reply->attribute(QNetworkRequest::RedirectionTargetAttribute).toUrl().toString();
                    break;
                }
            }

            this->m_nam->get(QNetworkRequest(reply->attribute(QNetworkRequest::RedirectionTargetAttribute).toUrl()));
        } else {
            qInfo() << statusCode;
            foreach (cConNADownload* pDownload, this->m_downloads) {
                if (reply->url().toString() == pDownload->m_url || reply->url().toString() == pDownload->m_redirectUrl) {
                    pDownload->m_data = reply->readAll();
                    emit this->signalDownloadFinished(pDownload->m_url, ERROR_CODE_SUCCESS);
                    break;
                }
            }
        }
    }
}
