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

#include "appuserevents.h"
#include "../../Common/General/config.h"
#include "../../Common/General/globalfunctions.h"

AppUserEvents::AppUserEvents(QObject* parent)
    : QObject(parent)
{
    this->m_eventNewAppVersion  = false;
    this->m_eventNewFanclubNews = 0;
}


void AppUserEvents::initialize(GlobalData* pGlobalData)
{
    this->m_pGlobalData = pGlobalData;
}

void AppUserEvents::addNewUserEvents(QJsonObject& jsObj)
{
    EventInfo* pEvent = new EventInfo();
    pEvent->m_info    = jsObj.value("info").toString();
    pEvent->m_type    = jsObj.value("type").toString();
    pEvent->m_eventID = static_cast<qint64>(jsObj.value("id").toDouble());

    if (pEvent->m_type == NOTIFY_TOPIC_NEW_APP_VERSION) {

        this->m_eventNewAppVersion = false;
        if (QString::compare(STAM_ORGA_VERSION_S, pEvent->m_info) >= 0)
            return; /* already have newest version */
        else
            this->m_eventNewAppVersion = true;
    } else if (pEvent->m_type == NOTIFY_TOPIC_NEW_FANCLUB_NEWS) {

        this->m_pGlobalData->setNewsDataItemHasEvent(pEvent->m_info.toUInt());
        this->m_eventNewFanclubNews++;
    } else if (pEvent->m_type == NOTIFY_TOPIC_NEW_FREE_TICKET) {

        this->m_pGlobalData->setGamePlayItemHasEvent(pEvent->m_info.toUInt());
    } else if (pEvent->m_type == NOTIFY_TOPIC_NEW_MEETING || pEvent->m_type == NOTIFY_TOPIC_CHANGE_MEETING) {

        this->m_pGlobalData->setGamePlayItemHasEvent(pEvent->m_info.toUInt());
    } else if (pEvent->m_type == NOTIFY_TOPIC_NEW_AWAY_ACCEPT) {

        this->m_pGlobalData->setGamePlayItemHasEvent(pEvent->m_info.toUInt());
    } else
        return;

    this->m_lEvents.append(pEvent);
}

void AppUserEvents::resetCurrentEvents()
{
    this->m_eventNewAppVersion  = false;
    this->m_eventNewFanclubNews = 0;
    this->m_pGlobalData->resetAllGamePlayEvents();
    this->m_pGlobalData->resetAllNewsDataEvents();
    for (int i = this->m_lEvents.count() - 1; i >= 0; i--) {
        delete this->m_lEvents[i];
        this->m_lEvents.removeAt(i);
    }
}


qint32 AppUserEvents::getCurrentMainEventCounter()
{
    qint32 rValue = 0;

    if (this->m_eventNewAppVersion)
        rValue++;

    if (this->m_eventNewFanclubNews)
        rValue += this->m_eventNewFanclubNews;

    return rValue;
}

qint32 AppUserEvents::getCurrentUpdateEventCounter()
{
    if (this->m_eventNewAppVersion)
        return 1;
    return 0;
}

qint32 AppUserEvents::getCurrentFanclubEventCounter()
{
    return this->m_eventNewFanclubNews;
}

qint32 AppUserEvents::clearUserEventFanclub(UserInterface* pInt, qint32 newsIndex)
{
    for (int i = this->m_lEvents.count() - 1; i >= 0; i--) {
        if (this->m_lEvents[i]->m_type == NOTIFY_TOPIC_NEW_FANCLUB_NEWS) {

            if (this->m_lEvents[i]->m_info.toInt() == newsIndex) {
                this->m_eventNewFanclubNews--;
                pInt->startSetUserEvents(this->m_lEvents[i]->m_eventID, 0);
                delete this->m_lEvents[i];
                this->m_lEvents.removeAt(i);
            }
        }
    }
    return ERROR_CODE_SUCCESS;
}

qint32 AppUserEvents::clearUserEventGamPlay(UserInterface* pInt, qint32 gameIndex)
{
    for (int i = this->m_lEvents.count() - 1; i >= 0; i--) {
        if (this->m_lEvents[i]->m_type == NOTIFY_TOPIC_NEW_FREE_TICKET
            || this->m_lEvents[i]->m_type == NOTIFY_TOPIC_NEW_AWAY_ACCEPT
            || this->m_lEvents[i]->m_type == NOTIFY_TOPIC_CHANGE_MEETING
            || this->m_lEvents[i]->m_type == NOTIFY_TOPIC_NEW_MEETING) {

            if (this->m_lEvents[i]->m_info.toInt() == gameIndex) {
                pInt->startSetUserEvents(this->m_lEvents[i]->m_eventID, 0);
                delete this->m_lEvents[i];
                this->m_lEvents.removeAt(i);
            }
        }
    }
    return ERROR_CODE_SUCCESS;
}

qint32 AppUserEvents::clearUserEventUpdate(UserInterface* pInt)
{
    for (int i = this->m_lEvents.count() - 1; i >= 0; i--) {
        if (this->m_lEvents[i]->m_type == NOTIFY_TOPIC_NEW_APP_VERSION) {
            this->m_eventNewAppVersion = false;
            pInt->startSetUserEvents(this->m_lEvents[i]->m_eventID, 0);
            delete this->m_lEvents[i];
            this->m_lEvents.removeAt(i);
        }
    }
    return ERROR_CODE_SUCCESS;
}
