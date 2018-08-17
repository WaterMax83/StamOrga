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

#include "../../Common/General/config.h"
#include "../../Common/General/globalfunctions.h"
#include "../Common/Network/messagecommand.h"
#include "../Connection/cconmanager.h"
#include "../Connection/cconusersettings.h"
#include "../Data/cdatagamesmanager.h"
#include "../Data/cdatanewsdatamanager.h"
#include "cdataappuserevents.h"

cDataAppUserEvents* g_DataAppUserEvents;

cDataAppUserEvents::cDataAppUserEvents(QObject* parent)
    : cGenDisposer(parent)
{
    this->m_eventNewAppVersion  = false;
    this->m_eventNewFanclubNews = 0;
}


qint32 cDataAppUserEvents::initialize()
{
    this->m_initialized = true;

    return ERROR_CODE_SUCCESS;
}

qint32 cDataAppUserEvents::addNewUserEvents(QJsonArray& jsArr)
{
    if (!this->m_initialized)
        return ERROR_CODE_NOT_INITIALIZED;

    this->resetCurrentEvents();

    for (int i = 0; i < jsArr.size(); i++) {
        QJsonObject jsObj  = jsArr.at(i).toObject();
        EventInfo*  pEvent = new EventInfo();
        pEvent->m_info     = jsObj.value("info").toString();
        pEvent->m_type     = jsObj.value("type").toString();
        pEvent->m_eventID  = static_cast<qint64>(jsObj.value("id").toDouble());

        if (pEvent->m_type == NOTIFY_TOPIC_NEW_APP_VERSION) {

            this->m_eventNewAppVersion = false;
            if (QString::compare(STAM_ORGA_VERSION_S, pEvent->m_info) >= 0) {
                this->startSetUserEvents(pEvent->m_eventID, 0);
                continue; /* already have newest version */
            } else
                this->m_eventNewAppVersion = true;
        } else if (pEvent->m_type == NOTIFY_TOPIC_NEW_FANCLUB_NEWS) {

            if (g_ConUserSettings->userIsFanclubEnabled()) {
                g_DataNewsDataManager->setNewsDataItemHasEvent(pEvent->m_info.toInt());
                this->m_eventNewFanclubNews++;
            } else {
                this->m_eventNewFanclubNews = 0;
                this->startSetUserEvents(pEvent->m_eventID, 0);
            }
        } else if (pEvent->m_type == NOTIFY_TOPIC_NEW_FREE_TICKET) {
            if (!g_DataGamesManager->setGamePlayItemHasEvent(pEvent->m_info.toInt()))
                this->startSetUserEvents(pEvent->m_eventID, 0);
        } else if (pEvent->m_type == NOTIFY_TOPIC_NEW_MEETING || pEvent->m_type == NOTIFY_TOPIC_CHANGE_MEETING) {
            if (!g_DataGamesManager->setGamePlayItemHasEvent(pEvent->m_info.toInt()))
                this->startSetUserEvents(pEvent->m_eventID, 0);
        } else if (pEvent->m_type == NOTIFY_TOPIC_NEW_AWAY_ACCEPT) {
            if (!g_DataGamesManager->setGamePlayItemHasEvent(pEvent->m_info.toInt()))
                this->startSetUserEvents(pEvent->m_eventID, 0);
        } else if (pEvent->m_type == NOTIFY_TOPIC_NEW_COMMENT) {
            if (!g_DataGamesManager->setGamePlayItemHasEvent(pEvent->m_info.toInt()))
                this->startSetUserEvents(pEvent->m_eventID, 0);
        } else
            continue;

        this->m_lEvents.append(pEvent);
    }

    return ERROR_CODE_SUCCESS;
}

void cDataAppUserEvents::resetCurrentEvents()
{
    this->m_eventNewAppVersion  = false;
    this->m_eventNewFanclubNews = 0;
    g_DataGamesManager->resetAllGamePlayEvents();
    g_DataNewsDataManager->resetAllNewsDataEvents();
    for (int i = this->m_lEvents.count() - 1; i >= 0; i--) {
        delete this->m_lEvents[i];
        this->m_lEvents.removeAt(i);
    }
}


qint32 cDataAppUserEvents::getCurrentMainEventCounter()
{
    qint32 rValue = 0;

    if (this->m_eventNewAppVersion)
        rValue++;

    if (this->m_eventNewFanclubNews)
        rValue += this->m_eventNewFanclubNews;

    return rValue;
}

qint32 cDataAppUserEvents::getCurrentUpdateEventCounter()
{
    if (this->m_eventNewAppVersion)
        return 1;
    return 0;
}

qint32 cDataAppUserEvents::getCurrentFanclubEventCounter()
{
    return this->m_eventNewFanclubNews;
}

qint32 cDataAppUserEvents::clearUserEventFanclub(qint32 newsIndex)
{
    for (int i = this->m_lEvents.count() - 1; i >= 0; i--) {
        if (this->m_lEvents[i]->m_type == NOTIFY_TOPIC_NEW_FANCLUB_NEWS) {
            if (this->m_lEvents[i]->m_info.toInt() == newsIndex) {
                this->m_eventNewFanclubNews--;
                this->startSetUserEvents(this->m_lEvents[i]->m_eventID, 0);
                delete this->m_lEvents[i];
                this->m_lEvents.removeAt(i);
            }
        }
    }
    return ERROR_CODE_SUCCESS;
}

qint32 cDataAppUserEvents::clearUserEventGamePlay(qint32 gameIndex)
{
    for (int i = this->m_lEvents.count() - 1; i >= 0; i--) {
        if (this->m_lEvents[i]->m_type == NOTIFY_TOPIC_NEW_FREE_TICKET
            || this->m_lEvents[i]->m_type == NOTIFY_TOPIC_NEW_AWAY_ACCEPT
            || this->m_lEvents[i]->m_type == NOTIFY_TOPIC_CHANGE_MEETING
            || this->m_lEvents[i]->m_type == NOTIFY_TOPIC_NEW_MEETING
            || this->m_lEvents[i]->m_type == NOTIFY_TOPIC_NEW_COMMENT) {

            if (this->m_lEvents[i]->m_info.toInt() == gameIndex) {
                this->startSetUserEvents(this->m_lEvents[i]->m_eventID, 0);
                delete this->m_lEvents[i];
                this->m_lEvents.removeAt(i);
            }
        }
    }
    return ERROR_CODE_SUCCESS;
}

qint32 cDataAppUserEvents::clearUserEventUpdate()
{
    for (int i = this->m_lEvents.count() - 1; i >= 0; i--) {
        if (this->m_lEvents[i]->m_type == NOTIFY_TOPIC_NEW_APP_VERSION) {
            this->m_eventNewAppVersion = false;
            this->startSetUserEvents(this->m_lEvents[i]->m_eventID, 0);
            delete this->m_lEvents[i];
            this->m_lEvents.removeAt(i);
        }
    }
    return ERROR_CODE_SUCCESS;
}

qint32 cDataAppUserEvents::startSetUserEvents(const qint64 eventID, const qint32 status)
{
    if (!this->m_initialized)
        return ERROR_CODE_NOT_INITIALIZED;

    QJsonObject rootObj;
    rootObj.insert("eventID", eventID);
    rootObj.insert("status", status);

    TcpDataConRequest* req = new TcpDataConRequest(OP_CODE_CMD_REQ::REQ_SET_USER_EVENTS);
    req->m_lData           = QJsonDocument(rootObj).toJson(QJsonDocument::Compact);

    g_ConManager->sendNewRequest(req);
    return ERROR_CODE_SUCCESS;
}

qint32 cDataAppUserEvents::handleSetUserEventsResponse(MessageProtocol* msg)
{
    if (!this->m_initialized)
        return ERROR_CODE_NOT_INITIALIZED;

    QByteArray  data(msg->getPointerToData());
    QJsonObject rootObj = QJsonDocument::fromJson(data).object();

    qint32 result = rootObj.value("ack").toInt(ERROR_CODE_NOT_FOUND);

    return result;
}
