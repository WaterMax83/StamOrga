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

#include "checkconsistentdata.h"

CheckConsistentData::CheckConsistentData(QObject* parent)
    : BackgroundWorker(parent)
{
}


qint32 CheckConsistentData::initialize(GlobalData* globalData)
{
    this->m_pGlobalData = globalData;

    return ERROR_CODE_SUCCESS;
}

int CheckConsistentData::DoBackgroundWork()
{
    this->m_timer = new QTimer();
    this->m_timer->setSingleShot(false);
    this->m_timer->setInterval(60 * 60 * 1000); // 1h

    connect(this->m_timer, &QTimer::timeout, this, &CheckConsistentData::slotTimerFired);

    return ERROR_CODE_SUCCESS;
}


void CheckConsistentData::slotTimerFired()
{
    qInfo().noquote() << "Starting consitency check";

    foreach (AvailableGameTickets* pGameTicket, this->m_pGlobalData->m_availableTickets) {
        pGameTicket->checkConsistency();
    }
}
