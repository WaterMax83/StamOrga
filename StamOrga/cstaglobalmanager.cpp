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
#include "../Data/cdataappuserevents.h"
#include "../Data/cdatagameuserdata.h"
#include "Connection/cconmanager.h"
#include "Connection/cconnetworkaccess.h"
#include "Connection/cconusersettings.h"
#include "Data/cdataconsolemanager.h"
#include "Data/cdatagamesmanager.h"
#include "Data/cdatamediamanager.h"
#include "Data/cdatameetinginfo.h"
#include "Data/cdatanewsdatamanager.h"
#include "Data/cdatappinfomanager.h"
#include "Data/cdatastatisticmanager.h"
#include "Data/cdataticketmanager.h"
#include "Data/cdatausermanager.h"
#include "Data/cdatawebpagemanager.h"
#include "cstaglobalmanager.h"
#include "cstaglobalsettings.h"
#include "cstasettingsmanager.h"
#include "cstaversionmanager.h"
#ifndef STAMORGA_APP
#include "PC/cpccontrolmanager.h"
#else
#include "userinterface.h"
#endif

cDataMeetingInfo* g_DataMeetingInfo;
cDataMeetingInfo* g_DataTripInfo;


cStaGlobalManager::cStaGlobalManager(QObject* parent)
    : cGenDisposer(parent)
{
}

qint32 cStaGlobalManager::initialize()
{
    qint32 rCode;

    QGuiApplication::setOrganizationName("WaterMax");
    QGuiApplication::setApplicationName("StamOrga");

// Register our component type with QML.
#ifdef STAMORGA_APP
    qmlRegisterType<UserInterface>("com.watermax.demo", 1, 0, "UserInterface");
#endif

    g_StaSettingsManager = new cStaSettingsManager();
    rCode                = g_StaSettingsManager->initialize();

    if (rCode == ERROR_CODE_SUCCESS) {
        g_DatAppInfoManager = new cDatAppInfoManager();
        rCode               = g_DatAppInfoManager->initialize();
    }
    if (rCode == ERROR_CODE_SUCCESS) {
        g_StaGlobalSettings = new cStaGlobalSettings();
        rCode               = g_StaGlobalSettings->initialize();
    }
    if (rCode == ERROR_CODE_SUCCESS) {
        g_ConUserSettings = new cConUserSettings();
        rCode             = g_ConUserSettings->initialize();
    }
    if (rCode == ERROR_CODE_SUCCESS) {
        g_ConNetworkAccess = new cConNetworkAccess();
        rCode              = g_ConNetworkAccess->initialize();
    }
    if (rCode == ERROR_CODE_SUCCESS) {
        g_StaVersionManager = new cStaVersionManager();
        rCode               = g_StaVersionManager->initialize();
    }
    if (rCode == ERROR_CODE_SUCCESS) {
        g_DataUserManager = new cDataUserManager();
        rCode             = g_DataUserManager->initialize();
    }
    if (rCode == ERROR_CODE_SUCCESS) {
        g_DataGamesManager = new cDataGamesManager();
        rCode              = g_DataGamesManager->initialize();
    }
    if (rCode == ERROR_CODE_SUCCESS) {
        g_DataTicketManager = new cDataTicketManager();
        rCode               = g_DataTicketManager->initialize();
    }
    if (rCode == ERROR_CODE_SUCCESS) {
        g_DataNewsDataManager = new cDataNewsDataManager();
        rCode                 = g_DataNewsDataManager->initialize();
    }
    if (rCode == ERROR_CODE_SUCCESS) {
        g_DataWebPageManager = new cDataWebPageManager();
        rCode                = g_DataWebPageManager->initialize();
    }
    if (rCode == ERROR_CODE_SUCCESS) {
        g_DataStatisticManager = new cDataStatisticManager();
        rCode                  = g_DataStatisticManager->initialize();
    }
    if (rCode == ERROR_CODE_SUCCESS) {
        g_DataMeetingInfo = new cDataMeetingInfo();
        rCode             = g_DataMeetingInfo->initialize();
    }
    if (rCode == ERROR_CODE_SUCCESS) {
        g_DataTripInfo = new cDataMeetingInfo();
        rCode          = g_DataTripInfo->initialize();
    }
    if (rCode == ERROR_CODE_SUCCESS) {
        g_DataGameUserData = new cDataGameUserData();
        rCode              = g_DataGameUserData->initialize();
    }
    if (rCode == ERROR_CODE_SUCCESS) {
        g_DataAppUserEvents = new cDataAppUserEvents();
        rCode               = g_DataAppUserEvents->initialize();
    }
    if (rCode == ERROR_CODE_SUCCESS) {
        g_DataConsoleManager = new cDataConsoleManager();
        rCode                = g_DataConsoleManager->initialize();
    }
    if (rCode == ERROR_CODE_SUCCESS) {
        g_DataMediaManager = new cDataMediaManager();
        rCode              = g_DataMediaManager->initialize();
    }
    if (rCode == ERROR_CODE_SUCCESS) {
        g_ConManager = new cConManager();
        rCode        = g_ConManager->initialize();
    }
#ifndef STAMORGA_APP
    if (rCode == ERROR_CODE_SUCCESS) {
        g_PCControlManager = new cPCControlManager();
        rCode              = g_PCControlManager->initialize();
    }
#else
    if (rCode == ERROR_CODE_SUCCESS)
        this->m_mediaProvider = new cDataMediaProvider();
#endif

    if (rCode == ERROR_CODE_SUCCESS)
        this->m_initialized = true;

    return rCode;
}

#ifdef STAMORGA_APP
void cStaGlobalManager::setQmlInformationClasses(QQmlApplicationEngine* engine)
{
    engine->rootContext()->setContextProperty("gDataAppInfoManager", g_DatAppInfoManager);
    engine->rootContext()->setContextProperty("gStaGlobalSettings", g_StaGlobalSettings);
    engine->rootContext()->setContextProperty("gStaVersionManager", g_StaVersionManager);
    engine->rootContext()->setContextProperty("gDataUserManager", g_DataUserManager);
    engine->rootContext()->setContextProperty("gDataGamesManager", g_DataGamesManager);
    engine->rootContext()->setContextProperty("gDataTicketManager", g_DataTicketManager);
    engine->rootContext()->setContextProperty("gDataNewsDataManager", g_DataNewsDataManager);
    engine->rootContext()->setContextProperty("gDataWebPageManager", g_DataWebPageManager);
    engine->rootContext()->setContextProperty("gDataStatisticManager", g_DataStatisticManager);
    engine->rootContext()->setContextProperty("gDataMeetingInfo", g_DataMeetingInfo);
    engine->rootContext()->setContextProperty("gDataGameUserData", g_DataGameUserData);
    engine->rootContext()->setContextProperty("gDataAppUserEvents", g_DataAppUserEvents);
    engine->rootContext()->setContextProperty("gDataTripInfo", g_DataTripInfo);
    engine->rootContext()->setContextProperty("gDataConsoleManager", g_DataConsoleManager);
    engine->rootContext()->setContextProperty("gDataMediaManager", g_DataMediaManager);
    engine->rootContext()->setContextProperty("gConUserSettings", g_ConUserSettings);

#ifdef STAMORGA_APP
    engine->addImageProvider("media", this->m_mediaProvider);
#endif
}
#endif

void cStaGlobalManager::setQMLObjectOwnershipToCpp(QObject* pObject)
{
#ifdef STAMORGA_APP
    QQmlEngine::setObjectOwnership(pObject, QQmlEngine::CppOwnership);
#else
    Q_UNUSED(pObject);
#endif
}
