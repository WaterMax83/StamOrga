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

#include "cconsettings.h"
#include "../Common/General/globalfunctions.h"
#include "../cstasettingsmanager.h"

// clang-format off

#define USER_GROUP      "USER_LOGIN"

#define USER_IPADDR     "IPAddress"
#define USER_USERNAME   "UserName"
#define USER_PASSWORD   "Password"
#define USER_SALT       "Salt"
#define USER_READABLE   "ReadableName"

// clang-format on


cConSettings g_ConSettings;

cConSettings::cConSettings(QObject* parent)
    : cGenDisposer(parent)
{
}

qint32 cConSettings::initialize()
{
    QString value;

    g_StaSettingsManager.getValue(USER_GROUP, USER_IPADDR, value);
    this->m_ipAddr = value;
    g_StaSettingsManager.getValue(USER_GROUP, USER_USERNAME, value);
    this->m_userName = value;
    g_StaSettingsManager.getValue(USER_GROUP, USER_PASSWORD, value);
    this->m_passWord = value;
    g_StaSettingsManager.getValue(USER_GROUP, USER_SALT, value);
    this->m_salt = value;
    g_StaSettingsManager.getValue(USER_GROUP, USER_READABLE, value);
    this->m_readableName = value;


    this->m_initialized = true;

    return ERROR_CODE_SUCCESS;
}

QString cConSettings::getIPAddr()
{
    return this->m_ipAddr;
}


void cConSettings::setIPAddr(const QString ipaddr)
{
    if (this->m_ipAddr != ipaddr) {
        this->m_ipAddr = ipaddr;
        g_StaSettingsManager.setValue(USER_GROUP, USER_IPADDR, ipaddr);
    }
}
