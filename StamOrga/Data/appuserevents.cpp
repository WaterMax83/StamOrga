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

    QString info = jsObj.value("info").toString();
    QString type = jsObj.value("type").toString();
    if (type == NOTIFY_TOPIC_NEW_APP_VERSION) {

        this->m_eventNewAppVersion = false;
        if (QString::compare(STAM_ORGA_VERSION_S, info) >= 0)
            return; /* already have newest version */
        else
            this->m_eventNewAppVersion = true;
    } else if (type == NOTIFY_TOPIC_NEW_FANCLUB_NEWS) {

        this->m_pGlobalData->setNewsDataItemHasEvent(info.toUInt());
        this->m_eventNewFanclubNews++;
    }
}

void AppUserEvents::resetCurrentEvents()
{
    this->m_eventNewAppVersion  = false;
    this->m_eventNewFanclubNews = 0;
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
