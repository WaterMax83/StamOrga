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

#include <QDateTime>

#include "configlist.h"

#include "../Common/General/globalfunctions.h"

ConfigList::ConfigList()
{
}


qint32 ConfigList::removeItem(const QString name)
{
    qint32 index = this->getItemIndex(name);
    if (index <= 0)
        return ERROR_CODE_COMMON;

    QMutexLocker locker(&this->m_mInternalInfoMutex);

    for (int i = 0; i < this->m_lInteralList.size(); i++) {
        if (this->m_lInteralList[i]->m_index == index) {
            this->m_lInteralList.removeAt(i);
            this->saveCurrentInteralList();

            qInfo().noquote() << QString("removed Item \"%1\"").arg(name);
            return ERROR_CODE_SUCCESS;
        }
    }

    CONSOLE_WARNING(QString("Could not find item \"%1\"").arg(name))
    return ERROR_CODE_COMMON;
}

qint32 ConfigList::removeItem(const qint32 index)
{
    QMutexLocker locker(&this->m_mInternalInfoMutex);

    for (int i = 0; i < this->m_lInteralList.size(); i++) {
        if (this->m_lInteralList[i]->m_index == index) {
            QString name = this->m_lInteralList[i]->m_itemName;
            this->m_lInteralList.removeAt(i);
            this->saveCurrentInteralList();

            qInfo().noquote() << QString("removed Item \"%1\"").arg(name);
            return ERROR_CODE_SUCCESS;
        }
    }

    CONSOLE_WARNING(QString("Could not find item \"%1\"").arg(index))
    return ERROR_CODE_NOT_FOUND;
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

bool ConfigList::itemExists(qint32 index)
{
    QMutexLocker locker(&this->m_mInternalInfoMutex);

    for (int i = 0; i < this->getNumberOfInternalList(); i++) {
        if (this->m_lInteralList[i]->m_index == index)
            return true;
    }
    return false;
}

ConfigItem* ConfigList::getItem(qint32 index)
{
    QMutexLocker locker(&this->m_mInternalInfoMutex);

    for (int i = 0; i < this->getNumberOfInternalList(); i++) {
        if (this->m_lInteralList[i]->m_index == index)
            return this->m_lInteralList[i];
    }
    return NULL;
}

qint32 ConfigList::getItemIndex(const QString name)
{
    QMutexLocker locker(&this->m_mInternalInfoMutex);

    for (int i = 0; i < this->getNumberOfInternalList(); i++) {
        if (this->m_lInteralList[i]->m_itemName == name)
            return (qint32) this->m_lInteralList[i]->m_index;
    }
    return -1;
}

QString ConfigList::getItemName(const qint32 index)
{
    QMutexLocker locker(&this->m_mInternalInfoMutex);

    for (int i = 0; i < this->getNumberOfInternalList(); i++) {
        if (this->m_lInteralList[i]->m_index == index)
            return this->m_lInteralList[i]->m_itemName;
    }
    return "";
}

qint64 ConfigList::getTimeStamp(const qint32 index)
{
    QMutexLocker locker(&this->m_mInternalInfoMutex);

    for (int i = 0; i < this->getNumberOfInternalList(); i++) {
        if (this->m_lInteralList[i]->m_index == index)
            return this->m_lInteralList[i]->m_timestamp;
    }
    return -1;
}

QString ConfigList::getFileName()
{
    QMutexLocker locker(&this->m_mConfigIniMutex);

    return this->m_pConfigSettings->fileName();
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

bool ConfigList::updateItemValue(ConfigItem* pItem, QString key, QVariant value, qint64 timeStamp)
{
    bool rValue = false;
    this->m_mConfigIniMutex.lock();

    this->m_pConfigSettings->beginGroup(GROUP_LIST_ITEM);
    int arrayCount = this->m_pConfigSettings->beginReadArray(CONFIG_LIST_ARRAY);
    for (int i = 0; i < arrayCount; i++) {
        this->m_pConfigSettings->setArrayIndex(i);
        //        QString actName  = this->m_pConfigSettings->value(ITEM_NAME, "").toString();
        qint32 actIndex = this->m_pConfigSettings->value(ITEM_INDEX, 0).toInt();
        if (pItem->m_index == actIndex) {

            this->m_pConfigSettings->setValue(key, value);
            //            qInfo().noquote() << QString("Change %1 of item %2 to %3").arg(key).arg(pItem->m_itemName).arg(value.toString());
            rValue = true;
            break;
        }
    }
    this->m_pConfigSettings->endArray();
    this->m_pConfigSettings->endGroup();

    this->m_pConfigSettings->sync();

    this->m_mConfigIniMutex.unlock();

    this->setNewUpdateTime(timeStamp);

    return rValue;
}

void ConfigList::addNewConfigItem(ConfigItem* item, QList<ConfigItem*>* pList)
{
    QMutexLocker locker(&this->m_mInternalInfoMutex);

    if (!pList->contains(item))
        pList->append(item);
}

quint32 ConfigList::getNextInternalIndex()
{
    qint32 savedIndex, usedIndex = 0;

    foreach (ConfigItem* item, this->m_lInteralList) {
        if (item->m_index > usedIndex)
            usedIndex = item->m_index;
    }

    QMutexLocker locker(&this->m_mConfigIniMutex);

    this->m_pConfigSettings->beginGroup(ITEM_INDEX_GROUP);
    savedIndex = this->m_pConfigSettings->value(ITEM_MAX_INDEX, 0).toInt();

    if (usedIndex > savedIndex)
        savedIndex = usedIndex;

    savedIndex++;

    this->m_pConfigSettings->setValue(ITEM_MAX_INDEX, savedIndex);
    this->m_pConfigSettings->endGroup();

    return savedIndex;
}

void ConfigList::restartConfigSettings()
{
    QString fileName = this->getFileName();

    QMutexLocker locker(&this->m_mConfigIniMutex);

    delete this->m_pConfigSettings;

    this->m_pConfigSettings = new QSettings(fileName, QSettings::IniFormat);
    this->m_pConfigSettings->setIniCodec(("UTF-8"));
}

qint64 ConfigList::getLastUpdateTime()
{
    return this->m_lastUpdateTimeStamp;
}

qint64 ConfigList::readLastUpdateTime()
{
    QMutexLocker locker(&this->m_mConfigIniMutex);

    this->m_pConfigSettings->beginGroup(ITEM_UPDATE_GROUP);
    this->m_lastUpdateTimeStamp = this->m_pConfigSettings->value(ITEM_LAST_UPDATE, 0).toLongLong();
    this->m_pConfigSettings->endGroup();

    return this->getLastUpdateTime();
}

qint64 ConfigList::setNewUpdateTime(qint64 timeStamp)
{
    QMutexLocker locker(&this->m_mConfigIniMutex);

    if (timeStamp == 0)
        timeStamp = QDateTime::currentMSecsSinceEpoch();

    if (this->m_lastUpdateTimeStamp == timeStamp)
        return getLastUpdateTime();

    this->m_pConfigSettings->beginGroup(ITEM_UPDATE_GROUP);

    this->m_lastUpdateTimeStamp = timeStamp;
    this->m_pConfigSettings->setValue(ITEM_LAST_UPDATE, this->m_lastUpdateTimeStamp);

    this->m_pConfigSettings->endGroup();

    return this->getLastUpdateTime();
}

void ConfigList::sortItemListByTimeAscending()
{
    QMutexLocker locker(&this->m_mInternalInfoMutex);

    std::sort(this->m_lInteralList.begin(), this->m_lInteralList.end(), ConfigItem::compareTimeStampFunctionAscending);
}

void ConfigList::sortItemListByTimeDescending()
{
    QMutexLocker locker(&this->m_mInternalInfoMutex);

    std::sort(this->m_lInteralList.begin(), this->m_lInteralList.end(), ConfigItem::compareTimeStampFunctionDescending);
}

qint32 ConfigList::terminate()
{
    QMutexLocker locker(&this->m_mConfigIniMutex);

    if (this->m_pConfigSettings != NULL)
        delete this->m_pConfigSettings;
    this->m_pConfigSettings = NULL;

    for (int i = 0; i < this->m_lInteralList.size(); i++)
        delete this->m_lInteralList[i];

    for (int i = 0; i < this->m_lAddItemProblems.size(); i++)
        delete this->m_lAddItemProblems[i];

    return ERROR_CODE_SUCCESS;
}


ConfigList::~ConfigList()
{
    for (int i = 0; i < this->m_lInteralList.size(); i++)
        delete this->m_lInteralList[i];

    for (int i = 0; i < this->m_lAddItemProblems.size(); i++)
        delete this->m_lAddItemProblems[i];
}
