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

#include "../Common/General/globalfunctions.h"
#include "../Common/Network/messagecommand.h"
#include "../Connection/cconmanager.h"
#include "cdatagamesmanager.h"
#include "cdatameetinginfo.h"
#include "cstaglobalmanager.h"

extern cStaGlobalManager* g_GlobalManager;

cDataMeetingInfo::cDataMeetingInfo(QObject* parent)
    : cGenDisposer(parent)
{
    this->resetMeetingInfo();
}

qint32 cDataMeetingInfo::initialize()
{
    qRegisterMetaType<AcceptMeetingInfo*>("AcceptMeetingInfo*");
    qRegisterMetaType<cDataCommentItem*>("cDataCommentItem*");

    this->m_initialized = true;

    return ERROR_CODE_SUCCESS;
}

void cDataMeetingInfo::resetMeetingInfo()
{
    this->m_info           = "";
    this->m_when           = "";
    this->m_where          = "";
    this->m_bDataIsReseted = true;
}


qint32 cDataMeetingInfo::startLoadMeetingInfo(const qint32 gameIndex, const qint32 type)
{
    if (!this->m_initialized)
        return ERROR_CODE_NOT_INITIALIZED;

    QMutexLocker lock(&this->m_mutex);

    QJsonObject rootObj;
    rootObj.insert("index", gameIndex);
    rootObj.insert("type", type);

    TcpDataConRequest* req;
    if (type == MEETING_TYPE_MEETING)
        req = new TcpDataConRequest(OP_CODE_CMD_REQ::REQ_GET_MEETING_INFO);
    else
        req = new TcpDataConRequest(OP_CODE_CMD_REQ::REQ_GET_AWAYTRIP_INFO);
    req->m_lData = QJsonDocument(rootObj).toJson(QJsonDocument::Compact);

    g_ConManager->sendNewRequest(req);
    return ERROR_CODE_SUCCESS;
}

qint32 cDataMeetingInfo::handleLoadMeetingInfoResponse(MessageProtocol* msg)
{
    if (!this->m_initialized)
        return ERROR_CODE_NOT_INITIALIZED;

    QByteArray  data(msg->getPointerToData());
    QJsonObject rootObj = QJsonDocument::fromJson(data).object();
    qint32      result  = rootObj.value("ack").toInt(ERROR_CODE_NOT_FOUND);

    this->clearAcceptInfoList();
    this->clearCommentList();

    QMutexLocker lock(&this->m_mutex);

    this->m_bDataIsReseted = false;

    if (result != ERROR_CODE_SUCCESS) {
        this->m_where = "";
        this->m_when  = "";
        this->m_info  = "";
    }
    this->m_where    = rootObj.value("where").toString();
    this->m_when     = rootObj.value("when").toString();
    this->m_info     = rootObj.value("info").toString();
    qint32 gameIndex = rootObj.value("index").toInt();
    qint32 type      = rootObj.value("type").toInt();

    bool bInfoSet = (m_when.isEmpty() && m_where.isEmpty() && m_info.isEmpty()) ? false : true;

    quint32    acceptMeeting = 0, interestMeeting = 0, declineMeeting = 0;
    QJsonArray acceptArr = rootObj.value("accept").toArray();
    for (int i = 0; i < acceptArr.size(); i++) {
        QJsonObject        acceptObj = acceptArr.at(i).toObject();
        AcceptMeetingInfo* pAmi      = new AcceptMeetingInfo();

        pAmi->setIndex(acceptObj.value("index").toInt());
        pAmi->setValue(acceptObj.value("state").toInt());
        pAmi->setUserIndex(acceptObj.value("user").toInt());
        pAmi->setName(acceptObj.value("name").toString());

        if (pAmi->value() == ACCEPT_STATE_ACCEPT)
            acceptMeeting++;
        else if (pAmi->value() == ACCEPT_STATE_MAYBE)
            interestMeeting++;
        else if (pAmi->value() == ACCEPT_STATE_DECLINE)
            declineMeeting++;

        g_GlobalManager->setQMLObjectOwnershipToCpp(pAmi);
        this->m_acceptInfo.append(pAmi);
    }

    GamePlay* pGame = g_DataGamesManager->getGamePlay(gameIndex);
    if (pGame != NULL) {
        if (type == MEETING_TYPE_MEETING) {
            pGame->setAcceptedMeetingCount(acceptMeeting);
            pGame->setInterestedMeetingCount(interestMeeting);
            pGame->setDeclinedMeetingCount(declineMeeting);
            if (bInfoSet || acceptMeeting > 0 || interestMeeting > 0)
                pGame->setMeetingInfo(1);
            else
                pGame->setMeetingInfo(0);
        } else if (type == MEETING_TYPE_AWAYTRIP) {
            pGame->setAcceptedTripCount(acceptMeeting);
            pGame->setInterestedTripCount(interestMeeting);
            pGame->setDeclinedTripCount(declineMeeting);
            if (bInfoSet || acceptMeeting > 0 || interestMeeting > 0)
                pGame->setTripInfo(1);
            else
                pGame->setMeetingInfo(0);
        }
    }

    QJsonArray commentArr = rootObj.value("comment").toArray();
    for (int i = 0; i < commentArr.size(); i++) {
        QJsonObject       commentObj = commentArr.at(i).toObject();
        cDataCommentItem* pItem      = new cDataCommentItem();

        pItem->setUser(commentObj.value("name").toString());
        pItem->setTimeStamp(commentObj.value("timestamp").toDouble());
        pItem->setComment(commentObj.value("comment").toString());

        g_GlobalManager->setQMLObjectOwnershipToCpp(pItem);
        this->m_comments.append(pItem);
    }

    return result;
}

qint32 cDataMeetingInfo::startSaveMeetingInfo(const qint32 gameIndex, const QString when,
                                              const QString where, const QString info,
                                              const qint32 type)
{
    if (!this->m_initialized)
        return ERROR_CODE_NOT_INITIALIZED;

    QMutexLocker lock(&this->m_mutex);

    if (!this->m_bDataIsReseted && this->m_when == when && this->m_where == where && this->m_info == info)
        return ERROR_CODE_NO_ERROR;

    QJsonObject rootObj;
    rootObj.insert("index", gameIndex);
    rootObj.insert("type", type);
    rootObj.insert("when", when);
    rootObj.insert("where", where);
    rootObj.insert("info", info);

    TcpDataConRequest* req;
    if (type == MEETING_TYPE_MEETING)
        req = new TcpDataConRequest(OP_CODE_CMD_REQ::REQ_CHANGE_MEETING_INFO);
    else
        req = new TcpDataConRequest(OP_CODE_CMD_REQ::REQ_CHANGE_AWAYTRIP_INFO);
    req->m_lData = QJsonDocument(rootObj).toJson(QJsonDocument::Compact);

    g_ConManager->sendNewRequest(req);
    return ERROR_CODE_SUCCESS;
}

qint32 cDataMeetingInfo::handleSaveMeetingInfoResponse(MessageProtocol* msg)
{
    if (!this->m_initialized)
        return ERROR_CODE_NOT_INITIALIZED;

    QByteArray  data(msg->getPointerToData());
    QJsonObject rootObj = QJsonDocument::fromJson(data).object();
    qint32      result  = rootObj.value("ack").toInt(ERROR_CODE_NOT_FOUND);

    /* rest is not handled */

    return result;
}

qint32 cDataMeetingInfo::startAcceptMeetingInfo(const qint32 gameIndex, const qint32 accept,
                                                const QString name, const qint32 type,
                                                const qint32 acceptIndex)
{
    if (!this->m_initialized)
        return ERROR_CODE_NOT_INITIALIZED;

    QMutexLocker lock(&this->m_mutex);

    QJsonObject rootObj;
    rootObj.insert("index", gameIndex);
    rootObj.insert("type", type);
    rootObj.insert("accept", accept);
    rootObj.insert("acceptIndex", acceptIndex);
    rootObj.insert("name", name);

    TcpDataConRequest* req;
    if (type == MEETING_TYPE_MEETING)
        req = new TcpDataConRequest(OP_CODE_CMD_REQ::REQ_ACCEPT_MEETING);
    else
        req = new TcpDataConRequest(OP_CODE_CMD_REQ::REQ_ACCEPT_AWAYTRIP);
    req->m_lData = QJsonDocument(rootObj).toJson(QJsonDocument::Compact);

    g_ConManager->sendNewRequest(req);
    return ERROR_CODE_SUCCESS;
}

qint32 cDataMeetingInfo::handAcceptMeetingInfo(MessageProtocol* msg)
{
    if (!this->m_initialized)
        return ERROR_CODE_NOT_INITIALIZED;

    QByteArray  data(msg->getPointerToData());
    QJsonObject rootObj = QJsonDocument::fromJson(data).object();
    qint32      result  = rootObj.value("ack").toInt(ERROR_CODE_NOT_FOUND);

    /* rest is not handled */

    return result;
}

qint32 cDataMeetingInfo::startSendNewComment(const qint32 gameIndex, const QString comment,
                                             const qint32 type)
{
    if (!this->m_initialized)
        return ERROR_CODE_NOT_INITIALIZED;

    QMutexLocker lock(&this->m_mutex);

    QJsonObject rootObj;
    rootObj.insert("index", gameIndex);
    rootObj.insert("type", type);
    rootObj.insert("comment", comment.trimmed());

    TcpDataConRequest* req;
    if (type == MEETING_TYPE_MEETING)
        req = new TcpDataConRequest(OP_CODE_CMD_REQ::REQ_SEND_COMMENT_MEET);
    else
        req = new TcpDataConRequest(OP_CODE_CMD_REQ::REQ_SEND_COMMENT_TRIP);
    req->m_lData = QJsonDocument(rootObj).toJson(QJsonDocument::Compact);

    g_ConManager->sendNewRequest(req);
    return ERROR_CODE_SUCCESS;
}

qint32 cDataMeetingInfo::handleSendCommentResponse(MessageProtocol* msg)
{
    if (!this->m_initialized)
        return ERROR_CODE_NOT_INITIALIZED;

    QByteArray  data(msg->getPointerToData());
    QJsonObject rootObj = QJsonDocument::fromJson(data).object();
    qint32      result  = rootObj.value("ack").toInt(ERROR_CODE_NOT_FOUND);

    /* rest is not handled */

    return result;
}


AcceptMeetingInfo* cDataMeetingInfo::getAcceptInfoFromIndex(qint32 index)
{
    QMutexLocker lock(&this->m_mutex);

    if (index >= this->m_acceptInfo.size())
        return NULL;

    return this->m_acceptInfo[index];
}

qint32 cDataMeetingInfo::addNewAcceptInfo(AcceptMeetingInfo* info)
{
    QMutexLocker lock(&this->m_mutex);

    for (int i = 0; i < this->m_acceptInfo.size(); i++) {
        if (this->m_acceptInfo[i]->index() == info->index())
            return -1;
    }

    this->m_acceptInfo.append(info);
    return 1;
}

void cDataMeetingInfo::clearAcceptInfoList()
{
    QMutexLocker lock(&this->m_mutex);

    for (int i = 0; i < this->m_acceptInfo.size(); i++)
        delete this->m_acceptInfo[i];
    this->m_acceptInfo.clear();
}

qint32 cDataMeetingInfo::getAcceptedListCount()
{
    QMutexLocker lock(&this->m_mutex);

    return this->m_acceptInfo.size();
}

cDataCommentItem* cDataMeetingInfo::getCommentFromIndex(qint32 index)
{
    QMutexLocker lock(&this->m_mutex);

    if (index >= this->m_comments.size())
        return NULL;

    return this->m_comments[index];
}
void cDataMeetingInfo::clearCommentList()
{
    QMutexLocker lock(&this->m_mutex);

    for (int i = 0; i < this->m_comments.size(); i++)
        delete this->m_comments[i];
    this->m_comments.clear();
}

qint32 cDataMeetingInfo::getCommentCount()
{
    QMutexLocker lock(&this->m_mutex);

    return this->m_comments.size();
}
