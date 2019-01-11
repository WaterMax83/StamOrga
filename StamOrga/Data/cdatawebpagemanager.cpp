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

#include "cdatawebpagemanager.h"

#include "../Common/General/globalfunctions.h"
#include "../Common/General/globaltiming.h"
#include "../Common/Network/messagecommand.h"
#include "../Connection/cconmanager.h"
#include "../Connection/cconusersettings.h"
#include "cstaglobalmanager.h"

// clang-format off
#define WEBPAGE_CMD_LIST    0
#define WEBPAGE_CMD_LOAD    1
#define WEBPAGE_CMD_SET     2
#define WEBPAGE_CMD_ADD     3
// clang-format on


extern cStaGlobalManager* g_GlobalManager;


cDataWebPageManager* g_DataWebPageManager;

cDataWebPageManager::cDataWebPageManager(QObject* parent)
    : cGenDisposer(parent)
{
}

qint32 cDataWebPageManager::initialize()
{
    this->m_stLastLocalUpdateTimeStamp  = 0;
    this->m_stLastServerUpdateTimeStamp = 0;

    this->m_initialized = true;

    return ERROR_CODE_SUCCESS;
}


void cDataWebPageManager::addNewWebPageData(TextDataItem* sWebPage, const quint16 updateIndex)
{
    if (!this->m_initialized)
        return;

    TextDataItem* pWebPage = this->getWebDataItem(sWebPage->index());
    if (pWebPage == NULL) {
        QMutexLocker lock(&this->m_mutex);
        this->m_lWebPages.append(sWebPage);
        return;
    } else if (updateIndex == UpdateIndex::UpdateDiff) {
        if (pWebPage->user() != sWebPage->user()) {
            pWebPage->setUser(sWebPage->user());
        }
        if (pWebPage->header() != sWebPage->header()) {
            pWebPage->setHeader(sWebPage->header());
        }
        if (pWebPage->info() != sWebPage->info()) {
            pWebPage->setInfo(sWebPage->info());
        }
        delete sWebPage;
    }
}

bool cDataWebPageManager::setWebPageItemHasEvent(qint32 index)
{
    if (!this->m_initialized)
        return false;

    QMutexLocker lock(&this->m_mutex);

    /* WebPage data is not yet called */
    if (this->m_stLastLocalUpdateTimeStamp == 0)
        return true;

    for (int i = 0; i < this->m_lWebPages.count(); i++) {
        if (this->m_lWebPages[i]->index() == index) {
            this->m_lWebPages[i]->setEvent(this->m_lWebPages[i]->event() + 1);
            return true;
        }
    }
    return false;
}

void cDataWebPageManager::resetAllWebPageEvents()
{
    if (!this->m_initialized)
        return;

    QMutexLocker lock(&this->m_mutex);

    for (int i = 0; i < this->m_lWebPages.count(); i++) {
        this->m_lWebPages[i]->setEvent(0);
    }
}


TextDataItem* cDataWebPageManager::getWebDataItem(qint32 index)
{
    if (!this->m_initialized)
        return NULL;

    QMutexLocker lock(&this->m_mutex);

    for (int i = 0; i < this->m_lWebPages.size(); i++) {
        if (this->m_lWebPages[i]->index() == index)
            return this->m_lWebPages[i];
    }
    return NULL;
}

qint32 cDataWebPageManager::getWebListLength()
{
    QMutexLocker lock(&this->m_mutex);
    return this->m_lWebPages.size();
}

TextDataItem* cDataWebPageManager::getWebDataFromArrayIndex(int index)
{
    QMutexLocker lock(&this->m_mutex);

    if (index < this->m_lWebPages.size()) {
        return this->m_lWebPages.at(index);
    }
    return NULL;
}

QString cDataWebPageManager::getWebPageLastLocalUpdateString()
{
    if (!this->m_initialized)
        return "";

    QMutexLocker lock(&this->m_mutex);
    QDateTime    time = QDateTime::fromMSecsSinceEpoch(this->m_stLastLocalUpdateTimeStamp);
    //    return QDateTime::fromMSecsSinceEpoch(this->m_stLastLocalUpdateTimeStamp).toString("dd.MM.yy hh:mm:ss");
    return getTimeStampSinceString(time);
}

qint32 cDataWebPageManager::startListWebPageData()
{
    if (!this->m_initialized)
        return ERROR_CODE_NOT_INITIALIZED;

    QMutexLocker lock(&this->m_mutex);

    QJsonObject rootObj;
    if (this->m_stLastLocalUpdateTimeStamp + TIMEOUT_UPDATE_TICKETS > QDateTime::currentMSecsSinceEpoch() && this->m_lWebPages.count() > 0)
        rootObj.insert("index", UpdateIndex::UpdateDiff);
    else
        rootObj.insert("index", UpdateIndex::UpdateAll);

    rootObj.insert("timestamp", this->m_stLastServerUpdateTimeStamp);
    rootObj.insert("cmd", "list");

    TcpDataConRequest* req = new TcpDataConRequest(OP_CODE_CMD_REQ::REQ_CMD_STADIUM);
    req->m_lData           = QJsonDocument(rootObj).toJson(QJsonDocument::Compact);
    req->m_subCmd          = WEBPAGE_CMD_LIST;

    g_ConManager->sendNewRequest(req);
    return ERROR_CODE_SUCCESS;
}

qint32 cDataWebPageManager::startAddWebPage()
{
    if (!this->m_initialized)
        return ERROR_CODE_NOT_INITIALIZED;

    QMutexLocker lock(&this->m_mutex);

    QJsonObject rootObj;
    rootObj.insert("cmd", "add");

    TcpDataConRequest* req = new TcpDataConRequest(OP_CODE_CMD_REQ::REQ_CMD_STADIUM);
    req->m_lData           = QJsonDocument(rootObj).toJson(QJsonDocument::Compact);
    req->m_subCmd          = WEBPAGE_CMD_ADD;

    g_ConManager->sendNewRequest(req);
    return ERROR_CODE_SUCCESS;
}

qint32 cDataWebPageManager::startLoadWebPage(const qint32 index, const qint32 width)
{
    if (!this->m_initialized)
        return ERROR_CODE_NOT_INITIALIZED;

    QMutexLocker lock(&this->m_mutex);

    QJsonObject rootObj;
    rootObj.insert("cmd", "load");
    rootObj.insert("index", index);

    TcpDataConRequest* req = new TcpDataConRequest(OP_CODE_CMD_REQ::REQ_CMD_STADIUM);
    req->m_lData           = QJsonDocument(rootObj).toJson(QJsonDocument::Compact);
    req->m_subCmd          = WEBPAGE_CMD_LOAD;

    this->m_screenWidth = width - 25; // Scrollbar is normaly visible

    g_ConManager->sendNewRequest(req);
    return ERROR_CODE_SUCCESS;
}

qint32 cDataWebPageManager::startSetWebPage(const QString text, const QString link)
{
    if (!this->m_initialized)
        return ERROR_CODE_NOT_INITIALIZED;

    QMutexLocker lock(&this->m_mutex);

    QJsonObject rootObj;
    rootObj.insert("cmd", "set");
    rootObj.insert("index", this->m_editItem->index());
    rootObj.insert("text", text);
    rootObj.insert("link", link);
    //    if (body.isEmpty()) {
    //        rootObj.insert("body", "");
    //    } else {
    //        QByteArray uBody = qCompress(body.toUtf8(), 9);
    //        rootObj.insert("body", QString(uBody.toHex()));
    //    }

    TcpDataConRequest* req = new TcpDataConRequest(OP_CODE_CMD_REQ::REQ_CMD_STADIUM);
    req->m_lData           = QJsonDocument(rootObj).toJson(QJsonDocument::Compact);
    req->m_subCmd          = WEBPAGE_CMD_SET;

    g_ConManager->sendNewRequest(req);
    return ERROR_CODE_SUCCESS;
}

qint32 cDataWebPageManager::handleWebPageResponse(MessageProtocol* msg)
{
    if (!this->m_initialized)
        return ERROR_CODE_NOT_INITIALIZED;

    QByteArray  data(msg->getPointerToData());
    QJsonObject rootObj = QJsonDocument::fromJson(data).object();

    QString cmd    = rootObj.value("cmd").toString();
    qint32  result = rootObj.value("ack").toInt(ERROR_CODE_NOT_FOUND);

    this->m_mutex.lock();

    if (cmd == "list" && result == ERROR_CODE_SUCCESS) {

        qint32     updateIndex = rootObj.value("index").toInt(UpdateAll);
        qint64     timestamp   = (qint64)rootObj.value("timestamp").toDouble(0);
        QJsonArray arrWebPage  = rootObj.value("webPages").toArray();

        if (updateIndex == UpdateIndex::UpdateAll) {
            for (int i = 0; i < this->m_lWebPages.size(); i++)
                delete this->m_lWebPages[i];
            this->m_lWebPages.clear();
        }
        this->m_stLastLocalUpdateTimeStamp = QDateTime::currentMSecsSinceEpoch();

        this->m_mutex.unlock();

        for (int i = 0; i < arrWebPage.count(); i++) {
            QJsonObject   webObj   = arrWebPage.at(i).toObject();
            TextDataItem* pWebPage = new TextDataItem();

            pWebPage->setIndex(webObj.value("index").toInt());
            pWebPage->setTimeStamp((qint64)webObj.value("lastUpdate").toDouble());
            pWebPage->setHeader(webObj.value("name").toString());
            //            pWebPage->setInfo(webObj.value("link").toString());

            g_GlobalManager->setQMLObjectOwnershipToCpp(pWebPage);
            this->addNewWebPageData(pWebPage, updateIndex);
        }

        std::sort(this->m_lWebPages.begin(), this->m_lWebPages.end(), TextDataItem::compareTimeStampFunctionAscending);

        this->m_stLastServerUpdateTimeStamp = timestamp;
    } else {

        this->m_mutex.unlock();
        if (cmd == "load") {
            qint32        index = rootObj.value("index").toInt();
            TextDataItem* pItem = this->getWebDataItem(index);
            if (pItem == NULL)
                return ERROR_CODE_NOT_FOUND;

            pItem->setHeader(rootObj.value("name").toString());
            QString link = rootObj.value("link").toString();
            if (!link.startsWith("http") && this->m_screenWidth != 0) {
                //                qInfo() << "Scrren width = " << this->m_screenWidth;
                link = link.replace("=\"width:360px", QString("=\"width:%1px").arg(this->m_screenWidth));
                //            QString body = rootObj.value("body").toString();
                //            if (body.isEmpty()) {
                //                pItem->setInfo("");
                //            } else {
                //                QByteArray uBody        = QByteArray::fromHex(body.toUtf8());
                //                QString    internalBody = QString(qUncompress(uBody));
                //#if defined(Q_OS_ANDROID) || defined(Q_OS_IOS)

                //#else

                //#endif
                //                pItem->setInfo(internalBody);
                //            }
            }
            pItem->setInfo(link);

            this->m_editItem = pItem;
        }
    }

    return result;
}

//qint32 cDataWebPageManager::startGetNewsDataItem(qint32 index)
//{
//    if (!this->m_initialized)
//        return ERROR_CODE_NOT_INITIALIZED;

//    QJsonObject rootObj;
//    rootObj.insert("index", index);

//    TcpDataConRequest* req = new TcpDataConRequest(OP_CODE_CMD_REQ::REQ_GET_NEWS_DATA_ITEM);
//    req->m_lData           = QJsonDocument(rootObj).toJson(QJsonDocument::Compact);

//    g_ConManager->sendNewRequest(req);
//    return ERROR_CODE_SUCCESS;
//}

//qint32 cDataWebPageManager::handleGetNewsDataItem(MessageProtocol* msg)
//{
//    if (!this->m_initialized)
//        return ERROR_CODE_NOT_INITIALIZED;

//    QByteArray  data(msg->getPointerToData());
//    QJsonObject rootObj = QJsonDocument::fromJson(data).object();

//    qint32 result = rootObj.value("ack").toInt(ERROR_CODE_NOT_FOUND);
//    if (result != ERROR_CODE_SUCCESS)
//        return result;
//    qint32 newsIndex = rootObj.value("index").toInt(0);

//    WebPageItem* pItem = this->getNewsDataItem(newsIndex);
//    if (pItem == NULL)
//        return ERROR_CODE_NOT_FOUND;

//    QMutexLocker lock(&this->m_mutex);

//    pItem->setTimeStamp((qint64)rootObj.value("timestamp").toDouble());
//    pItem->setUser(rootObj.value("user").toString());
//    pItem->setHeader(rootObj.value("header").toString());

//    QByteArray cryptInfo = QByteArray::fromHex(rootObj.value("info").toString().toUtf8());
//    if (cryptInfo.size() > 0)
//        pItem->setInfo(QString(qUncompress(cryptInfo)));
//    else
//        pItem->setInfo("");

//    std::sort(this->m_lNews.begin(), this->m_lNews.end(), WebPageItem::compareTimeStampFunctionDescending);

//    return result;
//}


//qint32 cDataWebPageManager::startChangeNewsDataItem(const qint32 index, const QString header, const QString info)
//{
//    if (!this->m_initialized)
//        return ERROR_CODE_NOT_INITIALIZED;

//    QJsonObject rootObj;
//    rootObj.insert("index", index);
//    rootObj.insert("header", header);
//    QByteArray dataInfo = qCompress(info.toUtf8(), 9);
//    rootObj.insert("info", QString(dataInfo.toHex()));

//    this->m_editHeader = header;
//    this->m_editInfo   = info;

//    qInfo().noquote() << QString("Compressed news info from %1 to %2 Bytes").arg(info.size()).arg(dataInfo.toHex().size());

//    TcpDataConRequest* req = new TcpDataConRequest(OP_CODE_CMD_REQ::REQ_CHANGE_NEWS_DATA);
//    req->m_lData           = QJsonDocument(rootObj).toJson(QJsonDocument::Compact);

//    g_ConManager->sendNewRequest(req);
//    return ERROR_CODE_SUCCESS;
//}

//qint32 cDataWebPageManager::handleChangeNewsDataResponse(MessageProtocol* msg)
//{
//    if (!this->m_initialized)
//        return ERROR_CODE_NOT_INITIALIZED;

//    QByteArray  data(msg->getPointerToData());
//    QJsonObject rootObj = QJsonDocument::fromJson(data).object();
//    qint32      result  = rootObj.value("ack").toInt(ERROR_CODE_NOT_FOUND);
//    qint32      index   = rootObj.value("index").toInt(0);

//    if (result == ERROR_CODE_SUCCESS && index > 0) {
//        bool          bAddedItem = false;
//        WebPageItem* pItem      = this->getNewsDataItem(index);
//        if (pItem == NULL) {
//            pItem = new WebPageItem();
//            pItem->setIndex(index);
//            bAddedItem = true;

//            g_GlobalManager->setQMLObjectOwnershipToCpp(pItem);
//        }

//        this->m_mutex.lock();

//        pItem->setHeader(this->m_editHeader);
//        pItem->setInfo(this->m_editInfo);
//        pItem->setTimeStamp(QDateTime::currentMSecsSinceEpoch());
//        pItem->setUser(g_ConUserSettings->getReadableName());

//        this->m_mutex.unlock();

//        this->m_editItem = pItem;

//        if (bAddedItem)
//            this->addNewNewsData(pItem, UpdateAll);

//        QMutexLocker lock(&this->m_mutex);

//        std::sort(this->m_lNews.begin(), this->m_lNews.end(), WebPageItem::compareTimeStampFunctionDescending);
//    }

//    // messageID unhandeld

//    return result;
//}

TextDataItem* cDataWebPageManager::getCurrentEditedItem()
{
    return this->m_editItem;
}

//qint32 cDataWebPageManager::startRemoveNewsDataItem(const qint32 index)
//{
//    if (!this->m_initialized)
//        return ERROR_CODE_NOT_INITIALIZED;

//    QJsonObject rootObj;
//    rootObj.insert("index", index);

//    TcpDataConRequest* req = new TcpDataConRequest(OP_CODE_CMD_REQ::REQ_DEL_NEWS_DATA_ITEM);
//    req->m_lData           = QJsonDocument(rootObj).toJson(QJsonDocument::Compact);

//    g_ConManager->sendNewRequest(req);
//    return ERROR_CODE_SUCCESS;
//}

//qint32 cDataWebPageManager::handleRemoveNewsDataItemResponse(MessageProtocol* msg)
//{
//    if (!this->m_initialized)
//        return ERROR_CODE_NOT_INITIALIZED;

//    QMutexLocker lock(&this->m_mutex);

//    this->m_stLastServerUpdateTimeStamp = 0;

//    return msg->getIntData();
//}
