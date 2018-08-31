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
#include "meetinginfo.h"


// clang-format off
#define MEETING_HEADER          "MeetingHeader"

#define MEET_INFO_HEAD_WHEN     "when"
#define MEET_INFO_HEAD_WHERE    "where"
#define MEET_INFO_HEAD_INFO     "infoseg"
#define MEET_INFO_HEAD_TYPE     "type"

#define MEET_INFO_STATE         "acceptState"
#define MEET_INFO_USER_ID       "userID"

#define GROUP_COMMENTS          "Comments"
// clang-format on

MeetingInfo::MeetingInfo()
{
}

QString MeetingInfo::getMeetingsHeadName()
{
    return "Meetings";
}

QString MeetingInfo::getMeetingHeader()
{
    return MEETING_HEADER;
}

quint32 MeetingInfo::getMeetingType()
{
    return MEETING_TYPE_MEETING;
}

qint32 MeetingInfo::initialize(quint32 year, quint32 competition, quint32 seasonIndex, quint32 index)
{
    this->m_year        = year;
    this->m_competition = competition;
    this->m_seasonIndex = seasonIndex;
    this->m_gameIndex   = index;

    QString configSetFilePath = getUserHomeConfigPath() + "/Settings/" + this->getMeetingsHeadName() + "/";
    configSetFilePath.append(QString("%1_Game_%2.ini").arg(this->getMeetingsHeadName()).arg(index));

    if (!checkFilePathExistAndCreate(configSetFilePath)) {
        CONSOLE_CRITICAL(QString("Could not create File for UserSettings"));
        return ERROR_CODE_COMMON;
    }

    this->m_pConfigSettings = new QSettings(configSetFilePath, QSettings::IniFormat);
    this->m_pConfigSettings->setIniCodec(("UTF-8"));

    this->m_pConfigSettings->beginGroup(this->getMeetingHeader());

    this->m_pConfigSettings->setValue("year", this->m_year);
    this->m_pConfigSettings->setValue("competition", this->m_competition);
    this->m_pConfigSettings->setValue("seasonIndex", this->m_seasonIndex);
    this->m_pConfigSettings->setValue("gameIndex", this->m_gameIndex);

    this->m_pConfigSettings->setValue(MEET_INFO_HEAD_WHEN, this->m_when);
    this->m_pConfigSettings->setValue(MEET_INFO_HEAD_WHERE, this->m_where);
    this->m_pConfigSettings->setValue(MEET_INFO_HEAD_INFO, this->m_info);
    this->m_pConfigSettings->setValue(MEET_INFO_HEAD_TYPE, this->getMeetingType());

    this->m_pConfigSettings->endGroup();

    return ERROR_CODE_SUCCESS;
}

qint32 MeetingInfo::initialize(QString filePath)
{
    this->m_pConfigSettings = new QSettings(filePath, QSettings::IniFormat);
    this->m_pConfigSettings->setIniCodec(("UTF-8"));

    this->m_pConfigSettings->beginGroup(this->getMeetingHeader());

    this->m_year        = this->m_pConfigSettings->value("year", 0).toUInt();
    this->m_competition = this->m_pConfigSettings->value("competition", 0).toUInt();
    this->m_seasonIndex = this->m_pConfigSettings->value("seasonIndex", 0).toUInt();
    this->m_gameIndex   = this->m_pConfigSettings->value("gameIndex", 0).toUInt();

    this->m_when  = this->m_pConfigSettings->value(MEET_INFO_HEAD_WHEN, "").toString();
    this->m_where = this->m_pConfigSettings->value(MEET_INFO_HEAD_WHERE, "").toString();
    this->m_info  = this->m_pConfigSettings->value(MEET_INFO_HEAD_INFO, "").toString();
    this->m_type  = this->m_pConfigSettings->value(MEET_INFO_HEAD_TYPE, 0).toUInt();

    this->m_pConfigSettings->endGroup();

    if (this->m_year == 0 || this->m_competition == 0 || this->m_seasonIndex == 0 || this->m_gameIndex == 0)
        return ERROR_CODE_COMMON;

    this->m_pConfigSettings->beginGroup(GROUP_COMMENTS);
    int sizeOfArray = this->m_pConfigSettings->beginReadArray(CONFIG_LIST_ARRAY);

    for (int i = 0; i < sizeOfArray; i++) {
        this->m_pConfigSettings->setArrayIndex(i);
        ConfigItem* pComment  = new ConfigItem();
        pComment->m_itemName  = this->m_pConfigSettings->value(ITEM_NAME, "").toString();
        pComment->m_index     = this->m_pConfigSettings->value(ITEM_INDEX, 0).toUInt();
        pComment->m_timestamp = this->m_pConfigSettings->value(ITEM_TIMESTAMP, 0x0).toLongLong();

        if (pComment->m_itemName.isEmpty())
            continue;

        this->m_lComments.append(pComment);
    }
    this->m_pConfigSettings->endArray();
    this->m_pConfigSettings->endGroup();

    std::sort(this->m_lComments.begin(), this->m_lComments.end(), ConfigItem::compareTimeStampFunctionAscending);

    /* Check wheter we have to save data after reading again */
    bool bProblems = false;
    {
        QMutexLocker locker(&this->m_mConfigIniMutex);

        this->m_pConfigSettings->beginGroup(GROUP_LIST_ITEM);
        int sizeOfArray = this->m_pConfigSettings->beginReadArray(CONFIG_LIST_ARRAY);

        for (int i = 0; i < sizeOfArray; i++) {
            this->m_pConfigSettings->setArrayIndex(i);
            QString name      = this->m_pConfigSettings->value(ITEM_NAME, "").toString();
            qint32  index     = this->m_pConfigSettings->value(ITEM_INDEX, 0).toUInt();
            qint64  timestamp = this->m_pConfigSettings->value(ITEM_TIMESTAMP, 0x0).toLongLong();

            qint32 state  = this->m_pConfigSettings->value(MEET_INFO_STATE, 0x0).toUInt();
            qint32 userID = this->m_pConfigSettings->value(MEET_INFO_USER_ID, 0x0).toUInt();

            if (userID == 0 || state == 0)
                continue;

            AcceptMeetingInfo* info = new AcceptMeetingInfo(name, timestamp, index, state, userID);
            if (!this->addNewAcceptInfo(info))
                bProblems = true;
        }
        this->m_pConfigSettings->endArray();
        this->m_pConfigSettings->endGroup();
    }


    for (int i = 0; i < this->m_lAddItemProblems.size(); i++) {
        bProblems                  = true;
        AcceptMeetingInfo* pAccept = (AcceptMeetingInfo*)(this->getProblemItemFromArrayIndex(i));
        if (pAccept == NULL)
            continue;
        pAccept->m_index = this->getNextInternalIndex();
        this->addNewAcceptInfo(pAccept);
    }
    this->m_lAddItemProblems.clear();

    if (bProblems)
        this->saveCurrentInteralList();

    this->sortAcceptations();

    return ERROR_CODE_SUCCESS;
}

qint32 MeetingInfo::changeMeetingInfo(const QString when, const QString where, const QString info)
{
    if (this->m_when != when) {
        this->updateHeaderValue(MEET_INFO_HEAD_WHEN, when);
        this->m_when = when;
    }

    if (this->m_where != where) {
        this->updateHeaderValue(MEET_INFO_HEAD_WHERE, where);
        this->m_where = where;
    }

    if (this->m_info != info) {
        this->updateHeaderValue(MEET_INFO_HEAD_INFO, info);
        this->m_info = info;
    }

    return ERROR_CODE_SUCCESS;
}

qint32 MeetingInfo::getMeetingInfo(QString& when, QString& where, QString& info)
{
    when  = this->m_when;
    where = this->m_where;
    info  = this->m_info;
    return ERROR_CODE_SUCCESS;
}

qint32 MeetingInfo::addNewAcceptation(const qint32 acceptState, const qint32 userID, QString name)
{
    if (acceptState == ACCEPT_STATE_NOT_POSSIBLE) {
        qWarning().noquote() << QString("Could not add acceptation \"%1\", state 0 is not allowed").arg(name);
        return ERROR_CODE_COMMON;
    }

    if (this->itemExists(name)) {
        qWarning().noquote() << QString("Could not add acceptation \"%1\", name already exists").arg(name);
        return ERROR_CODE_ALREADY_EXIST;
    }

    int newIndex = this->getNextInternalIndex();

    QMutexLocker locker(&this->m_mConfigIniMutex);

    qint64 timestamp = QDateTime::currentDateTime().toMSecsSinceEpoch();

    this->m_pConfigSettings->beginGroup(GROUP_LIST_ITEM);
    this->m_pConfigSettings->beginWriteArray(CONFIG_LIST_ARRAY);
    this->m_pConfigSettings->setArrayIndex(this->getNumberOfInternalList());

    this->m_pConfigSettings->setValue(ITEM_NAME, name);
    this->m_pConfigSettings->setValue(ITEM_TIMESTAMP, timestamp);
    this->m_pConfigSettings->setValue(ITEM_INDEX, newIndex);

    this->m_pConfigSettings->setValue(MEET_INFO_STATE, acceptState);
    this->m_pConfigSettings->setValue(MEET_INFO_USER_ID, userID);

    this->m_pConfigSettings->endArray();
    this->m_pConfigSettings->endGroup();
    this->m_pConfigSettings->sync();

    AcceptMeetingInfo* info = new AcceptMeetingInfo(name, timestamp, newIndex, acceptState, userID);
    this->addNewAcceptInfo(info, false);

    this->sortAcceptations();

    qInfo().noquote() << QString("Added meeting accept %1:%2 for game %3").arg(name).arg(acceptState).arg(this->m_gameIndex);

    return ERROR_CODE_SUCCESS;
}

qint32 MeetingInfo::changeAcceptation(const qint32 acceptIndex, const qint32 acceptState, const qint32 userID, QString name)
{
    AcceptMeetingInfo* aInfo = (AcceptMeetingInfo*)this->getItem(acceptIndex);
    if (aInfo == NULL) {
        qWarning().noquote() << QString("Could not found a accept meeting info to change with index %1").arg(acceptIndex);
        return ERROR_CODE_NOT_FOUND;
    }

    this->m_mInternalInfoMutex.lock();

    bool bChangedItem = false;
    if (aInfo->m_state != acceptState) {
        if (this->updateItemValue(aInfo, MEET_INFO_STATE, QVariant(acceptState))) {
            aInfo->m_state = acceptState;
            qInfo().noquote() << QString("Changed accept state from game %1 to %2").arg(this->m_gameIndex).arg(acceptState);
            bChangedItem = true;
        }
    }
    if (aInfo->m_itemName != name) {
        if (this->updateItemValue(aInfo, ITEM_NAME, QVariant(name))) {
            aInfo->m_itemName = name;
            qInfo().noquote() << QString("Changed accept name from game %1 to %2").arg(this->m_gameIndex).arg(name);
            bChangedItem = true;
        }
    }
    if (aInfo->m_userID != userID) {
        if (this->updateItemValue(aInfo, MEET_INFO_USER_ID, QVariant(userID)))
            aInfo->m_userID = userID;
    }

    this->m_mInternalInfoMutex.unlock();

    if (bChangedItem)
        this->sortAcceptations();

    return ERROR_CODE_SUCCESS;
}

bool MeetingInfo::updateHeaderValue(QString key, QVariant value)
{
    bool         rValue = true;
    QMutexLocker locker(&this->m_mConfigIniMutex);

    this->m_pConfigSettings->beginGroup(this->getMeetingHeader());
    this->m_pConfigSettings->setValue(key, value);
    this->m_pConfigSettings->endGroup();
    return rValue;
}

quint16 MeetingInfo::getAcceptedNumber(const qint32 state)
{
    QMutexLocker locker(&this->m_mInternalInfoMutex);

    quint16 rValue = 0;
    for (int i = 0; i < this->getNumberOfInternalList(); i++) {
        AcceptMeetingInfo* pInfo = (AcceptMeetingInfo*)(this->getItemFromArrayIndex(i));
        if (pInfo->m_state == state)
            rValue++;
    }
    return rValue;
}

qint32 MeetingInfo::addMeetingComment(const qint32 userID, const qint64 timestamp, const QString comment)
{
    QMutexLocker locker1(&this->m_mConfigIniMutex);
    QMutexLocker locker2(&this->m_mInternalInfoMutex);

    ConfigItem* pComment  = new ConfigItem();
    pComment->m_itemName  = comment;
    pComment->m_index     = userID;
    pComment->m_timestamp = timestamp;

    this->m_pConfigSettings->beginGroup(GROUP_COMMENTS);
    this->m_pConfigSettings->beginWriteArray(CONFIG_LIST_ARRAY);
    this->m_pConfigSettings->setArrayIndex(this->m_lComments.size());

    this->m_pConfigSettings->setValue(ITEM_NAME, comment);
    this->m_pConfigSettings->setValue(ITEM_TIMESTAMP, timestamp);
    this->m_pConfigSettings->setValue(ITEM_INDEX, userID);

    this->m_pConfigSettings->endArray();
    this->m_pConfigSettings->endGroup();
    this->m_pConfigSettings->sync();

    this->m_lComments.append(pComment);

    std::sort(this->m_lComments.begin(), this->m_lComments.end(), ConfigItem::compareTimeStampFunctionAscending);

    return ERROR_CODE_SUCCESS;
}


void MeetingInfo::saveCurrentInteralList()
{
    QMutexLocker locker(&this->m_mConfigIniMutex);

    this->m_pConfigSettings->beginGroup(GROUP_LIST_ITEM);
    this->m_pConfigSettings->remove(""); // clear all elements

    this->m_pConfigSettings->beginWriteArray(CONFIG_LIST_ARRAY);
    for (int i = 0; i < this->getNumberOfInternalList(); i++) {

        AcceptMeetingInfo* pItem = (AcceptMeetingInfo*)(this->getItemFromArrayIndex(i));
        if (pItem == NULL)
            continue;
        this->m_pConfigSettings->setArrayIndex(i);

        this->m_pConfigSettings->setValue(ITEM_NAME, pItem->m_itemName);
        this->m_pConfigSettings->setValue(ITEM_TIMESTAMP, pItem->m_timestamp);
        this->m_pConfigSettings->setValue(ITEM_INDEX, pItem->m_index);

        this->m_pConfigSettings->setValue(MEET_INFO_STATE, pItem->m_state);
        this->m_pConfigSettings->setValue(MEET_INFO_USER_ID, pItem->m_userID);
    }

    this->m_pConfigSettings->endArray();
    this->m_pConfigSettings->endGroup();

    qDebug().noquote() << QString("saved Meeting List %1 with %2 entries").arg(this->m_pConfigSettings->fileName()).arg(this->getNumberOfInternalList());
}

void MeetingInfo::sortAcceptations()
{
    QMutexLocker locker(&this->m_mInternalInfoMutex);

    std::sort(this->m_lInteralList.begin(), this->m_lInteralList.end(), AcceptMeetingInfo::compareAcceptMeetingInfo);
}

bool MeetingInfo::addNewAcceptInfo(AcceptMeetingInfo* info, bool checkItem)
{
    if (checkItem) {
        if (info->m_index == 0 || itemExists(info->m_index)) {
            qWarning().noquote() << QString("Meeting acceptation with index \"%1\" already exists, saving with new index").arg(info->m_index);
            this->addNewConfigItem(info, &this->m_lAddItemProblems);
            return false;
        }
    }

    this->addNewConfigItem(info, &this->m_lInteralList);
    return true;
}
