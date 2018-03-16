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

#include "cstasettingsmanager.h"
#include "../Common/General/globalfunctions.h"


cStaSettingsManager g_StaSettingsManager;

cStaSettingsManager::cStaSettingsManager(QObject* parent)
    : cGenDisposer(parent)
{
}


qint32 cStaSettingsManager::initialize()
{
    this->m_pMainUserSettings = new QSettings();
    this->m_pMainUserSettings->setIniCodec(("UTF-8"));

    this->m_initialized = true;

    return ERROR_CODE_SUCCESS;
}


qint32 cStaSettingsManager::getValue(const QString group, const QString key, QString& value)
{
    if (!this->m_initialized)
        return ERROR_CODE_NOT_INITIALIZED;

    this->m_pMainUserSettings->beginGroup(group);

    value = this->m_pMainUserSettings->value(key, "").toString();

    this->m_pMainUserSettings->endGroup();

    return ERROR_CODE_SUCCESS;
}

qint32 cStaSettingsManager::setValue(const QString group, const QString key, const QString value)
{
    return this->setValue(group, key, QVariant(value));
}

qint32 cStaSettingsManager::getBoolValue(const QString group, const QString key, bool& value)
{
    if (!this->m_initialized)
        return ERROR_CODE_NOT_INITIALIZED;

    this->m_pMainUserSettings->beginGroup(group);

    value = this->m_pMainUserSettings->value(key, false).toBool();

    this->m_pMainUserSettings->endGroup();

    return ERROR_CODE_SUCCESS;
}

qint32 cStaSettingsManager::setBoolValue(const QString group, const QString key, const bool value)
{
    return this->setValue(group, key, QVariant(value));
}


qint32 cStaSettingsManager::setValue(const QString group, const QString key, const QVariant value)
{
    if (!this->m_initialized)
        return ERROR_CODE_NOT_INITIALIZED;

    this->m_pMainUserSettings->beginGroup(group);

    this->m_pMainUserSettings->setValue(key, value);

    this->m_pMainUserSettings->endGroup();

    this->m_pMainUserSettings->sync();

    return ERROR_CODE_SUCCESS;
}
