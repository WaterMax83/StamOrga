#ifndef CONFIGLIST_H
#define CONFIGLIST_H


#include <QtCore/QSettings>
#include <QtCore/QMutex>

class ConfigItem
{
public:
    quint32 m_index;
    QString m_itemName;
    qint64  m_timestamp;
};

#define GROUP_LIST_ITEM             "ListedItem"
#define CONFIG_LIST_ARRAY           "item"
#define ITEM_INDEX                  "index"
#define ITEM_NAME                   "itemName"
#define ITEM_TIMESTAMP              "timeStamp"


#define ITEM_INDEX_GROUP    "IndexCount"
#define ITEM_MAX_INDEX      "CurrentCount"

class ConfigList
{
public:
    ConfigList();

    virtual quint32 getNumberOfInternalList() = 0;

    quint16 startRequestGetItemList()
    {
        this->m_mInternalInfoMutex.lock();
        return this->getNumberOfInternalList();
    }

    virtual ConfigItem *getRequestConfigItem(int index) = 0;

    void stopRequestGetItemList()
    {
        this->m_mInternalInfoMutex.unlock();
    }

protected:
    QSettings           *m_pConfigSettings = NULL;
    QMutex              m_mConfigIniMutex;
    QMutex              m_mInternalInfoMutex;

    virtual void saveCurrentInteralList() = 0;

    quint32 getNextInternalIndex();

};

#endif // CONFIGLIST_H
