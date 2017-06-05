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

#include "meetinginfo.h"
#include "../Common/General/globalfunctions.h"

#define MEET_INFO_HEAD_WHEN "when"
#define MEET_INFO_HEAD_WHERE "where"
#define MEET_INFO_HEAD_INFO "infoseg"

MeetingInfo::MeetingInfo()
{

}

qint32 MeetingInfo::initialize(quint32 year, quint32 competition, quint32 seasonIndex, quint32 index)
{
    this->m_year        = year;
    this->m_competition = competition;
    this->m_seasonIndex = seasonIndex;
    this->m_gameIndex   = index;

    QString userSetFilePath = getUserHomeConfigPath() + "/Settings/";
    userSetFilePath.append(QString("Meetings_Game_%1.ini").arg(index));

    if (!checkFilePathExistAndCreate(userSetFilePath)) {
        CONSOLE_CRITICAL(QString("Could not create File for UserSettings"));
        return ERROR_CODE_COMMON;
    }

    this->m_pConfigSettings = new QSettings(userSetFilePath, QSettings::IniFormat);

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

    this->m_pConfigSettings->beginGroup("MeetingHeader");

    this->m_year        = this->m_pConfigSettings->value("year", 0).toUInt();
    this->m_competition = this->m_pConfigSettings->value("competition", 0).toUInt();
    this->m_seasonIndex = this->m_pConfigSettings->value("seasonIndex", 0).toUInt();
    this->m_gameIndex   = this->m_pConfigSettings->value("gameIndex", 0).toUInt();

    this->m_when = this->m_pConfigSettings->value(MEET_INFO_HEAD_WHEN, "").toString();
    this->m_where = this->m_pConfigSettings->value(MEET_INFO_HEAD_WHERE, "").toString();
    this->m_info   = this->m_pConfigSettings->value(MEET_INFO_HEAD_INFO, "").toString();

    this->m_pConfigSettings->endGroup();

    if (this->m_year == 0 || this->m_competition == 0 || this->m_seasonIndex == 0 || this->m_gameIndex == 0)
        return ERROR_CODE_COMMON;

//    /* Check wheter we have to save data after reading again */
//    bool bProblems = false;
//    {
//        QMutexLocker locker(&this->m_mConfigIniMutex);

//        this->m_pConfigSettings->beginGroup(GROUP_LIST_ITEM);
//        int sizeOfLogins = this->m_pConfigSettings->beginReadArray(CONFIG_LIST_ARRAY);

//        for (int i = 0; i < sizeOfLogins; i++) {
//            this->m_pConfigSettings->setArrayIndex(i);
//            QString name      = this->m_pConfigSettings->value(ITEM_NAME, "").toString();
//            quint32 index     = this->m_pConfigSettings->value(ITEM_INDEX, 0).toUInt();
//            qint64  timestamp = this->m_pConfigSettings->value(ITEM_TIMESTAMP, 0x0).toULongLong();

//            quint32 ticketID = this->m_pConfigSettings->value(AVAILABLE_TICKET_ID, 0x0).toUInt();
//            quint32 userID   = this->m_pConfigSettings->value(AVAILABLE_USER_ID, 0x0).toUInt();
//            quint32 state    = this->m_pConfigSettings->value(AVAILABLE_STATE, 0).toUInt();

//            if (ticketID == 0 || userID == 0 || state == 0)
//                continue;

//            if (!this->addNewAvailableTicket(name, timestamp, index, ticketID, userID, state))
//                bProblems = true;
//        }
//        this->m_pConfigSettings->endArray();
//        this->m_pConfigSettings->endGroup();
//    }


//    for (int i = 0; i < this->m_lAddItemProblems.size(); i++) {
//        bProblems                    = true;
//        AvailableTicketInfo* pTicket = (AvailableTicketInfo*)(this->getProblemItemFromArrayIndex(i));
//        if (pTicket == NULL)
//            continue;
//        pTicket->m_index = this->getNextInternalIndex();
//        this->addNewAvailableTicket(pTicket->m_itemName, pTicket->m_timestamp,
//                                    pTicket->m_index, pTicket->m_ticketID,
//                                    pTicket->m_userID, pTicket->m_state);

//        delete pTicket;
//    }
//    this->m_lAddItemProblems.clear();

//    if (bProblems)
//        this->saveCurrentInteralList();
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

qint32 MeetingInfo::getMeetingInfo(QString &when, QString &where, QString &info)
{
    when = this->m_when;
    where = this->m_where;
    info = this->m_info;
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

//        AvailableTicketInfo* pTicket = (AvailableTicketInfo*)(this->getItemFromArrayIndex(i));
//        if (pTicket == NULL)
//            continue;
        this->m_pConfigSettings->setArrayIndex(i);

//        this->m_pConfigSettings->setValue(ITEM_NAME, pTicket->m_itemName);
//        this->m_pConfigSettings->setValue(ITEM_TIMESTAMP, pTicket->m_timestamp);
//        this->m_pConfigSettings->setValue(ITEM_INDEX, pTicket->m_index);

//        this->m_pConfigSettings->setValue(AVAILABLE_TICKET_ID, pTicket->m_ticketID);
//        this->m_pConfigSettings->setValue(AVAILABLE_USER_ID, pTicket->m_userID);
//        this->m_pConfigSettings->setValue(AVAILABLE_STATE, pTicket->m_state);
    }

    this->m_pConfigSettings->endArray();
    this->m_pConfigSettings->endGroup();

    qDebug().noquote() << QString("saved Meeting List %1 with %2 entries").arg(this->m_pConfigSettings->fileName()).arg(this->getNumberOfInternalList());
}
