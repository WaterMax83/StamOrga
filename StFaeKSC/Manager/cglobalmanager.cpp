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

#include "cglobalmanager.h"
#include "../Common/General/globalfunctions.h"

#include "ccontrolmanager.h"
#include "cgamesmanager.h"
#include "cmediamanager.h"
#include "cmeetinginfomanager.h"
#include "cnewsdatamanager.h"
#include "csmtpmanager.h"
#include "cstadiumwebpagemanager.h"
#include "cstatisticmanager.h"
#include "cticketmanager.h"
#include "cusermanager.h"

cGlobalManager::cGlobalManager(QObject* parent)
    : cGenDisposer(parent)
{
}

qint32 cGlobalManager::initialize()
{
    qint32 rValue;

    rValue = g_UserManager.initialize();

    if (rValue == ERROR_CODE_SUCCESS)
        rValue = g_GamesManager.initialize();

    if (rValue == ERROR_CODE_SUCCESS)
        rValue = g_TicketManager.initialize();

    if (rValue == ERROR_CODE_SUCCESS)
        rValue = g_NewsDataManager.initialize();

    if (rValue == ERROR_CODE_SUCCESS)
        rValue = g_StatisticManager.initialize();

    if (rValue == ERROR_CODE_SUCCESS)
        rValue = g_MeetingInfoManager.initialize();

    if (rValue == ERROR_CODE_SUCCESS)
        rValue = g_MediaManager.initialize();

    if (rValue == ERROR_CODE_SUCCESS)
        rValue = g_StadiumWebPageManager.initialize();

    if (rValue == ERROR_CODE_SUCCESS) {
        rValue = g_SmtpManager.initialize();
        this->m_ctrlSmtp.Start(&g_SmtpManager, false);
    }

    if (rValue == ERROR_CODE_SUCCESS)
        rValue = g_ControlManager.initialize();

    this->m_initialized = true;

    return rValue;
}
