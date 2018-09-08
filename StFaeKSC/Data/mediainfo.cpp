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

#include <QtCore/QDateTime>
#include <QtCore/QDebug>

#include "../Common/General/globalfunctions.h"
#include "mediainfo.h"

// clang-format off
#define MEDIA_HEADER          "MediaHeader"

#define MEDIA_USER_ID         "userID"

// clang-format on

MediaInfo::MediaInfo()
{
}


qint32 MediaInfo::initialize(quint32 year, quint32 competition, quint32 seasonIndex, quint32 index)
{
    this->m_year        = year;
    this->m_competition = competition;
    this->m_seasonIndex = seasonIndex;
    this->m_gameIndex   = index;

    QString configSetFilePath = getUserHomeConfigPath() + "/Settings/Media/";
    configSetFilePath.append(QString("Media_Game_%1.ini").arg(index));

    if (!checkFilePathExistAndCreate(configSetFilePath)) {
        CONSOLE_CRITICAL(QString("Could not create File for Media Settings"));
        return ERROR_CODE_COMMON;
    }

    this->m_pConfigSettings = new QSettings(configSetFilePath, QSettings::IniFormat);
    this->m_pConfigSettings->setIniCodec(("UTF-8"));

    this->m_pConfigSettings->beginGroup(MEDIA_HEADER);

    this->m_pConfigSettings->setValue("year", this->m_year);
    this->m_pConfigSettings->setValue("competition", this->m_competition);
    this->m_pConfigSettings->setValue("seasonIndex", this->m_seasonIndex);
    this->m_pConfigSettings->setValue("gameIndex", this->m_gameIndex);

    this->m_pConfigSettings->endGroup();

    return ERROR_CODE_SUCCESS;
}

qint32 MediaInfo::initialize(QString filePath)
{
    this->m_pConfigSettings = new QSettings(filePath, QSettings::IniFormat);
    this->m_pConfigSettings->setIniCodec(("UTF-8"));

    this->m_pConfigSettings->beginGroup(MEDIA_HEADER);

    this->m_year        = this->m_pConfigSettings->value("year", 0).toUInt();
    this->m_competition = this->m_pConfigSettings->value("competition", 0).toUInt();
    this->m_seasonIndex = this->m_pConfigSettings->value("seasonIndex", 0).toInt();
    this->m_gameIndex   = this->m_pConfigSettings->value("gameIndex", 0).toInt();

    this->m_pConfigSettings->endGroup();

    if (this->m_year == 0 || this->m_competition == 0 || this->m_seasonIndex == 0 || this->m_gameIndex == 0)
        return ERROR_CODE_COMMON;

    /* Check wheter we have to save data after reading again */
    bool bProblems = false;
    {
        QMutexLocker locker(&this->m_mConfigIniMutex);

        this->m_pConfigSettings->beginGroup(GROUP_LIST_ITEM);
        int sizeOfArray = this->m_pConfigSettings->beginReadArray(CONFIG_LIST_ARRAY);

        for (int i = 0; i < sizeOfArray; i++) {
            this->m_pConfigSettings->setArrayIndex(i);
            MediaItem* media   = new MediaItem();
            media->m_itemName  = this->m_pConfigSettings->value(ITEM_NAME, "").toString();
            media->m_index     = this->m_pConfigSettings->value(ITEM_INDEX, 0).toInt();
            media->m_timestamp = this->m_pConfigSettings->value(ITEM_TIMESTAMP, 0x0).toLongLong();
            media->m_userID    = this->m_pConfigSettings->value(MEDIA_USER_ID, 0).toInt();

            if (!this->addNewMediaInfo(media))
                bProblems = true;
        }
        this->m_pConfigSettings->endArray();
        this->m_pConfigSettings->endGroup();
    }


    for (int i = 0; i < this->m_lAddItemProblems.size(); i++) {
        bProblems         = true;
        MediaItem* pMedia = (MediaItem*)(this->getProblemItemFromArrayIndex(i));
        if (pMedia == NULL)
            continue;
        pMedia->m_index = this->getNextInternalIndex();
        this->addNewMediaInfo(pMedia);
    }
    this->m_lAddItemProblems.clear();

    if (bProblems)
        this->saveCurrentInteralList();

    this->sortMedias();

    return ERROR_CODE_SUCCESS;
}

qint32 MediaInfo::addNewMediaItem(const QString format, QByteArray& data, const qint32 userID)
{
    Q_UNUSED(data);

    int     newIndex = this->getNextInternalIndex();
    QString name     = QString("Media_Game_%1_%2.%3").arg(this->m_gameIndex).arg(newIndex).arg(format);

    QMutexLocker locker(&this->m_mConfigIniMutex);

    qint64 timestamp = QDateTime::currentDateTime().toMSecsSinceEpoch();

    this->m_pConfigSettings->beginGroup(GROUP_LIST_ITEM);
    this->m_pConfigSettings->beginWriteArray(CONFIG_LIST_ARRAY);
    this->m_pConfigSettings->setArrayIndex(this->getNumberOfInternalList());

    this->m_pConfigSettings->setValue(ITEM_NAME, name);
    this->m_pConfigSettings->setValue(ITEM_TIMESTAMP, timestamp);
    this->m_pConfigSettings->setValue(ITEM_INDEX, newIndex);

    this->m_pConfigSettings->setValue(MEDIA_USER_ID, userID);

    this->m_pConfigSettings->endArray();
    this->m_pConfigSettings->endGroup();
    this->m_pConfigSettings->sync();

    MediaItem* info   = new MediaItem();
    info->m_itemName  = name;
    info->m_timestamp = timestamp;
    info->m_index     = newIndex;
    info->m_userID    = userID;
    this->addNewMediaInfo(info, false);

    this->sortMedias();


    return ERROR_CODE_SUCCESS;
}

qint32 MediaInfo::removeMediaItems(QStringList& lMedias)
{
    if (lMedias.size() == 0)
        return ERROR_CODE_COMMON;

    QMutexLocker locker(&this->m_mInternalInfoMutex);

    qint32 removeCounter = 0;
    foreach (QString url, lMedias) {
        for (int i = 0; i < this->m_lInteralList.size(); i++) {
            if (this->m_lInteralList[i]->m_itemName == url) {
                this->m_lInteralList.removeAt(i);
                removeCounter++;
            }
        }
    }

    if (removeCounter > 0) {
        this->saveCurrentInteralList();
        return ERROR_CODE_SUCCESS;
    }

    return ERROR_CODE_NOT_FOUND;
}

void MediaInfo::saveCurrentInteralList()
{
    QMutexLocker locker(&this->m_mConfigIniMutex);

    this->m_pConfigSettings->beginGroup(GROUP_LIST_ITEM);
    this->m_pConfigSettings->remove(""); // clear all elements

    this->m_pConfigSettings->beginWriteArray(CONFIG_LIST_ARRAY);
    for (int i = 0; i < this->getNumberOfInternalList(); i++) {

        MediaItem* pItem = (MediaItem*)(this->getItemFromArrayIndex(i));
        if (pItem == NULL)
            continue;
        this->m_pConfigSettings->setArrayIndex(i);

        this->m_pConfigSettings->setValue(ITEM_NAME, pItem->m_itemName);
        this->m_pConfigSettings->setValue(ITEM_TIMESTAMP, pItem->m_timestamp);
        this->m_pConfigSettings->setValue(ITEM_INDEX, pItem->m_index);

        this->m_pConfigSettings->setValue(MEDIA_USER_ID, pItem->m_userID);
    }

    this->m_pConfigSettings->endArray();
    this->m_pConfigSettings->endGroup();

    qDebug().noquote() << QString("saved Media Info %1 with %2 entries").arg(this->m_pConfigSettings->fileName()).arg(this->getNumberOfInternalList());
}

void MediaInfo::sortMedias()
{
    QMutexLocker locker(&this->m_mInternalInfoMutex);

    std::sort(this->m_lInteralList.begin(), this->m_lInteralList.end(), ConfigItem::compareTimeStampFunctionAscending);
}

bool MediaInfo::addNewMediaInfo(MediaItem* media, bool checkItem)
{
    if (checkItem) {
        if (media->m_index == 0 || itemExists(media->m_index)) {
            qWarning().noquote() << QString("Media info with index \"%1\" already exists, saving with new index").arg(media->m_index);
            this->addNewConfigItem(media, &this->m_lAddItemProblems);
            return false;
        }
    }

    this->addNewConfigItem(media, &this->m_lInteralList);
    return true;
}
