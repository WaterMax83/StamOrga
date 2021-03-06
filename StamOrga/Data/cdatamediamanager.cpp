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

#include <QtCore/QBuffer>
#include <QtCore/QDataStream>
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

qint32 cDataMediaManager::startGetPictureList(const qint32 gameIndex)
{
    if (!this->m_initialized)
        return ERROR_CODE_NOT_INITIALIZED;

    QMutexLocker lock(&this->m_mutex);

    QJsonObject rootObj;
    rootObj.insert("cmd", "list");
    rootObj.insert("type", "Media");
    rootObj.insert("version", "V1.0");
    rootObj.insert("gameIndex", gameIndex);

    TcpDataConRequest* req = new TcpDataConRequest(OP_CODE_CMD_REQ::REQ_CMD_MEDIA);
    req->m_lData           = QJsonDocument(rootObj).toJson(QJsonDocument::Compact);

    g_ConManager->sendNewRequest(req);
    return ERROR_CODE_SUCCESS;
}

qint32 cDataMediaManager::startAddPicture(const qint32 gameIndex, QString url)
{
    if (!this->m_initialized)
        return ERROR_CODE_NOT_INITIALIZED;

    QMutexLocker lock(&this->m_mutex);

    QImage media;
    media.load(url);
    if (media.isNull())
        return ERROR_CODE_NOT_POSSIBLE;

    qInfo() << QString("%1 %2").arg(media.size().width()).arg(media.size().height());

    if (media.size().width() > media.size().height()) {
        if (media.size().width() > 1024)
            media = media.scaledToWidth(1024, Qt::SmoothTransformation);
    } else {
        if (media.size().height() > 1024)
            media = media.scaledToHeight(1024, Qt::SmoothTransformation);
    }

    qInfo() << QString("%1 %2").arg(media.size().width()).arg(media.size().height());
    QByteArray fileEnding = url.right(url.size() - (url.lastIndexOf(".") + 1)).toUtf8();

    QByteArray  data;
    QDataStream stream(&data, QIODevice::WriteOnly);
    media.save(stream.device(), fileEnding);

    QByteArray data1 = qCompress(data, 9);

    qInfo() << QString("Size %1 %2 %3%").arg(data.size()).arg(data1.size()).arg((data1.size() * 100) / data.size());

    QJsonObject rootObj;
    rootObj.insert("cmd", "add");
    rootObj.insert("type", "Media");
    rootObj.insert("version", "V1.0");
    rootObj.insert("format", QString(fileEnding));
    rootObj.insert("data", QString(data.toHex()));
    rootObj.insert("gameIndex", gameIndex);

    TcpDataConRequest* req = new TcpDataConRequest(OP_CODE_CMD_REQ::REQ_CMD_MEDIA);
    req->m_lData           = QJsonDocument(rootObj).toJson(QJsonDocument::Compact);

    g_ConManager->sendNewRequest(req);
    return ERROR_CODE_SUCCESS;
}

qint32 cDataMediaManager::startDeletePictures(const qint32 gameIndex, QStringList list)
{
    if (!this->m_initialized)
        return ERROR_CODE_NOT_INITIALIZED;

    QMutexLocker lock(&this->m_mutex);

    QJsonObject rootObj;
    rootObj.insert("cmd", "delete");
    rootObj.insert("type", "Media");
    rootObj.insert("version", "V1.0");
    rootObj.insert("gameIndex", gameIndex);

    QJsonArray picsArr;
    foreach (QString pic, list) {
        pic.replace("image://media/", "");
        picsArr.append(pic);
    }
    rootObj.insert("media", picsArr);

    TcpDataConRequest* req = new TcpDataConRequest(OP_CODE_CMD_REQ::REQ_CMD_MEDIA);
    req->m_lData           = QJsonDocument(rootObj).toJson(QJsonDocument::Compact);

    g_ConManager->sendNewRequest(req);
    return ERROR_CODE_SUCCESS;

    return 0;
}

qint32 cDataMediaManager::handleMediaCommandResponse(MessageProtocol* msg)
{
    if (!this->m_initialized)
        return ERROR_CODE_NOT_INITIALIZED;

    QMutexLocker lock(&this->m_mutex);

    QByteArray  data(msg->getPointerToData());
    QJsonObject rootObj = QJsonDocument::fromJson(data).object();

    qint32  result  = rootObj.value("ack").toInt(ERROR_CODE_NOT_FOUND);
    QString command = rootObj.value("cmd").toString();

    if (result != ERROR_CODE_SUCCESS)
        return result;

    if (command == "add" || command == "delete") {

    } else if (command == "list") {
        for (int i = 0; i < this->m_files.size(); i++)
            delete this->m_files.at(i);
        this->m_files.clear();

        QJsonArray mediaArr = rootObj.value("media").toArray();
        for (int i = 0; i < mediaArr.size(); i++) {
            MediaContent* pContent = new MediaContent();
            pContent->m_fileUrl    = mediaArr.at(i).toObject().value("file").toString("");
            pContent->m_data       = QByteArray::fromHex(mediaArr.at(i).toObject().value("data").toString("").toUtf8());

            this->m_files.append(pContent);
        }
    } else
        result = ERROR_CODE_NOT_IMPLEMENTED;

    return result;
}

qint32 cDataMediaManager::getMediaCount()
{
    QMutexLocker lock(&this->m_mutex);

    return this->m_files.size();
}


QString cDataMediaManager::getMediaFileString(qint32 index)
{
    QMutexLocker lock(&this->m_mutex);

    if (index >= this->m_files.size())
        return "";

    return this->m_files.at(index)->m_fileUrl;
}
