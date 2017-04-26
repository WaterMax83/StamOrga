/*
*	This file is part of StamOrga
*   Copyright (C) 2017 Markus Schneider
*
*	This program is free software; you can redistribute it and/or modify
*   it under the terms of the GNU General Public License as published by
*   the Free Software Foundation; either version 3 of the License, or
*   (at your option) any later version.
*
*	Foobar is distributed in the hope that it will be useful,
*    but WITHOUT ANY WARRANTY; without even the implied warranty of
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*    GNU General Public License for more details.

*    You should have received a copy of the GNU General Public License
*    along with Foobar.  If not, see <http://www.gnu.org/licenses/>.
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

class ConfigList
{
public:
    ConfigList();

    virtual qint32 getNumberOfInternalList() { return this->m_lInteralList.size(); }

    qint32 removeItem(const QString name);
    qint32 removeItem(const quint32 index);
    bool itemExists(QString name);
    bool itemExists(quint32 index);
    quint32 getItemIndex(const QString name);

    quint16 startRequestGetItemList()
    {
        this->m_mInternalInfoMutex.lock();
        return this->getNumberOfInternalList();
    }

    virtual ConfigItem* getRequestConfigItem(int index)
    {
        if (index < this->m_lInteralList.size())
            return this->m_lInteralList[index];
        return NULL;
    }

    void stopRequestGetItemList()
    {
        this->m_mInternalInfoMutex.unlock();
    }

protected:
    QList<ConfigItem*> m_lInteralList;
    QList<ConfigItem*> m_lAddItemProblems;

    QSettings* m_pConfigSettings = NULL;
    QMutex     m_mConfigIniMutex;
    QMutex     m_mInternalInfoMutex;

    virtual void saveCurrentInteralList() = 0;

    ConfigItem* getItemFromArrayIndex(int index);
    ConfigItem* getProblemItemFromArrayIndex(int index);

    bool updateItemValue(ConfigItem* pItem, QString key, QVariant value);

    quint32 getNextInternalIndex();
};

#endif // CONFIGLIST_H
