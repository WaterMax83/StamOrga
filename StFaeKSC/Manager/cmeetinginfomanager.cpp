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
#include "cmeetinginfomanager.h"

extern GlobalData* g_GlobalData;

cMeetingInfoManager g_MeetingInfoManager;

cMeetingInfoManager::cMeetingInfoManager(QObject* parent)
    : cGenDisposer(parent)
{
}


qint32 cMeetingInfoManager::initialize()
{
    this->m_initialized = true;

    return ERROR_CODE_SUCCESS;
}

MessageProtocol* cMeetingInfoManager::getMeetingInfo(UserConData* pUserCon, MessageProtocol* request)
{
    if (!this->m_initialized)
        return NULL;

    QByteArray  data    = QByteArray(request->getPointerToData());
    QJsonObject rootObj = QJsonDocument::fromJson(data).object();

    qint32 gameIndex = rootObj.value("index").toInt(0);
    qint32 type      = rootObj.value("type").toInt(-1);

    QMutexLocker lock(&g_GlobalData->m_globalDataMutex);

    quint32              ack;
    QList<MeetingInfo*>* pList;
    if (type == MEETING_TYPE_MEETING) {
        ack   = OP_CODE_CMD_RES::ACK_GET_MEETING_INFO;
        pList = &g_GlobalData->m_meetingInfos;
    } else if (type == MEETING_TYPE_AWAYTRIP) {
        ack   = OP_CODE_CMD_RES::ACK_GET_AWAYTRIP_INFO;
        pList = (QList<MeetingInfo*>*)&g_GlobalData->m_awayTripInfos;
    } else
        return NULL;

    QJsonObject rootAns;
    rootAns.insert("index", gameIndex);
    rootAns.insert("type", type);

    GamesPlay* pGame = (GamesPlay*)g_GlobalData->m_GamesList.getItem(gameIndex);
    if (pGame == NULL) {
        rootAns.insert("ack", ERROR_CODE_NOT_FOUND);
    } else {
        rootAns.insert("ack", ERROR_CODE_SUCCESS);
        QString      when;
        QString      where;
        QString      info;
        qint32       result;
        MeetingInfo* mInfo = NULL;
        for (int i = 0; i < pList->size(); i++) {
            MeetingInfo* mi = pList->at(i);
            if (mi->getGameIndex() == gameIndex) {
                result = mi->getMeetingInfo(when, where, info);
                mInfo  = mi;
                break;
            }
        }
        if (result != ERROR_CODE_SUCCESS)
            rootAns.insert("ack", result);
        else {
            rootAns.insert("when", when);
            rootAns.insert("where", where);
            rootAns.insert("info", info);

            QJsonArray acceptArr;
            qint32     acceptCount = mInfo->getNumberOfInternalList();
            for (int i = 0; i < acceptCount; i++) {
                AcceptMeetingInfo* ami = (AcceptMeetingInfo*)mInfo->getRequestConfigItemFromListIndex(i);
                QJsonObject        acceptObj;
                acceptObj.insert("index", ami->m_index);
                acceptObj.insert("state", ami->m_state);
                acceptObj.insert("user", ami->m_userID);
                acceptObj.insert("name", ami->m_itemName);
                acceptArr.append(acceptObj);
            }
            rootAns.insert("accept", acceptArr);
        }
    }

    QByteArray answer = QJsonDocument(rootAns).toJson(QJsonDocument::Compact);

    qInfo().noquote() << QString("User %1 got MeetingInfo of game %2")
                             .arg(pUserCon->m_userName)
                             .arg(g_GlobalData->m_GamesList.getItemName(gameIndex));
    return new MessageProtocol(ack, answer);
}

MessageProtocol* cMeetingInfoManager::getChangeMeetingInfo(UserConData* pUserCon, MessageProtocol* request)
{
    if (!this->m_initialized)
        return NULL;

    QByteArray  data    = QByteArray(request->getPointerToData());
    QJsonObject rootObj = QJsonDocument::fromJson(data).object();

    qint32  gameIndex = rootObj.value("index").toInt(0);
    qint32  type      = rootObj.value("type").toInt(-1);
    QString when      = rootObj.value("when").toString();
    QString where     = rootObj.value("where").toString();
    QString info      = rootObj.value("info").toString();

    quint32 ack;
    if (type == MEETING_TYPE_MEETING) {
        ack = OP_CODE_CMD_RES::ACK_CHANGE_MEETING_INFO;
    } else if (type == MEETING_TYPE_AWAYTRIP) {
        ack = OP_CODE_CMD_RES::ACK_CHANGE_AWAYTRIP_INFO;
    } else
        return NULL;

    qint64 messageID = -1;
    qint32 rCode     = g_GlobalData->requestChangeMeetingInfo(gameIndex, 0, when, where, info, pUserCon->m_userID, type, messageID);
    if (rCode == ERROR_CODE_SUCCESS)
        qInfo().noquote() << QString("User %1 set MeetingInfo of game %2")
                                 .arg(pUserCon->m_userName)
                                 .arg(g_GlobalData->m_GamesList.getItemName(gameIndex));

    QJsonObject rootAns;
    rootAns.insert("ack", rCode);
    rootAns.insert("index", gameIndex);
    rootAns.insert("type", type);
    rootAns.insert("messageID", messageID);

    QByteArray answer = QJsonDocument(rootAns).toJson(QJsonDocument::Compact);

    return new MessageProtocol(ack, answer);
}
