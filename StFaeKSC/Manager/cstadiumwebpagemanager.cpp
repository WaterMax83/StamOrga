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

#include "cstadiumwebpagemanager.h"
#include "../Common/Network/messagecommand.h"
#include "../General/pushnotification.h"

//extern GlobalData* g_GlobalData;

cStadiumWebPageManager g_StadiumWebPageManager;

cStadiumWebPageManager::cStadiumWebPageManager(QObject* parent)
    : cGenDisposer(parent)
{
}

qint32 cStadiumWebPageManager::initialize()
{
    this->m_webPageList = new StadiumWebPage();
    this->m_webPageList->initialize();

    this->m_initialized = true;

    return ERROR_CODE_SUCCESS;
}

/*
 * Request
 * {
 *      "type": "Media",
 *      "version": "V1.0"       // not used at the moment
 *      "cmd":  "add"  -> get groups of statistic
 *      "format": "JPEG" or "PNG"
 *      "data":     -> Data of this media
 *      "gameIndex": -> Game index of this media
 * }
 */
MessageProtocol* cStadiumWebPageManager::handleStadiumCommand(UserConData* pUserCon, MessageProtocol* request)
{
    qint32          rCode = ERROR_CODE_SUCCESS;
    QJsonParseError jerror;
    QByteArray      data    = QByteArray(request->getPointerToData());
    QJsonObject     rootObj = QJsonDocument::fromJson(data, &jerror).object();
    if (jerror.error != QJsonParseError::NoError) {
        qWarning().noquote() << QString("Could not answer stadium command, json parse errror: %1 - %2").arg(jerror.errorString()).arg(jerror.offset);
        rCode = ERROR_CODE_WRONG_PARAMETER;
    }

    QString cmd = rootObj.value("cmd").toString("");
    if (cmd.isEmpty()) {
        qWarning().noquote() << QString("No cmd found in JSON stadium command");
        rCode = ERROR_CODE_MISSING_PARAMETER;
    }

    QJsonObject rootObjAnswer;
    if (rCode == ERROR_CODE_SUCCESS) {
        if (cmd == "add") {
            rCode = this->m_webPageList->addNewWebPageItem(pUserCon->m_userID);
        } else if (cmd == "load") {
            QString text, link;
            qint32  index = rootObj.value("index").toInt(0);
            rCode         = this->m_webPageList->loadWebPageDataItem(index, text, link);
            if (rCode == ERROR_CODE_SUCCESS) {
                rootObjAnswer.insert("index", index);
                rootObjAnswer.insert("name", text);
                rootObjAnswer.insert("link", link);
            }
        } else if (cmd == "set") {
            qint32  index = rootObj.value("index").toInt(0);
            QString text  = rootObj.value("text").toString();
            QString link  = rootObj.value("link").toString();
            rCode         = this->m_webPageList->setWebPageDataItem(index, text, link);
            if (rCode == ERROR_CODE_SUCCESS)
                g_pushNotify->sendNewStadiumWebPageNotification(text, pUserCon->m_userID, index);
        } else if (cmd == "list") {
            rCode = this->handleStadiumGetListCommand(rootObj, rootObjAnswer);
        } else
            rCode = ERROR_CODE_NOT_IMPLEMENTED;
    }

    rootObjAnswer.insert("type", "Stadium");
    rootObjAnswer.insert("cmd", cmd);
    rootObjAnswer.insert("ack", rCode);

    if (cmd != "list")
        qInfo().noquote() << QString("Handle stadium command %3 from %1 with %2").arg(pUserCon->m_userName).arg(rCode).arg(cmd);

    QByteArray answer = QJsonDocument(rootObjAnswer).toJson(QJsonDocument::Compact);

    return new MessageProtocol(OP_CODE_CMD_RES::ACK_CMD_STADIUM, answer);
}

qint32 cStadiumWebPageManager::handleStadiumGetListCommand(QJsonObject& rootObj, QJsonObject& rootObjAnswer)
{
    qint32 updateIndex           = rootObj.value("index").toInt(UpdateIndex::UpdateAll);
    qint64 lastUpdateListFromApp = (qint64)rootObj.value("timestamp").toDouble(0);

    if (lastUpdateListFromApp == 0)
        updateIndex = UpdateIndex::UpdateAll;

    rootObjAnswer.insert("index", updateIndex);

    QJsonArray webPageArr;
    for (int j = 0; j < this->m_webPageList->getNumberOfInternalList(); j++) {
        WebPageItem* pItem = (WebPageItem*)this->m_webPageList->getRequestConfigItemFromListIndex(j);
        if (pItem == NULL)
            continue;

        if (updateIndex == UpdateIndex::UpdateDiff && pItem->m_lastUpdate <= lastUpdateListFromApp)
            continue; // Skip ticket because user already has all info

        QJsonObject webPageObj;
        webPageObj.insert("name", pItem->m_itemName);
        webPageObj.insert("index", pItem->m_index);
        webPageObj.insert("timestamp", pItem->m_timestamp);
        webPageObj.insert("userID", pItem->m_userID);
        webPageObj.insert("lastUpdate", pItem->m_lastUpdate);
//        webPageObj.insert("link", pItem->m_link);
        webPageArr.append(webPageObj);
    }
    rootObjAnswer.insert("timestamp", this->m_webPageList->getLastUpdateTime());
    rootObjAnswer.insert("webPages", webPageArr);

    return ERROR_CODE_SUCCESS;
}
