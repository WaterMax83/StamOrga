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

#include "userinformation.h"
#include "../Common/General/globalfunctions.h"

UserInformation::UserInformation()
{
    QString configSetFilePath = getUserHomeConfigPath() + "/Settings/UserInformation.ini";

    if (!checkFilePathExistAndCreate(configSetFilePath)) {
        CONSOLE_CRITICAL(QString("Could not create File for UserInformation"));
        return;
    }

    this->m_pConfigSettings = new QSettings(configSetFilePath, QSettings::IniFormat);
    this->m_pConfigSettings->setIniCodec(("UTF-8"));

    /* Check wheter we have to save data after reading again */
    bool bProblems = false;
    {
        QMutexLocker locker(&this->m_mConfigIniMutex);

        this->m_pConfigSettings->beginGroup(GROUP_LIST_ITEM);
        int sizeOfArray = this->m_pConfigSettings->beginReadArray(CONFIG_LIST_ARRAY);

        for (int i = 0; i < sizeOfArray; i++) {
            this->m_pConfigSettings->setArrayIndex(i);
            QString name      = this->m_pConfigSettings->value(ITEM_NAME, "").toString();
            qint32  index     = this->m_pConfigSettings->value(ITEM_INDEX, 0).toInt();
            qint64  timestamp = this->m_pConfigSettings->value(ITEM_TIMESTAMP, 0x0).toLongLong();

            UserStats* pUser = new UserStats(name, timestamp, index);
            if (!this->addNewUserStats(pUser))
                bProblems = true;
        }
        this->m_pConfigSettings->endArray();
        this->m_pConfigSettings->endGroup();
    }


    //    for (int i = 0; i < this->m_lAddItemProblems.size(); i++) {
    //        bProblems        = true;
    //        UserStats* pUser = (UserStats*)(this->getProblemItemFromArrayIndex(i));
    //        if (pUser == NULL)
    //            continue;
    //        pUser->m_index = this->getNextInternalIndex();
    //        this->addNewUserStats(pUser);
    //    }
    this->m_lAddItemProblems.clear();

    if (bProblems)
        this->saveCurrentInteralList();
}

int UserInformation::addUserInfo(const QString& name, const qint64 timestamp, const qint32 index)
{
    if (this->itemExists(index)) {
        CONSOLE_WARNING(QString("User \"%1\" already exists").arg(index));
        return -1;
    }

    QMutexLocker locker(&this->m_mConfigIniMutex);

    this->m_pConfigSettings->beginGroup(GROUP_LIST_ITEM);
    this->m_pConfigSettings->beginWriteArray(CONFIG_LIST_ARRAY);
    this->m_pConfigSettings->setArrayIndex(this->getNumberOfInternalList());

    this->m_pConfigSettings->setValue(ITEM_NAME, name);
    this->m_pConfigSettings->setValue(ITEM_TIMESTAMP, timestamp);
    this->m_pConfigSettings->setValue(ITEM_INDEX, index);

    this->m_pConfigSettings->endArray();
    this->m_pConfigSettings->endGroup();
    this->m_pConfigSettings->sync();

    UserStats* pUser = new UserStats(name, timestamp, index);
    this->addNewUserStats(pUser, false);

    qInfo().noquote() << QString("Added new user information: %1").arg(name);
    return index;
}

QString UserInformation::getReadableName(const qint32 userIndex)
{
    UserStats* pUser = (UserStats*)(this->getItem(userIndex));
    if (pUser == NULL)
        return "";

    QMutexLocker locker(&this->m_mInternalInfoMutex);

    return pUser->m_readName;
}

bool UserInformation::userChangeReadName(const qint32 userIndex, const QString& readName)
{
    if (readName.length() < 3)
        return false;

    UserStats* pUser = (UserStats*)(this->getItem(userIndex));
    if (pUser == NULL)
        return false;

    QMutexLocker locker(&this->m_mInternalInfoMutex);

    if (this->updateItemValue(pUser, ITEM_NAME, QVariant(readName))) {
        pUser->m_itemName = readName;
        pUser->m_readName = readName;
        return true;
    }

    return false;
}

bool UserInformation::userChangeOnlineTime(const qint32 userIndex, const qint64 timestamp)
{
    UserStats* pUser = (UserStats*)(this->getItem(userIndex));
    if (pUser == NULL)
        return false;

    QMutexLocker locker(&this->m_mInternalInfoMutex);

    if (this->updateItemValue(pUser, ITEM_TIMESTAMP, QVariant(timestamp))) {
        pUser->m_timestamp  = timestamp;
        pUser->m_lastOnline = timestamp;
        return true;
    }

    return false;
}

void UserInformation::saveCurrentInteralList()
{
    QMutexLocker locker(&this->m_mConfigIniMutex);

    this->m_pConfigSettings->beginGroup(GROUP_LIST_ITEM);
    this->m_pConfigSettings->remove(""); // clear all elements

    this->m_pConfigSettings->beginWriteArray(CONFIG_LIST_ARRAY);
    for (int i = 0; i < this->getNumberOfInternalList(); i++) {

        UserStats* pItem = (UserStats*)(this->getItemFromArrayIndex(i));
        if (pItem == NULL)
            continue;
        this->m_pConfigSettings->setArrayIndex(i);

        this->m_pConfigSettings->setValue(ITEM_NAME, pItem->m_itemName);
        this->m_pConfigSettings->setValue(ITEM_TIMESTAMP, pItem->m_timestamp);
        this->m_pConfigSettings->setValue(ITEM_INDEX, pItem->m_index);
    }

    this->m_pConfigSettings->endArray();
    this->m_pConfigSettings->endGroup();

    qInfo().noquote() << QString("saved current User Information List with %1 entries").arg(this->getNumberOfInternalList());
}

bool UserInformation::addNewUserStats(UserStats* pUser, bool checkItem)
{
    if (checkItem) {
        if (pUser->m_index == 0 || itemExists(pUser->m_index)) {
            qWarning().noquote() << QString("User \"%1\" with index \"%2\" already exists, saving with new index").arg(pUser->m_itemName).arg(pUser->m_index);
            this->addNewConfigItem(pUser, &this->m_lAddItemProblems);
            return false;
        }
    }

    this->addNewConfigItem(pUser, &this->m_lInteralList);
    return true;
}

UserInformation::~UserInformation()
{
    if (this->m_pConfigSettings != NULL)
        delete this->m_pConfigSettings;
}
