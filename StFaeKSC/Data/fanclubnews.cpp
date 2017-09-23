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

#include "fanclubnews.h"
#include "listeduser.h"

FanclubNews::FanclubNews()
{
    QString fanclubSetFilePath = getUserHomeConfigPath() + "/Settings/FanclubNews.ini";

    if (!checkFilePathExistAndCreate(fanclubSetFilePath)) {
        CONSOLE_CRITICAL(QString("Could not create File for fanclub setting"));
        return;
    }

    this->m_pConfigSettings = new QSettings(fanclubSetFilePath, QSettings::IniFormat);
    this->m_pConfigSettings->setIniCodec(("UTF-8"));

    /* Check wheter we have to save data after reading again */
    bool bProblems = false;
    {
        QMutexLocker locker(&this->m_mConfigIniMutex);

        this->m_pConfigSettings->beginGroup(GROUP_LIST_ITEM);
        int sizeOfArray = this->m_pConfigSettings->beginReadArray(CONFIG_LIST_ARRAY);

        for (int i = 0; i < sizeOfArray; i++) {
            this->m_pConfigSettings->setArrayIndex(i);
            QString header    = this->m_pConfigSettings->value(ITEM_NAME, "").toString();
            qint64  timestamp = this->m_pConfigSettings->value(ITEM_TIMESTAMP, 0x0).toLongLong();
            quint32 index     = this->m_pConfigSettings->value(ITEM_INDEX, 0).toInt();

            QByteArray newsText = this->m_pConfigSettings->value(NEWS_DATA_TEXT, "").toByteArray();
            quint32    userID   = this->m_pConfigSettings->value(NEWS_DATA_USERID, false).toUInt();

            if (header == "") {
                bProblems = true;
                continue;
            }
            NewsData* news = new NewsData(header, index, timestamp, newsText, userID);
            if (!this->addNewNewsData(news))
                bProblems = true;
        }
        this->m_pConfigSettings->endArray();
        this->m_pConfigSettings->endGroup();
    }
    if (this->readLastUpdateTime() == 0)
        this->setNewUpdateTime();

    for (int i = 0; i < this->m_lAddItemProblems.size(); i++) {
        bProblems       = true;
        NewsData* pItem = (NewsData*)(this->getProblemItemFromArrayIndex(i));
        if (pItem == NULL)
            continue;
        pItem->m_index = this->getNextInternalIndex();
        this->addNewNewsData(pItem);
    }
    this->m_lAddItemProblems.clear();

    this->sortItemListByTime();

    if (bProblems)
        this->saveCurrentInteralList();
}

int FanclubNews::addNewFanclubNews(const QString header, const QByteArray info, const quint32 userID)
{
    qint64 timestamp = QDateTime::currentMSecsSinceEpoch();
    int    newIndex  = this->getNextInternalIndex();

    this->m_mConfigIniMutex.lock();

    this->m_pConfigSettings->beginGroup(GROUP_LIST_ITEM);
    this->m_pConfigSettings->beginWriteArray(CONFIG_LIST_ARRAY);
    this->m_pConfigSettings->setArrayIndex(this->getNumberOfInternalList());

    this->m_pConfigSettings->setValue(ITEM_NAME, header);
    this->m_pConfigSettings->setValue(ITEM_TIMESTAMP, timestamp);
    this->m_pConfigSettings->setValue(ITEM_INDEX, newIndex);

    this->m_pConfigSettings->setValue(NEWS_DATA_TEXT, info);
    this->m_pConfigSettings->setValue(NEWS_DATA_USERID, userID);

    this->m_pConfigSettings->endArray();
    this->m_pConfigSettings->endGroup();
    this->m_pConfigSettings->sync();

    this->m_mConfigIniMutex.unlock();

    NewsData* item = new NewsData(header, newIndex, timestamp, info, userID);

    this->addNewNewsData(item, false);

    this->sortItemListByTime();

    this->setNewUpdateTime();

    qInfo().noquote() << QString("Added new fanclub news: %1").arg(header);
    return newIndex;
}

int FanclubNews::changeFanclubNews(const quint32 newsIndex, const QString header, const QByteArray info, const quint32 userID)
{
    NewsData* pItem =  (NewsData*)this->getItem(newsIndex);
    if (pItem == NULL)
        return ERROR_CODE_NOT_FOUND;

    bool bChangedItem = false;
    if (pItem->m_itemName != header) {
        if (this->updateItemValue(pItem, ITEM_NAME, QVariant(header))) {
            pItem->m_itemName = header;
            bChangedItem = true;
        }
    }

    if (pItem->m_newsText != info) {
        if (this->updateItemValue(pItem, NEWS_DATA_TEXT, QVariant(info))) {
            pItem->m_newsText = info;
            bChangedItem = true;
        }
    }

    if (pItem->m_userID != userID) {
        if (this->updateItemValue(pItem, NEWS_DATA_USERID, QVariant(userID))) {
            pItem->m_userID = userID;
            bChangedItem = true;
        }
    }

    if (bChangedItem) {
        qint64 timestamp = QDateTime::currentMSecsSinceEpoch();
        if (this->updateItemValue(pItem, ITEM_TIMESTAMP, QVariant(timestamp)))
            pItem->m_timestamp = timestamp;
    }

    return ERROR_CODE_SUCCESS;
}


int FanclubNews::showNewsData()
{
    QMutexLocker locker(&this->m_mInternalInfoMutex);

    for (int i = 0; i < this->getNumberOfInternalList(); i++) {
        NewsData* pItem = (NewsData*)(this->getItemFromArrayIndex(i));
        if (pItem == NULL)
            continue;

        QString date   = QDateTime::fromMSecsSinceEpoch(pItem->m_timestamp).toString("dd.MM.yyyy hh:mm");
        QString output = QString("%1 - %2: %3 - %4")
                             .arg(pItem->m_index, 2, 10, QChar('0'))
                             .arg(date, g_ListedUser->getItemName(pItem->m_userID), pItem->m_itemName);

        std::cout << output.toStdString() << std::endl;
    }
    return 0;
}

void FanclubNews::saveCurrentInteralList()
{
    this->m_mConfigIniMutex.lock();

    this->m_pConfigSettings->beginGroup(GROUP_LIST_ITEM);
    this->m_pConfigSettings->remove(""); // clear all elements

    this->m_pConfigSettings->beginWriteArray(CONFIG_LIST_ARRAY);
    for (int i = 0; i < this->getNumberOfInternalList(); i++) {
        NewsData* pItem = (NewsData*)(this->getItemFromArrayIndex(i));
        if (pItem == NULL)
            continue;

        this->m_pConfigSettings->setArrayIndex(i);

        this->m_pConfigSettings->setValue(ITEM_NAME, pItem->m_itemName);
        this->m_pConfigSettings->setValue(ITEM_TIMESTAMP, pItem->m_timestamp);
        this->m_pConfigSettings->setValue(ITEM_INDEX, pItem->m_index);

        this->m_pConfigSettings->setValue(NEWS_DATA_TEXT, pItem->m_newsText);
        this->m_pConfigSettings->setValue(NEWS_DATA_USERID, pItem->m_userID);
    }

    this->m_pConfigSettings->endArray();
    this->m_pConfigSettings->endGroup();

    this->m_mConfigIniMutex.unlock();

    this->setNewUpdateTime();

    qDebug().noquote() << QString("saved current NewsData List with %1 entries").arg(this->getNumberOfInternalList());
}

bool FanclubNews::addNewNewsData(NewsData* news, bool checkItem)
{
    if (checkItem) {
        if (news->m_index == 0 || itemExists(news->m_index)) {
            qWarning().noquote() << QString("NewsData with index \"%1\" already exists, saving with new index").arg(news->m_index);
            this->addNewConfigItem(news, &this->m_lAddItemProblems);
            return false;
        }
    }

    this->addNewConfigItem(news, &this->m_lInteralList);
    return true;
}
