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
#include <QtGui/QGuiApplication>

#include "../Common/General/globalfunctions.h"
#include "Connection/cconmanager.h"
#include "Connection/cconusersettings.h"
#include "Data/cdatappinfomanager.h"
#include "cstaglobalmanager.h"
#include "cstaglobalsettings.h"
#include "cstasettingsmanager.h"


cStaGlobalManager::cStaGlobalManager(QObject* parent)
    : cGenDisposer(parent)
{
}

qint32 cStaGlobalManager::initialize()
{
    qint32 rCode;

    QGuiApplication::setOrganizationName("WaterMax");
    QGuiApplication::setApplicationName("StamOrga");

    rCode = g_StaSettingsManager.initialize();

    if (rCode == ERROR_CODE_SUCCESS)
        rCode = g_StaGlobalSettings.initialize();

    if (rCode == ERROR_CODE_SUCCESS)
        rCode = g_DatAppInfoManager.initialize();

    if (rCode == ERROR_CODE_SUCCESS)
        rCode = g_ConUserSettings.initialize();

    if (rCode == ERROR_CODE_SUCCESS)
        rCode = g_ConManager.initialize();

    if (rCode == ERROR_CODE_SUCCESS)
        this->m_initialized = true;

    return rCode;
}
