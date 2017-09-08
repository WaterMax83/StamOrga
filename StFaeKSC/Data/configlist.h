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

#ifndef CONFIGLIST_H
#define CONFIGLIST_H


#include <QtCore/QMutex>
#include <QtCore/QSettings>

class ConfigItem
{
public:
    quint32 m_index;
    QString m_itemName;
    qint64  m_timestamp;

    static bool compareTimeStampFunction(ConfigItem* p1, ConfigItem* p2)
    {
        if (p1->m_timestamp > p2->m_timestamp)
            return false;
        return true;
    }
};

#define GROUP_LIST_ITEM "ListedItem"
#define CONFIG_LIST_ARRAY "item"
#define ITEM_INDEX "index"
#define ITEM_NAME "itemName"
#define ITEM_TIMESTAMP "timeStamp"


#define ITEM_INDEX_GROUP "IndexCount"
#define ITEM_MAX_INDEX "CurrentCount"

#define ITEM_UPDATE_GROUP "Update"
#define ITEM_LAST_UPDATE "LastUpdate"

class ConfigList
{
public:
    ConfigList();
    virtual ~ConfigList();

    virtual qint32 getNumberOfInternalList() { return this->m_lInteralList.size(); }

    qint32 removeItem(const QString name);
    qint32 removeItem(const quint32 index);
    bool itemExists(QString name);
    bool itemExists(quint32 index);
    ConfigItem* getItem(quint32 index);
    qint32 getItemIndex(const QString name);
    QString getItemName(quint32 index);

    virtual ConfigItem* getRequestConfigItemFromListIndex(int index)
    {
        QMutexLocker lock(&this->m_mInternalInfoMutex);
        return this->getItemFromArrayIndex(index);
    }

    qint64 getLastUpdateTime();

    void sortItemListByTime();

protected:
    QList<ConfigItem*> m_lInteralList;
    QList<ConfigItem*> m_lAddItemProblems;

    QSettings* m_pConfigSettings = NULL;
    QMutex     m_mConfigIniMutex;
    QMutex     m_mInternalInfoMutex;

    virtual void saveCurrentInteralList() = 0;

    ConfigItem* getItemFromArrayIndex(int index);
    ConfigItem* getProblemItemFromArrayIndex(int index);

    bool updateItemValue(ConfigItem* pItem, QString key, QVariant value, qint64 timeStamp = 0);

    void addNewConfigItem(ConfigItem* item, QList<ConfigItem*>* pList);

    quint32 getNextInternalIndex();

    qint64 m_lastUpdateTimeStamp;
    qint64 readLastUpdateTime();
    qint64 setNewUpdateTime(qint64 timeStamp = 0);
};

#endif // CONFIGLIST_H
