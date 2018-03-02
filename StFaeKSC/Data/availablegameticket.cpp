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
#include "availablegameticket.h"


#define AVAILABLE_TICKET_ID "ticketID"
#define AVAILABLE_USER_ID "userID"
#define AVAILABLE_STATE "state"

AvailableGameTickets::AvailableGameTickets()
{
}

qint32 AvailableGameTickets::initialize(quint32 year, quint32 competition, quint32 seasonIndex, quint32 index)
{
    this->m_year        = year;
    this->m_competition = competition;
    this->m_seasonIndex = seasonIndex;
    this->m_gameIndex   = index;

    QString configSetFilePath = getUserHomeConfigPath() + "/Settings/AvailableTickets/";
    configSetFilePath.append(QString("Tickets_Game_%1.ini").arg(index));

    if (!checkFilePathExistAndCreate(configSetFilePath)) {
        CONSOLE_CRITICAL(QString("Could not create File for UserSettings"));
        return ERROR_CODE_COMMON;
    }

    this->m_pConfigSettings = new QSettings(configSetFilePath, QSettings::IniFormat);
    this->m_pConfigSettings->setIniCodec(("UTF-8"));

    this->m_pConfigSettings->beginGroup("TicketHeader");

    this->m_pConfigSettings->setValue("year", this->m_year);
    this->m_pConfigSettings->setValue("competition", this->m_competition);
    this->m_pConfigSettings->setValue("seasonIndex", this->m_seasonIndex);
    this->m_pConfigSettings->setValue("gameIndex", this->m_gameIndex);

    this->m_pConfigSettings->endGroup();

    return ERROR_CODE_SUCCESS;
}

qint32 AvailableGameTickets::initialize(QString filePath)
{
    this->m_pConfigSettings = new QSettings(filePath, QSettings::IniFormat);
    this->m_pConfigSettings->setIniCodec(("UTF-8"));

    this->m_pConfigSettings->beginGroup("TicketHeader");

    this->m_year        = this->m_pConfigSettings->value("year", 0).toUInt();
    this->m_competition = this->m_pConfigSettings->value("competition", 0).toUInt();
    this->m_seasonIndex = this->m_pConfigSettings->value("seasonIndex", 0).toUInt();
    this->m_gameIndex   = this->m_pConfigSettings->value("gameIndex", 0).toUInt();

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
            QString name      = this->m_pConfigSettings->value(ITEM_NAME, "").toString();
            quint32 index     = this->m_pConfigSettings->value(ITEM_INDEX, 0).toUInt();
            qint64  timestamp = this->m_pConfigSettings->value(ITEM_TIMESTAMP, 0x0).toULongLong();

            quint32 ticketID = this->m_pConfigSettings->value(AVAILABLE_TICKET_ID, 0x0).toUInt();
            quint32 userID   = this->m_pConfigSettings->value(AVAILABLE_USER_ID, 0x0).toUInt();
            quint32 state    = this->m_pConfigSettings->value(AVAILABLE_STATE, 0).toUInt();

            if (ticketID == 0 || userID == 0 || state == 0)
                continue;

            AvailableTicketInfo* ticket = new AvailableTicketInfo(name, timestamp, index, ticketID, userID, state);
            if (!this->addNewAvailableTicket(ticket))
                bProblems = true;
        }
        this->m_pConfigSettings->endArray();
        this->m_pConfigSettings->endGroup();
    }


    for (int i = 0; i < this->m_lAddItemProblems.size(); i++) {
        bProblems                    = true;
        AvailableTicketInfo* pTicket = (AvailableTicketInfo*)(this->getProblemItemFromArrayIndex(i));
        if (pTicket == NULL)
            continue;
        pTicket->m_index = this->getNextInternalIndex();
        this->addNewAvailableTicket(pTicket);
    }
    this->m_lAddItemProblems.clear();

    if (bProblems)
        this->saveCurrentInteralList();
    return ERROR_CODE_SUCCESS;
}

qint32 AvailableGameTickets::addNewTicket(quint32 ticketID, quint32 userID, quint32 state, QString name)
{
    if (state == TICKET_STATE_BLOCKED) {
        qWarning().noquote() << QString("Could not add available ticket \"%1\", state 0 is not allowed ").arg(name);
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

    this->m_pConfigSettings->setValue(AVAILABLE_TICKET_ID, ticketID);
    this->m_pConfigSettings->setValue(AVAILABLE_USER_ID, userID);
    this->m_pConfigSettings->setValue(AVAILABLE_STATE, state);

    this->m_pConfigSettings->endArray();
    this->m_pConfigSettings->endGroup();
    this->m_pConfigSettings->sync();

    AvailableTicketInfo* ticket = new AvailableTicketInfo(name, timestamp, newIndex, ticketID, userID, state);
    this->addNewAvailableTicket(ticket, false);

    return ERROR_CODE_SUCCESS;
}

qint32 AvailableGameTickets::changeTicketState(qint32 ticketID, qint32 userID, quint32 state, QString name)
{
    //    if (state == TICKET_STATE_BLOCKED) {
    //        qWarning().noquote() << QString("Could not change available ticket, state 0 is not allowed ").arg(state);
    //        return ERROR_CODE_COMMON;
    //    }
    QMutexLocker locker(&this->m_mInternalInfoMutex);

    qint64 timestamp = QDateTime::currentDateTime().toMSecsSinceEpoch();

    for (int i = 0; i < this->getNumberOfInternalList(); i++) {
        AvailableTicketInfo* pTicket = (AvailableTicketInfo*)(this->getItemFromArrayIndex(i));
        if (pTicket->m_ticketID == ticketID) {
            if (pTicket->m_userID != userID) {
                pTicket->m_userID = userID;
                this->updateItemValue(pTicket, AVAILABLE_USER_ID, QVariant(userID));
            }
            if (pTicket->m_state != state) {
                pTicket->m_state = state;
                this->updateItemValue(pTicket, AVAILABLE_STATE, QVariant(state));
            }
            if (pTicket->m_itemName != name) {
                pTicket->m_itemName = name;
                this->updateItemValue(pTicket, ITEM_NAME, QVariant(name));
            }
            if (pTicket->m_timestamp != timestamp) {
                pTicket->m_timestamp = timestamp;
                this->updateItemValue(pTicket, ITEM_TIMESTAMP, QVariant(timestamp));
            }

            return ERROR_CODE_SUCCESS;
        }
    }
    return ERROR_CODE_NOT_FOUND;
}

qint32 AvailableGameTickets::getTicketState(qint32 ticketID)
{
    QMutexLocker locker(&this->m_mInternalInfoMutex);

    for (int i = 0; i < this->getNumberOfInternalList(); i++) {
        AvailableTicketInfo* pTicket = (AvailableTicketInfo*)(this->getItemFromArrayIndex(i));
        if (pTicket->m_ticketID == ticketID)
            return pTicket->m_state;
    }
    return TICKET_STATE_NOT_POSSIBLE;
}

QString AvailableGameTickets::getTicketName(qint32 ticketID)
{
    QMutexLocker locker(&this->m_mInternalInfoMutex);

    for (int i = 0; i < this->getNumberOfInternalList(); i++) {
        AvailableTicketInfo* pTicket = (AvailableTicketInfo*)(this->getItemFromArrayIndex(i));
        if (pTicket->m_ticketID == ticketID)
            return pTicket->m_itemName;
    }
    return "";
}

quint16 AvailableGameTickets::getTicketNumber(const quint32 state)
{
    QMutexLocker locker(&this->m_mInternalInfoMutex);

    quint16 rValue = 0;
    for (int i = 0; i < this->getNumberOfInternalList(); i++) {
        AvailableTicketInfo* pTicket = (AvailableTicketInfo*)(this->getItemFromArrayIndex(i));
        if (pTicket->m_state == state)
            rValue++;
    }
    return rValue;
}


void AvailableGameTickets::saveCurrentInteralList()
{
    QMutexLocker locker(&this->m_mConfigIniMutex);

    this->m_pConfigSettings->beginGroup(GROUP_LIST_ITEM);
    this->m_pConfigSettings->remove(""); // clear all elements

    this->m_pConfigSettings->beginWriteArray(CONFIG_LIST_ARRAY);
    for (int i = 0; i < this->getNumberOfInternalList(); i++) {

        AvailableTicketInfo* pItem = (AvailableTicketInfo*)(this->getItemFromArrayIndex(i));
        if (pItem == NULL)
            continue;
        this->m_pConfigSettings->setArrayIndex(i);

        this->m_pConfigSettings->setValue(ITEM_NAME, pItem->m_itemName);
        this->m_pConfigSettings->setValue(ITEM_TIMESTAMP, pItem->m_timestamp);
        this->m_pConfigSettings->setValue(ITEM_INDEX, pItem->m_index);

        this->m_pConfigSettings->setValue(AVAILABLE_TICKET_ID, pItem->m_ticketID);
        this->m_pConfigSettings->setValue(AVAILABLE_USER_ID, pItem->m_userID);
        this->m_pConfigSettings->setValue(AVAILABLE_STATE, pItem->m_state);
    }

    this->m_pConfigSettings->endArray();
    this->m_pConfigSettings->endGroup();

    qDebug().noquote() << QString("saved Ticket List %1 with %2 entries").arg(this->m_pConfigSettings->fileName()).arg(this->getNumberOfInternalList());
}


bool AvailableGameTickets::addNewAvailableTicket(AvailableTicketInfo* ticket, bool checkItem)
{
    if (checkItem) {
        if (ticket->m_index == 0 || itemExists(ticket->m_index)) {
            qWarning().noquote() << QString("Available ticket with index \"%1\" already exists, saving with new index").arg(ticket->m_index);
            this->addNewConfigItem(ticket, &this->m_lAddItemProblems);
            return false;
        }
    }

    this->addNewConfigItem(ticket, &this->m_lInteralList);
    return true;
}

qint32 AvailableGameTickets::checkConsistency()
{
    this->m_mInternalInfoMutex.lock();

    for (int i = 0; i < this->m_lInteralList.size(); i++) {
        AvailableTicketInfo* pWrittenTicket = this->getWrittenTicketInfo(i);
        if (pWrittenTicket == NULL)
            continue;

        this->m_mInternalInfoMutex.unlock();
        AvailableTicketInfo* pTicketInfo = (AvailableTicketInfo*)this->getItem(pWrittenTicket->m_index);
        this->m_mInternalInfoMutex.lock();
        if (pTicketInfo == NULL) {
            delete pWrittenTicket;
            continue;
        }

        if (pTicketInfo->m_userID != pWrittenTicket->m_userID) {
            this->updateItemValue(pTicketInfo, AVAILABLE_USER_ID, QVariant(pTicketInfo->m_userID), this->m_lastUpdateTimeStamp);
            qInfo().noquote() << QString("Fix consistency of userID at game %1 to %2").arg(this->m_gameIndex).arg(pTicketInfo->m_userID);
        }
        if (pTicketInfo->m_state != pWrittenTicket->m_state) {
            this->updateItemValue(pTicketInfo, AVAILABLE_STATE, QVariant(pTicketInfo->m_state), this->m_lastUpdateTimeStamp);
            qInfo().noquote() << QString("Fix consistency of state at game %1 to %2").arg(this->m_gameIndex).arg(pTicketInfo->m_state);
        }
        if (pTicketInfo->m_itemName != pWrittenTicket->m_itemName) {
            this->updateItemValue(pTicketInfo, ITEM_NAME, QVariant(pTicketInfo->m_itemName), this->m_lastUpdateTimeStamp);
            qInfo().noquote() << QString("Fix consistency of name at game %1 to %2").arg(this->m_gameIndex).arg(pTicketInfo->m_itemName);
        }
        if (pTicketInfo->m_timestamp != pWrittenTicket->m_timestamp) {
            this->updateItemValue(pTicketInfo, ITEM_TIMESTAMP, QVariant(pTicketInfo->m_timestamp), this->m_lastUpdateTimeStamp);
            qInfo().noquote() << QString("Fix consistency of timestamp at game %1 to %2").arg(this->m_gameIndex).arg(pTicketInfo->m_timestamp);
        }
        delete pWrittenTicket;
    }

    this->m_mInternalInfoMutex.unlock();

    return ERROR_CODE_SUCCESS;
}

AvailableTicketInfo* AvailableGameTickets::getWrittenTicketInfo(const qint32 arrayIndex)
{
    QMutexLocker lock(&this->m_mConfigIniMutex);

    this->m_pConfigSettings->beginGroup(GROUP_LIST_ITEM);
    int sizeOfArray = this->m_pConfigSettings->beginReadArray(CONFIG_LIST_ARRAY);
    if (arrayIndex >= sizeOfArray)
        return NULL;

    this->m_pConfigSettings->setArrayIndex(arrayIndex);
    QString name      = this->m_pConfigSettings->value(ITEM_NAME, "").toString();
    quint32 index     = this->m_pConfigSettings->value(ITEM_INDEX, 0).toUInt();
    qint64  timestamp = this->m_pConfigSettings->value(ITEM_TIMESTAMP, 0x0).toULongLong();

    quint32 ticketID = this->m_pConfigSettings->value(AVAILABLE_TICKET_ID, 0x0).toUInt();
    quint32 userID   = this->m_pConfigSettings->value(AVAILABLE_USER_ID, 0x0).toUInt();
    quint32 state    = this->m_pConfigSettings->value(AVAILABLE_STATE, 0).toUInt();

    this->m_pConfigSettings->endArray();
    this->m_pConfigSettings->endGroup();

    if (ticketID == 0 || userID == 0 || state == 0)
        return NULL;

    return new AvailableTicketInfo(name, timestamp, index, ticketID, userID, state);
}
