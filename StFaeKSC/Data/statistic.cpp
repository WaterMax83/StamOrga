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

#include <QDebug>

#include "../General/globaldata.h"
#include "statistic.h"

extern GlobalData* g_GlobalData;

Statistic::Statistic(QObject* parent)
    : BackgroundWorker(parent)
{
}


int Statistic::initialize()
{
    this->m_bckGrndCtrl = new BackgroundController();
    this->m_bckGrndCtrl->Start(this, false);

    return 1;
}

int Statistic::DoBackgroundWork()
{
    this->m_cycleTimer = new QTimer();
    this->m_cycleTimer->setSingleShot(true);
    connect(this->m_cycleTimer, &QTimer::timeout, this, &Statistic::slotCycleTimerFired);
    this->m_cycleTimer->start(10000);

    return ERROR_CODE_SUCCESS;
}

void Statistic::slotCycleTimerFired()
{
    QMutexLocker lock(&g_GlobalData->m_globalDataMutex);

    foreach (AvailableGameTickets* pAvTick, g_GlobalData->m_availableTickets) {
        //        pAvTick->
    }
}
