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

#include <QtCore/QJsonArray>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>
#include <QtQml/QQmlEngine>

#include "cdatanewsdatamanager.h"

#include "../Common/General/globalfunctions.h"
#include "../Common/General/globaltiming.h"
#include "../Common/Network/messagecommand.h"
#include "../Connection/cconmanager.h"
#include "../Connection/cconusersettings.h"
//#include "../cstaglobalsettings.h"


cDataNewsDataManager g_DataNewsDataManager;

cDataNewsDataManager::cDataNewsDataManager(QObject* parent)
    : cGenDisposer(parent)
{
}

qint32 cDataNewsDataManager::initialize()
{
    this->m_stLastLocalUpdateTimeStamp  = 0;
    this->m_stLastServerUpdateTimeStamp = 0;

    this->m_initialized = true;

    return ERROR_CODE_SUCCESS;
}


void cDataNewsDataManager::addNewNewsData(NewsDataItem* sNews, const quint16 updateIndex)
{
    if (!this->m_initialized)
        return;

    NewsDataItem* pNews = this->getNewsDataItem(sNews->index());
    if (pNews == NULL) {
        QMutexLocker lock(&this->m_mutex);
        this->m_lNews.append(sNews);
        return;
    } else if (updateIndex == UpdateIndex::UpdateDiff) {
        if (pNews->user() != sNews->user()) {
            pNews->setUser(sNews->user());
        }
        if (pNews->header() != sNews->header()) {
            pNews->setHeader(sNews->header());
        }
        if (pNews->info() != sNews->info()) {
            pNews->setInfo(sNews->info());
        }
    }
}

NewsDataItem* cDataNewsDataManager::getNewsDataItem(qint32 newsIndex)
{
    if (!this->m_initialized)
        return NULL;

    QMutexLocker lock(&this->m_mutex);

    for (int i = 0; i < this->m_lNews.size(); i++) {
        if (this->m_lNews[i]->index() == newsIndex)
            return this->m_lNews[i];
    }
    return NULL;
}

qint32 cDataNewsDataManager::getNewsDataLength()
{
    QMutexLocker lock(&this->m_mutex);
    return this->m_lNews.size();
}

NewsDataItem* cDataNewsDataManager::getNewsDataFromArrayIndex(int index)
{
    QMutexLocker lock(&this->m_mutex);

    if (index < this->m_lNews.size()) {
        return this->m_lNews.at(index);
    }
    return NULL;
}

QString cDataNewsDataManager::getNewsDataLastLocalUpdateString()
{
    if (!this->m_initialized)
        return "";

    QMutexLocker lock(&this->m_mutex);
    return QDateTime::fromMSecsSinceEpoch(this->m_stLastLocalUpdateTimeStamp).toString("dd.MM.yy hh:mm:ss");
}

qint32 cDataNewsDataManager::startListNewsData()
{
    if (!this->m_initialized)
        return ERROR_CODE_NOT_INITIALIZED;

    QMutexLocker lock(&this->m_mutex);

    QJsonObject rootObj;
    if (this->m_stLastLocalUpdateTimeStamp + TIMEOUT_UPDATE_TICKETS > QDateTime::currentMSecsSinceEpoch() && this->m_lNews.count() > 0)
        rootObj.insert("index", UpdateIndex::UpdateDiff);
    else
        rootObj.insert("index", UpdateIndex::UpdateAll);

    rootObj.insert("timestamp", this->m_stLastServerUpdateTimeStamp);

    TcpDataConRequest* req = new TcpDataConRequest(OP_CODE_CMD_REQ::REQ_GET_NEWS_DATA_LIST);
    req->m_lData           = QJsonDocument(rootObj).toJson(QJsonDocument::Compact);

    g_ConManager.sendNewRequest(req);
    return ERROR_CODE_SUCCESS;
}

qint32 cDataNewsDataManager::handleListNewsDataResponse(MessageProtocol* msg)
{
    if (!this->m_initialized)
        return ERROR_CODE_NOT_INITIALIZED;

    QByteArray  data(msg->getPointerToData());
    QJsonObject rootObj = QJsonDocument::fromJson(data).object();

    qint32     result      = rootObj.value("ack").toInt(ERROR_CODE_NOT_FOUND);
    qint32     updateIndex = rootObj.value("index").toInt(UpdateAll);
    qint64     timestamp   = (qint64)rootObj.value("timestamp").toDouble(0);
    QJsonArray arrNews     = rootObj.value("news").toArray();

    this->m_mutex.lock();

    if (updateIndex == UpdateIndex::UpdateAll) {
        for (int i = 0; i < this->m_lNews.size(); i++)
            delete this->m_lNews[i];
        this->m_lNews.clear();
    }
    this->m_stLastLocalUpdateTimeStamp = QDateTime::currentMSecsSinceEpoch();

    this->m_mutex.unlock();

    for (int i = 0; i < arrNews.count(); i++) {
        QJsonObject   newsObj = arrNews.at(i).toObject();
        NewsDataItem* pNews   = new NewsDataItem();

        pNews->setIndex(newsObj.value("index").toInt());
        pNews->setTimeStamp((qint64)newsObj.value("timestamp").toDouble());
        pNews->setHeader(newsObj.value("header").toString());
        pNews->setUser(newsObj.value("user").toString());

        QQmlEngine::setObjectOwnership(pNews, QQmlEngine::CppOwnership);
        this->addNewNewsData(pNews, updateIndex);
    }

    std::sort(this->m_lNews.begin(), this->m_lNews.end(), NewsDataItem::compareTimeStampFunctionDescending);

    this->m_stLastServerUpdateTimeStamp = timestamp;

    return result;
}

qint32 cDataNewsDataManager::startGetNewsDataItem(qint32 index)
{
    if (!this->m_initialized)
        return ERROR_CODE_NOT_INITIALIZED;

    QJsonObject rootObj;
    rootObj.insert("index", index);

    TcpDataConRequest* req = new TcpDataConRequest(OP_CODE_CMD_REQ::REQ_GET_NEWS_DATA_ITEM);
    req->m_lData           = QJsonDocument(rootObj).toJson(QJsonDocument::Compact);

    g_ConManager.sendNewRequest(req);
    return ERROR_CODE_SUCCESS;
}

qint32 cDataNewsDataManager::handleGetNewsDataItem(MessageProtocol* msg)
{
    if (!this->m_initialized)
        return ERROR_CODE_NOT_INITIALIZED;

    QByteArray  data(msg->getPointerToData());
    QJsonObject rootObj = QJsonDocument::fromJson(data).object();

    qint32 result = rootObj.value("ack").toInt(ERROR_CODE_NOT_FOUND);
    if (result != ERROR_CODE_SUCCESS)
        return result;
    qint32 newsIndex = rootObj.value("index").toInt(0);

    NewsDataItem* pItem = this->getNewsDataItem(newsIndex);
    if (pItem == NULL)
        return ERROR_CODE_NOT_FOUND;

    QMutexLocker lock(&this->m_mutex);

    pItem->setTimeStamp((qint64)rootObj.value("timestamp").toDouble());
    pItem->setUser(rootObj.value("user").toString());
    pItem->setHeader(rootObj.value("header").toString());

    QByteArray cryptInfo = QByteArray::fromHex(rootObj.value("info").toString().toUtf8());
    if (cryptInfo.size() > 0)
        pItem->setInfo(QString(qUncompress(cryptInfo)));
    else
        pItem->setInfo("");

    std::sort(this->m_lNews.begin(), this->m_lNews.end(), NewsDataItem::compareTimeStampFunctionDescending);

    return result;
}


qint32 cDataNewsDataManager::startChangeNewsDataItem(const qint32 index, const QString header, const QString info)
{
    if (!this->m_initialized)
        return ERROR_CODE_NOT_INITIALIZED;

    QJsonObject rootObj;
    rootObj.insert("index", index);
    rootObj.insert("header", header);
    QByteArray dataInfo = qCompress(info.toUtf8(), 9);
    rootObj.insert("info", QString(dataInfo.toHex()));

    this->m_editHeader = header;
    this->m_editInfo   = info;

    qInfo().noquote() << QString("Compressed news info from %1 to %2 Bytes").arg(info.size()).arg(dataInfo.toHex().size());

    TcpDataConRequest* req = new TcpDataConRequest(OP_CODE_CMD_REQ::REQ_CHANGE_NEWS_DATA);
    req->m_lData           = QJsonDocument(rootObj).toJson(QJsonDocument::Compact);

    g_ConManager.sendNewRequest(req);
    return ERROR_CODE_SUCCESS;
}

qint32 cDataNewsDataManager::handleChangeNewsDataResponse(MessageProtocol* msg)
{
    if (!this->m_initialized)
        return ERROR_CODE_NOT_INITIALIZED;

    QByteArray  data(msg->getPointerToData());
    QJsonObject rootObj = QJsonDocument::fromJson(data).object();
    qint32      result  = rootObj.value("ack").toInt(ERROR_CODE_NOT_FOUND);
    qint32      index   = rootObj.value("index").toInt(0);

    if (result == ERROR_CODE_SUCCESS && index > 0) {
        bool          bAddedItem = false;
        NewsDataItem* pItem      = this->getNewsDataItem(index);
        if (pItem == NULL) {
            pItem = new NewsDataItem();
            pItem->setIndex(index);
            bAddedItem = true;

            QQmlEngine::setObjectOwnership(pItem, QQmlEngine::CppOwnership);
        }

        this->m_mutex.lock();

        pItem->setHeader(this->m_editHeader);
        pItem->setInfo(this->m_editInfo);
        pItem->setTimeStamp(QDateTime::currentMSecsSinceEpoch());
        pItem->setUser(g_ConUserSettings.getReadableName());

        this->m_mutex.unlock();

        this->m_editItem = pItem;

        if (bAddedItem)
            this->addNewNewsData(pItem, UpdateAll);

        QMutexLocker lock(&this->m_mutex);

        std::sort(this->m_lNews.begin(), this->m_lNews.end(), NewsDataItem::compareTimeStampFunctionDescending);
    }

    // messageID unhandeld

    return result;
}

NewsDataItem* cDataNewsDataManager::getCurrentEditedItem()
{
    return this->m_editItem;
}

qint32 cDataNewsDataManager::startRemoveNewsDataItem(const qint32 index)
{
    if (!this->m_initialized)
        return ERROR_CODE_NOT_INITIALIZED;

    QJsonObject rootObj;
    rootObj.insert("index", index);

    TcpDataConRequest* req = new TcpDataConRequest(OP_CODE_CMD_REQ::REQ_DEL_NEWS_DATA_ITEM);
    req->m_lData           = QJsonDocument(rootObj).toJson(QJsonDocument::Compact);

    g_ConManager.sendNewRequest(req);
    return ERROR_CODE_SUCCESS;
}

qint32 cDataNewsDataManager::handleRemoveNewsDataItemResponse(MessageProtocol* msg)
{
    if (!this->m_initialized)
        return ERROR_CODE_NOT_INITIALIZED;

    QMutexLocker lock(&this->m_mutex);

    this->m_stLastServerUpdateTimeStamp = 0;

    return msg->getIntData();
}
