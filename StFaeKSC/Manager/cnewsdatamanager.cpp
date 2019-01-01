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

#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>

#include "../Common/General/globalfunctions.h"
#include "../Common/Network/messagecommand.h"
#include "../General/globaldata.h"
#include "../General/pushnotification.h"
#include "cnewsdatamanager.h"

extern GlobalData* g_GlobalData;

cNewsDataManager g_NewsDataManager;

cNewsDataManager::cNewsDataManager(QObject* parent)
    : cGenDisposer(parent)
{
}


qint32 cNewsDataManager::initialize()
{
    this->m_initialized = true;

    return ERROR_CODE_SUCCESS;
}

MessageProtocol* cNewsDataManager::getNewsDataList(UserConData* pUserCon, MessageProtocol* request)
{
    if (!this->m_initialized)
        return NULL;

    QByteArray  data    = QByteArray(request->getPointerToData());
    QJsonObject rootObj = QJsonDocument::fromJson(data).object();

    qint32 updateIndex              = rootObj.value("index").toInt(UpdateIndex::UpdateAll);
    qint64 lastUpdateTicketsFromApp = (qint64)rootObj.value("timestamp").toDouble(0);

    if (lastUpdateTicketsFromApp == 0)
        updateIndex = UpdateIndex::UpdateAll;

    QJsonObject rootAns;
    rootAns.insert("ack", ERROR_CODE_SUCCESS);
    rootAns.insert("index", updateIndex);

    quint16 numbOfNews = g_GlobalData->m_fanclubNews.getNumberOfInternalList();

    QJsonArray arrNews;
    for (quint32 i = 0; i < numbOfNews; i++) {
        NewsData* pNews = (NewsData*)(g_GlobalData->m_fanclubNews.getRequestConfigItemFromListIndex(i));
        if (pNews == NULL)
            continue;
        QJsonObject newsObj;

        if (updateIndex == UpdateIndex::UpdateDiff && pNews->m_timestamp <= lastUpdateTicketsFromApp)
            continue; // Skip ticket because user already has all info

        newsObj.insert("header", pNews->m_itemName);
        newsObj.insert("timestamp", pNews->m_timestamp);
        newsObj.insert("index", pNews->m_index);
        newsObj.insert("user", g_ListedUser->getReadableName(pNews->m_userID));

        arrNews.append(newsObj);
    }

    rootAns.insert("timestamp", g_GlobalData->m_fanclubNews.getLastUpdateTime());
    rootAns.insert("news", arrNews);

    QByteArray answer = QJsonDocument(rootAns).toJson(QJsonDocument::Compact);

    Q_UNUSED(pUserCon);
    //    qInfo().noquote() << QString("User %1 request News Data List with %2 entries").arg(pUserCon->m_userName).arg(numbOfNews);

    return new MessageProtocol(OP_CODE_CMD_RES::ACK_GET_NEWS_DATA_LIST, answer);
}

MessageProtocol* cNewsDataManager::getNewsDataItem(UserConData* pUserCon, MessageProtocol* request)
{
    if (!this->m_initialized)
        return NULL;

    QByteArray  data    = QByteArray(request->getPointerToData());
    QJsonObject rootObj = QJsonDocument::fromJson(data).object();

    qint32 newsIndex = rootObj.value("index").toInt(0);

    QJsonObject rootAns;
    rootAns.insert("ack", ERROR_CODE_NOT_FOUND);
    rootAns.insert("index", newsIndex);

    if (newsIndex != 0) {
        NewsData* pItem = (NewsData*)g_GlobalData->m_fanclubNews.getItem(newsIndex);
        if (pItem != NULL) {

            rootAns.insert("ack", ERROR_CODE_SUCCESS);
            rootAns.insert("user", g_ListedUser->getReadableName(pItem->m_userID));
            rootAns.insert("header", pItem->m_itemName);
            QString info = QString(pItem->m_newsText.toHex());
            rootAns.insert("info", info);
            rootAns.insert("timestamp", pItem->m_timestamp);

            Q_UNUSED(pUserCon);
            //            qInfo().noquote() << QString("User %1 got fanclub news item %2").arg(pUserCon->m_userName, pItem->m_itemName);
        }
    }

    QByteArray answer = QJsonDocument(rootAns).toJson(QJsonDocument::Compact);

    return new MessageProtocol(OP_CODE_CMD_RES::ACK_GET_NEWS_DATA_ITEM, answer);
}
MessageProtocol* cNewsDataManager::getNewsDataChangeRequest(UserConData* pUserCon, MessageProtocol* request)
{
    if (!this->m_initialized)
        return NULL;

    QByteArray  data    = QByteArray(request->getPointerToData());
    QJsonObject rootObj = QJsonDocument::fromJson(data).object();

    qint32     newsIndex = rootObj.value("index").toInt(-1);
    QString    header    = rootObj.value("header").toString();
    QByteArray info      = QByteArray::fromHex(rootObj.value("info").toString().toUtf8());

    qint64      messageID = -1;
    qint32      rCode;
    QJsonObject rootAns;
    rootAns.insert("ack", ERROR_CODE_NOT_FOUND);
    rootAns.insert("index", newsIndex);
    if (newsIndex == 0) {
        rCode = g_GlobalData->m_fanclubNews.addNewFanclubNews(header, info, pUserCon->m_userID);
        if (rCode > ERROR_CODE_NO_ERROR) {
            qInfo().noquote() << QString("User %1 added news %2").arg(pUserCon->m_userName, header);
            QString bigText = QString("Es gibt eine neue Nachricht von %1: %2").arg(g_GlobalData->m_UserList.getReadableName(pUserCon->m_userID), header);
            messageID = g_pushNotify->sendNewFanclubNewsNotification(header, bigText, pUserCon->m_userID, rCode);
            rootAns.insert("ack", ERROR_CODE_SUCCESS);
            rootAns.insert("index", rCode);
        }
    } else {
        rCode = g_GlobalData->m_fanclubNews.changeFanclubNews(newsIndex, header, info, pUserCon->m_userID);
        if (rCode == ERROR_CODE_SUCCESS) {
            qInfo().noquote() << QString("User %1 changed news %2").arg(pUserCon->m_userName, header);
            QString bigText = QString("Eine Nachricht wurde geändert von %1: %2").arg(g_GlobalData->m_UserList.getReadableName(pUserCon->m_userID), header);
            messageID = g_pushNotify->sendNewFanclubNewsNotification(header, bigText, pUserCon->m_userID, newsIndex);
            rootAns.insert("ack", ERROR_CODE_SUCCESS);
        }
    }
    rootAns.insert("messageID", messageID);

    QByteArray answer = QJsonDocument(rootAns).toJson(QJsonDocument::Compact);

    return new MessageProtocol(OP_CODE_CMD_RES::ACK_CHANGE_NEWS_DATA, answer);
}

MessageProtocol* cNewsDataManager::getNewsDataRemoveRequest(UserConData* pUserCon, MessageProtocol* request)
{
    if (!this->m_initialized)
        return NULL;

    QByteArray  data    = QByteArray(request->getPointerToData());
    QJsonObject rootObj = QJsonDocument::fromJson(data).object();

    qint32 index = rootObj.value("index").toInt(0);
    qint32 rCode = ERROR_CODE_NOT_FOUND;
    if (index != 0) {
        if ((rCode = g_GlobalData->m_fanclubNews.removeItem(index)) == ERROR_CODE_SUCCESS) {
            qInfo().noquote() << QString("User %1 removed News Item %2")
                                     .arg(pUserCon->m_userName)
                                     .arg(index);
        }
    }

    return new MessageProtocol(OP_CODE_CMD_RES::ACK_DEL_NEWS_DATA_ITEM, rCode);
}
