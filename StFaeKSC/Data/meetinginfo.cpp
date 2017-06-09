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
#define MEET_INFO_HEAD_WHEN     "when"
#define MEET_INFO_HEAD_WHERE    "where"
#define MEET_INFO_HEAD_INFO     "infoseg"

#define MEET_INFO_STATE         "acceptState"
#define MEET_INFO_USER_ID       "userID"
// clang-format on

MeetingInfo::MeetingInfo()
{
}

qint32 MeetingInfo::initialize(quint32 year, quint32 competition, quint32 seasonIndex, quint32 index)
{
    this->m_year        = year;
    this->m_competition = competition;
    this->m_seasonIndex = seasonIndex;
    this->m_gameIndex   = index;

    QString userSetFilePath = getUserHomeConfigPath() + "/Settings/Meetings/";
    userSetFilePath.append(QString("Meetings_Game_%1.ini").arg(index));

    if (!checkFilePathExistAndCreate(userSetFilePath)) {
        CONSOLE_CRITICAL(QString("Could not create File for UserSettings"));
        return ERROR_CODE_COMMON;
    }

    this->m_pConfigSettings = new QSettings(userSetFilePath, QSettings::IniFormat);
    this->m_pConfigSettings->setIniCodec(("UTF-8"));

    this->m_pConfigSettings->beginGroup("MeetingHeader");

    this->m_pConfigSettings->setValue("year", this->m_year);
    this->m_pConfigSettings->setValue("competition", this->m_competition);
    this->m_pConfigSettings->setValue("seasonIndex", this->m_seasonIndex);
    this->m_pConfigSettings->setValue("gameIndex", this->m_gameIndex);

    this->m_pConfigSettings->setValue(MEET_INFO_HEAD_WHEN, this->m_when);
    this->m_pConfigSettings->setValue(MEET_INFO_HEAD_WHERE, this->m_where);
    this->m_pConfigSettings->setValue(MEET_INFO_HEAD_INFO, this->m_info);

    this->m_pConfigSettings->endGroup();

    return ERROR_CODE_SUCCESS;
}

qint32 MeetingInfo::initialize(QString filePath)
{
    this->m_pConfigSettings = new QSettings(filePath, QSettings::IniFormat);
    this->m_pConfigSettings->setIniCodec(("UTF-8"));

    this->m_pConfigSettings->beginGroup("MeetingHeader");

    this->m_year        = this->m_pConfigSettings->value("year", 0).toUInt();
    this->m_competition = this->m_pConfigSettings->value("competition", 0).toUInt();
    this->m_seasonIndex = this->m_pConfigSettings->value("seasonIndex", 0).toUInt();
    this->m_gameIndex   = this->m_pConfigSettings->value("gameIndex", 0).toUInt();

    this->m_when  = this->m_pConfigSettings->value(MEET_INFO_HEAD_WHEN, "").toString();
    this->m_where = this->m_pConfigSettings->value(MEET_INFO_HEAD_WHERE, "").toString();
    this->m_info  = this->m_pConfigSettings->value(MEET_INFO_HEAD_INFO, "").toString();

    this->m_pConfigSettings->endGroup();

    if (this->m_year == 0 || this->m_competition == 0 || this->m_seasonIndex == 0 || this->m_gameIndex == 0)
        return ERROR_CODE_COMMON;

    /* Check wheter we have to save data after reading again */
    bool bProblems = false;
    {
        QMutexLocker locker(&this->m_mConfigIniMutex);

        this->m_pConfigSettings->beginGroup(GROUP_LIST_ITEM);
        int sizeOfLogins = this->m_pConfigSettings->beginReadArray(CONFIG_LIST_ARRAY);

        for (int i = 0; i < sizeOfLogins; i++) {
            this->m_pConfigSettings->setArrayIndex(i);
            QString name      = this->m_pConfigSettings->value(ITEM_NAME, "").toString();
            quint32 index     = this->m_pConfigSettings->value(ITEM_INDEX, 0).toUInt();
            qint64  timestamp = this->m_pConfigSettings->value(ITEM_TIMESTAMP, 0x0).toULongLong();

            quint32 state  = this->m_pConfigSettings->value(MEET_INFO_STATE, 0x0).toUInt();
            quint32 userID = this->m_pConfigSettings->value(MEET_INFO_USER_ID, 0x0).toUInt();

            if (userID == 0 || state == 0)
                continue;

            if (!this->addNewAcceptInfo(name, timestamp, index, state, userID))
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
        this->addNewAcceptInfo(pAccept->m_itemName, pAccept->m_timestamp,
                               pAccept->m_index, pAccept->m_state,
                               pAccept->m_userID);

        delete pAccept;
    }
    this->m_lAddItemProblems.clear();

    if (bProblems)
        this->saveCurrentInteralList();
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

qint32 MeetingInfo::addNewAcceptation(const quint32 acceptState, const quint32 userID, QString name)
{
    if (acceptState == ACCEPT_STATE_NOT_POSSIBLE) {
        qWarning().noquote() << QString("Could not add acceptation \"%1\", state 0 is not allowed ").arg(name);
        return ERROR_CODE_COMMON;
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

    this->addNewAcceptInfo(name, timestamp, newIndex, acceptState, userID, false);

    qInfo().noquote() << QString("Added meeting accept %1:%2 for game %3").arg(name).arg(acceptState).arg(this->m_gameIndex);

    return ERROR_CODE_SUCCESS;
}

qint32 MeetingInfo::changeAcceptation(const quint32 acceptIndex, const quint32 acceptState, const quint32 userID, QString name)
{
    AcceptMeetingInfo* aInfo = (AcceptMeetingInfo*)this->getItem(acceptIndex);
    if (aInfo == NULL) {
        qWarning().noquote() << QString("Could not found a accept meeting info to change with index %1").arg(acceptIndex);
        return ERROR_CODE_NOT_FOUND;
    }

    QMutexLocker locker(&this->m_mInternalInfoMutex);

    if (aInfo->m_state != acceptState) {
        if (this->updateItemValue(aInfo, MEET_INFO_STATE, QVariant(acceptState))) {
            aInfo->m_state = acceptState;
            qInfo().noquote() << QString("Changed accept state from game %1 to %2").arg(this->m_gameIndex).arg(acceptState);
        }
    }
    if (aInfo->m_itemName != name) {
        if (this->updateItemValue(aInfo, ITEM_NAME, QVariant(name))) {
            aInfo->m_itemName = name;
            qInfo().noquote() << QString("Changed accept name from game %1 to %2").arg(this->m_gameIndex).arg(name);
        }
    }
    if (aInfo->m_userID != userID) {
        if (this->updateItemValue(aInfo, MEET_INFO_USER_ID, QVariant(userID))) {
            aInfo->m_userID = userID;
        }
    }

    return ERROR_CODE_SUCCESS;
}

bool MeetingInfo::updateHeaderValue(QString key, QVariant value)
{
    bool         rValue = true;
    QMutexLocker locker(&this->m_mConfigIniMutex);

    this->m_pConfigSettings->beginGroup("MeetingHeader");
    this->m_pConfigSettings->setValue(key, value);
    this->m_pConfigSettings->endGroup();
    return rValue;
}


void MeetingInfo::saveCurrentInteralList()
{
    QMutexLocker locker(&this->m_mConfigIniMutex);

    this->m_pConfigSettings->beginGroup(GROUP_LIST_ITEM);
    this->m_pConfigSettings->remove(""); // clear all elements

    this->m_pConfigSettings->beginWriteArray(CONFIG_LIST_ARRAY);
    for (int i = 0; i < this->getNumberOfInternalList(); i++) {

        AcceptMeetingInfo* accept = (AcceptMeetingInfo*)(this->getItemFromArrayIndex(i));
        if (accept == NULL)
            continue;
        this->m_pConfigSettings->setArrayIndex(i);

        this->m_pConfigSettings->setValue(ITEM_NAME, accept->m_itemName);
        this->m_pConfigSettings->setValue(ITEM_TIMESTAMP, accept->m_timestamp);
        this->m_pConfigSettings->setValue(ITEM_INDEX, accept->m_index);

        this->m_pConfigSettings->setValue(MEET_INFO_STATE, accept->m_state);
        this->m_pConfigSettings->setValue(MEET_INFO_USER_ID, accept->m_userID);
    }

    this->m_pConfigSettings->endArray();
    this->m_pConfigSettings->endGroup();

    qDebug().noquote() << QString("saved Meeting List %1 with %2 entries").arg(this->m_pConfigSettings->fileName()).arg(this->getNumberOfInternalList());
}

bool MeetingInfo::addNewAcceptInfo(QString name, qint64 timestamp, quint32 index, quint32 state, quint32 userID, bool checkAccept)
{
    if (checkAccept) {
        if (index == 0 || itemExists(index)) {
            qWarning().noquote() << QString("Meeting acceptation with index \"%1\" already exists, saving with new index").arg(index);
            this->addNewAcceptInfo(name, timestamp, index, state, userID, &this->m_lAddItemProblems);
            return false;
        }
    }

    this->addNewAcceptInfo(name, timestamp, index, state, userID, &this->m_lInteralList);
    return true;
}

void MeetingInfo::addNewAcceptInfo(QString name, qint64 timestamp, quint32 index, quint32 state, quint32 userID, QList<ConfigItem*>* pList)
{
    QMutexLocker locker(&this->m_mInternalInfoMutex);

    AcceptMeetingInfo* accept = new AcceptMeetingInfo();
    accept->m_itemName        = name;
    accept->m_timestamp       = timestamp;
    accept->m_index           = index;
    accept->m_state           = state;
    accept->m_userID          = userID;

    pList->append(accept);
}
