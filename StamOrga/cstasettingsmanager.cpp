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


#define GROUP_ARRAY_ITEM "item"

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

/************************ QString values ******************************/
qint32 cStaSettingsManager::getValue(const QString group, const QString key, QString& value)
{
    QVariant vValue;
    qint32   rValue = this->getValue(group, key, vValue, QVariant(""));

    if (rValue == ERROR_CODE_SUCCESS)
        value = vValue.toString();

    return rValue;
}

qint32 cStaSettingsManager::setValue(const QString group, const QString key, const QString value)
{
    return this->setValue(group, key, QVariant(value));
}

qint32 cStaSettingsManager::getValue(const QString group, const QString key, const qint32 index, QString& value)
{
    QVariant vValue;
    qint32   rValue = this->getValue(group, key, index, vValue, QVariant(""));

    if (rValue == ERROR_CODE_SUCCESS)
        value = vValue.toString();

    return rValue;
}

qint32 cStaSettingsManager::setValue(const QString group, const QString key, const qint32 index, const QString value)
{
    return this->setValue(group, key, index, QVariant(value));
}

/************************ Boolean values ******************************/
qint32 cStaSettingsManager::getBoolValue(const QString group, const QString key, bool& value)
{
    QVariant vValue;
    qint32   rValue = this->getValue(group, key, vValue, QVariant(false));

    if (rValue == ERROR_CODE_SUCCESS)
        value = vValue.toBool();

    return rValue;
}

qint32 cStaSettingsManager::setBoolValue(const QString group, const QString key, const bool value)
{
    return this->setValue(group, key, QVariant(value));
}

/************************ qint64 values ******************************/
qint32 cStaSettingsManager::getInt64Value(const QString group, const QString key, qint64& value)
{
    QVariant vValue;
    qint32   rValue = this->getValue(group, key, vValue, QVariant(0));

    if (rValue == ERROR_CODE_SUCCESS)
        value = vValue.toLongLong();

    return rValue;
}

qint32 cStaSettingsManager::setInt64Value(const QString group, const QString key, const qint64 value)
{
    return this->setValue(group, key, QVariant(value));
}

qint32 cStaSettingsManager::getInt64Value(const QString group, const QString key, const qint32 index, qint64& value)
{
    QVariant vValue;
    qint32   rValue = this->getValue(group, key, index, vValue, QVariant(0));

    if (rValue == ERROR_CODE_SUCCESS)
        value = vValue.toLongLong();

    return rValue;
}

qint32 cStaSettingsManager::setInt64Value(const QString group, const QString key, const qint32 index, const qint64 value)
{
    return this->setValue(group, key, index, QVariant(value));
}

qint32 cStaSettingsManager::removeGroup(const QString group)
{
    qint32 rValue = ERROR_CODE_NOT_FOUND;
    this->m_pMainUserSettings->beginGroup(group);

    if (this->m_pMainUserSettings->childGroups().length() > 0 || this->m_pMainUserSettings->childKeys().length() > 0) {
        this->m_pMainUserSettings->remove("");
        rValue = ERROR_CODE_SUCCESS;
    }

    this->m_pMainUserSettings->endGroup();

    return rValue;
}


/************************ private ******************************/
qint32 cStaSettingsManager::getValue(const QString group, const QString key, QVariant& value, const QVariant defaultValue)
{
    if (!this->m_initialized)
        return ERROR_CODE_NOT_INITIALIZED;

    this->m_pMainUserSettings->beginGroup(group);

    value = this->m_pMainUserSettings->value(key, defaultValue);

    this->m_pMainUserSettings->endGroup();

    return ERROR_CODE_SUCCESS;
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

qint32 cStaSettingsManager::getValue(const QString group, const QString key, const qint32 index, QVariant& value, const QVariant defaultValue)
{
    if (!this->m_initialized)
        return ERROR_CODE_NOT_INITIALIZED;

    this->m_pMainUserSettings->beginGroup(group);
    qint32 count = this->m_pMainUserSettings->beginReadArray(GROUP_ARRAY_ITEM);
    if (index >= count) {
        this->m_pMainUserSettings->endArray();
        this->m_pMainUserSettings->endGroup();

        return ERROR_CODE_NOT_FOUND;
    }

    this->m_pMainUserSettings->setArrayIndex(index);

    value = this->m_pMainUserSettings->value(key, defaultValue);

    this->m_pMainUserSettings->endArray();
    this->m_pMainUserSettings->endGroup();

    return ERROR_CODE_SUCCESS;
}

qint32 cStaSettingsManager::setValue(const QString group, const QString key, const qint32 index, const QVariant value)
{
    if (!this->m_initialized)
        return ERROR_CODE_NOT_INITIALIZED;

    this->m_pMainUserSettings->beginGroup(group);
    this->m_pMainUserSettings->beginWriteArray(GROUP_ARRAY_ITEM);

    this->m_pMainUserSettings->setArrayIndex(index);
    this->m_pMainUserSettings->setValue(key, value);

    this->m_pMainUserSettings->endArray();
    this->m_pMainUserSettings->endGroup();

    this->m_pMainUserSettings->sync();

    return ERROR_CODE_SUCCESS;
}
