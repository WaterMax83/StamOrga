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

#include "configlist.h"

#include "../Common/General/globalfunctions.h"

ConfigList::ConfigList()
{
}


qint32 ConfigList::removeItem(const QString name)
{
    quint32 index = this->getItemIndex(name);

    QMutexLocker locker(&this->m_mInternalInfoMutex);

    for (int i = 0; i < this->m_lInteralList.size(); i++) {
        if (this->m_lInteralList[i]->m_index == index) {
            this->m_lInteralList.removeAt(i);
            this->saveCurrentInteralList();

            CONSOLE_INFO(QString("removed Item \"%1\"").arg(name));
            return ERROR_CODE_SUCCESS;
        }
    }

    CONSOLE_WARNING(QString("Could not find item \"%1\"").arg(name))
    return ERROR_CODE_COMMON;
}

qint32 ConfigList::removeItem(const quint32 index)
{
    QMutexLocker locker(&this->m_mInternalInfoMutex);

    for (int i = 0; i < this->m_lInteralList.size(); i++) {
        if (this->m_lInteralList[i]->m_index == index) {
            QString name = this->m_lInteralList[i]->m_itemName;
            this->m_lInteralList.removeAt(i);
            this->saveCurrentInteralList();

            CONSOLE_INFO(QString("removed Item \"%1\"").arg(name));
            return ERROR_CODE_SUCCESS;
        }
    }

    CONSOLE_WARNING(QString("Could not find item \"%1\"").arg(index))
    return ERROR_CODE_COMMON;
}

bool ConfigList::itemExists(QString name)
{
    QMutexLocker locker(&this->m_mInternalInfoMutex);


    for (int i = 0; i < this->getNumberOfInternalList(); i++) {
        if (this->m_lInteralList[i]->m_itemName == name)
            return true;
    }
    return false;
}

bool ConfigList::itemExists(quint32 index)
{
    QMutexLocker locker(&this->m_mInternalInfoMutex);

    for (int i = 0; i < this->getNumberOfInternalList(); i++) {
        if (this->m_lInteralList[i]->m_index == index)
            return true;
    }
    return false;
}

ConfigItem* ConfigList::getItem(quint32 index)
{
    QMutexLocker locker(&this->m_mInternalInfoMutex);

    for (int i = 0; i < this->getNumberOfInternalList(); i++) {
        if (this->m_lInteralList[i]->m_index == index)
            return this->m_lInteralList[i];
    }
    return NULL;
}

quint32 ConfigList::getItemIndex(const QString name)
{
    QMutexLocker locker(&this->m_mInternalInfoMutex);

    for (int i = 0; i < this->getNumberOfInternalList(); i++) {
        if (this->m_lInteralList[i]->m_itemName == name)
            return this->m_lInteralList[i]->m_index;
    }
    return 0;
}

QString ConfigList::getItemName(quint32 index)
{
    QMutexLocker locker(&this->m_mInternalInfoMutex);

    for (int i = 0; i < this->getNumberOfInternalList(); i++) {
        if (this->m_lInteralList[i]->m_index == index)
            return this->m_lInteralList[i]->m_itemName;
    }
    return "";
}

ConfigItem* ConfigList::getItemFromArrayIndex(int index)
{
    if (index >= this->getNumberOfInternalList())
        return NULL;

    return this->m_lInteralList[index];
}

ConfigItem* ConfigList::getProblemItemFromArrayIndex(int index)
{
    if (index >= this->m_lAddItemProblems.size())
        return NULL;

    return this->m_lAddItemProblems[index];
}

bool ConfigList::updateItemValue(ConfigItem* pItem, QString key, QVariant value)
{
    bool         rValue = false;
    QMutexLocker locker(&this->m_mConfigIniMutex);

    this->m_pConfigSettings->beginGroup(GROUP_LIST_ITEM);
    int arrayCount = this->m_pConfigSettings->beginReadArray(CONFIG_LIST_ARRAY);
    for (int i = 0; i < arrayCount; i++) {
        this->m_pConfigSettings->setArrayIndex(i);
        //        QString actName  = this->m_pConfigSettings->value(ITEM_NAME, "").toString();
        quint32 actIndex = this->m_pConfigSettings->value(ITEM_INDEX, 0).toInt();
        if (pItem->m_index == actIndex) {

            this->m_pConfigSettings->setValue(key, value);
            //            qInfo().noquote() << QString("Change %1 of item %2 to %3").arg(key).arg(pItem->m_itemName).arg(value.toString());
            rValue = true;
            break;
        }
    }
    this->m_pConfigSettings->endArray();
    this->m_pConfigSettings->endGroup();
    return rValue;
}


quint32 ConfigList::getNextInternalIndex()
{
    quint32 savedIndex, usedIndex = 0;

    foreach (ConfigItem* item, this->m_lInteralList) {
        if (item->m_index > usedIndex)
            usedIndex = item->m_index;
    }

    QMutexLocker locker(&this->m_mConfigIniMutex);

    this->m_pConfigSettings->beginGroup(ITEM_INDEX_GROUP);
    savedIndex = this->m_pConfigSettings->value(ITEM_MAX_INDEX, 0).toUInt();

    if (usedIndex > savedIndex)
        savedIndex = usedIndex;

    savedIndex++;

    this->m_pConfigSettings->setValue(ITEM_MAX_INDEX, savedIndex);
    this->m_pConfigSettings->endGroup();

    return savedIndex;
}

ConfigList::~ConfigList()
{
    for (int i = 0; i < this->m_lInteralList.size(); i++)
        delete this->m_lInteralList[i];

    for (int i = 0; i < this->m_lAddItemProblems.size(); i++)
        delete this->m_lAddItemProblems[i];
}
