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

#include <QtCore/QFile>
#include <QtCore/QJsonArray>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>
#include <QtGui/QImage>

#include "../Common/General/globalfunctions.h"
#include "../Common/Network/messagecommand.h"
#include "../Connection/cconmanager.h"
#include "cdatamediamanager.h"

cDataMediaManager* g_DataMediaManager;

cDataMediaManager::cDataMediaManager(QObject* parent)
    : cGenDisposer(parent)
{
}


qint32 cDataMediaManager::initialize()
{
    this->m_initialized = true;

    return ERROR_CODE_SUCCESS;
}

qint32 cDataMediaManager::startAddPicture(const qint32 gameIndex, QString url)
{
    if (!this->m_initialized)
        return ERROR_CODE_NOT_INITIALIZED;

    QMutexLocker lock(&this->m_mutex);

    QJsonObject rootObj;
    //    rootObj.insert("timestamp", this->m_stLastServerUpdateTimeStamp);
    //    rootObj.insert("pastGames", pastGames);
    QImage media;
    qInfo() << media.load(url);

    QFile file(url);
    qInfo() << file.open(QIODevice::ReadOnly);


    if (media.isNull())
        return ERROR_CODE_NOT_POSSIBLE;

    qInfo() << QString("%1 %2").arg(media.size().width()).arg(media.size().height());

    return ERROR_CODE_SUCCESS;

    TcpDataConRequest* req = new TcpDataConRequest(OP_CODE_CMD_REQ::REQ_CMD_MEDIA);
    req->m_lData           = QJsonDocument(rootObj).toJson(QJsonDocument::Compact);

    g_ConManager->sendNewRequest(req);
    return ERROR_CODE_SUCCESS;
}

qint32 cDataMediaManager::handleMediaCommandResponse(MessageProtocol* msg)
{
    if (!this->m_initialized)
        return ERROR_CODE_NOT_INITIALIZED;

    QByteArray  data(msg->getPointerToData());
    QJsonObject rootObj = QJsonDocument::fromJson(data).object();

    qint32 result = rootObj.value("ack").toInt(ERROR_CODE_NOT_FOUND);
    //    qint32     updateIndex  = rootObj.value("index").toInt(UpdateAll);
    //    qint64     timestamp    = (qint64)rootObj.value("timestamp").toDouble(0);
    //    QJsonArray arrGames     = rootObj.value("games").toArray();

    return result;
}
