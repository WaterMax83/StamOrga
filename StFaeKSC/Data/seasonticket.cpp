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

#include "../Common/General/globalfunctions.h"
#include "seasonticket.h"

SeasonTicket::SeasonTicket()
{
    QString ticketSetFilePath = getUserHomeConfigPath() + "/Settings/SeasonTicket.ini";

    if (!checkFilePathExistAndCreate(ticketSetFilePath)) {
        CONSOLE_CRITICAL(QString("Could not create File for SeasonTicket setting"));
        return;
    }

    this->m_pConfigSettings = new QSettings(ticketSetFilePath, QSettings::IniFormat);

    /* Check wheter we have to save data after reading again */
    bool bProblems = false;
    {
        QMutexLocker locker(&this->m_mConfigIniMutex);

        this->m_pConfigSettings->beginGroup(GROUP_LIST_ITEM);
        int sizeOfTicket = this->m_pConfigSettings->beginReadArray(CONFIG_LIST_ARRAY);

        for (int i = 0; i < sizeOfTicket; i++) {
            this->m_pConfigSettings->setArrayIndex(i);
            QString ticketName = this->m_pConfigSettings->value(ITEM_NAME, "").toString();
            qint64  timestamp  = this->m_pConfigSettings->value(ITEM_TIMESTAMP, 0x0).toULongLong();
            quint32 index      = this->m_pConfigSettings->value(ITEM_INDEX, 0).toInt();

            QString user      = this->m_pConfigSettings->value(TICKET_USER, "").toString();
            quint32 userIndex = this->m_pConfigSettings->value(TICKET_USER_INDEX, 0).toUInt();
            quint8  discount  = quint8(this->m_pConfigSettings->value(TICKET_DISCOUNT, 0).toUInt());
            QString place     = this->m_pConfigSettings->value(TICKET_PLACE, "").toString();

            if (!this->addNewTicketInfo(user, userIndex, ticketName, timestamp, discount, place, index))
                bProblems = true;
        }
        this->m_pConfigSettings->endArray();
        this->m_pConfigSettings->endGroup();
    }


    for (int i = 0; i < this->m_lAddItemProblems.size(); i++) {
        bProblems           = true;
        TicketInfo* pTicket = (TicketInfo*)(this->getProblemItemFromArrayIndex(i));
        if (pTicket == NULL)
            continue;
        pTicket->m_index = this->getNextInternalIndex();
        this->addNewTicketInfo(pTicket->user, pTicket->userIndex,
                               pTicket->m_itemName,
                               pTicket->m_timestamp, pTicket->discount,
                               pTicket->place, pTicket->m_index);

        delete pTicket;
    }
    this->m_lAddItemProblems.clear();

    if (bProblems)
        this->saveCurrentInteralList();
}

int SeasonTicket::addNewSeasonTicket(QString user, quint32 userIndex, QString ticketName, quint8 discount)
{
    if (this->itemExists(ticketName)) {
        qInfo() << QString("SeasonTicket \"%1\" already exists, cannot add").arg(ticketName);
        return ERROR_CODE_ALREADY_EXIST;
    }

    int newIndex = this->getNextInternalIndex();

    QMutexLocker locker(&this->m_mConfigIniMutex);

    qint64 timestamp = QDateTime::currentDateTime().toMSecsSinceEpoch();


    this->m_pConfigSettings->beginGroup(GROUP_LIST_ITEM);
    this->m_pConfigSettings->beginWriteArray(CONFIG_LIST_ARRAY);

    this->m_pConfigSettings->setArrayIndex(this->getNumberOfInternalList());

    this->m_pConfigSettings->setValue(ITEM_NAME, ticketName);
    this->m_pConfigSettings->setValue(ITEM_TIMESTAMP, timestamp);
    this->m_pConfigSettings->setValue(ITEM_INDEX, newIndex);

    this->m_pConfigSettings->setValue(TICKET_USER, user);
    this->m_pConfigSettings->setValue(TICKET_USER_INDEX, userIndex);
    this->m_pConfigSettings->setValue(TICKET_DISCOUNT, discount);
    this->m_pConfigSettings->setValue(TICKET_PLACE, ticketName);

    this->m_pConfigSettings->endArray();
    this->m_pConfigSettings->endGroup();
    this->m_pConfigSettings->sync();

    this->addNewTicketInfo(user, userIndex, ticketName, timestamp, discount, ticketName, newIndex, false);

    //    qInfo() << (QString("Added new ticket: %1").arg(ticketName));
    return newIndex;
}

int SeasonTicket::changePlaceFromTicket(const quint32 index, QString newPlace)
{
    QMutexLocker locker(&this->m_mInternalInfoMutex);

    for (int i = 0; i < this->getNumberOfInternalList(); i++) {
        TicketInfo* pTicket = (TicketInfo*)(this->getItemFromArrayIndex(i));
        if (pTicket == NULL)
            continue;
        if (pTicket->m_index == index) {
            QString ticketName = pTicket->m_itemName;
            if (this->updateItemValue(pTicket, TICKET_PLACE, QVariant(newPlace))) {
                pTicket->place = newPlace;
                qInfo().noquote() << (QString("changed Place of Ticket %1 to %2").arg(ticketName, newPlace));
                return ERROR_CODE_SUCCESS;
            }
        }
    }

    qWarning() << (QString("Could not find season ticket with \"%1\" to change place").arg(index));
    return ERROR_CODE_NOT_FOUND;
}

int SeasonTicket::showAllSeasonTickets()
{
    QMutexLocker locker(&this->m_mInternalInfoMutex);

    for (int i = 0; i < this->getNumberOfInternalList(); i++) {
        TicketInfo* pTicket = (TicketInfo*)(this->getItemFromArrayIndex(i));
        if (pTicket == NULL)
            continue;
        QString date = QDateTime::fromMSecsSinceEpoch(pTicket->m_timestamp).toString("dd.MM.yyyy hh:mm");
        QString output;
        output = QString("%1: %2 - %3 - %4 - %5").arg(pTicket->m_itemName).arg(pTicket->discount).arg(pTicket->place, date, pTicket->user);
        std::cout << output.toStdString() << std::endl;
    }
    return 0;
}

void SeasonTicket::saveCurrentInteralList()
{
    QMutexLocker locker(&this->m_mConfigIniMutex);

    this->m_pConfigSettings->beginGroup(GROUP_LIST_ITEM);
    this->m_pConfigSettings->remove(""); // clear all elements

    this->m_pConfigSettings->beginWriteArray(CONFIG_LIST_ARRAY);
    for (int i = 0; i < this->getNumberOfInternalList(); i++) {
        TicketInfo* pTicket = (TicketInfo*)(this->getItemFromArrayIndex(i));
        if (pTicket == NULL)
            continue;
        this->m_pConfigSettings->setArrayIndex(i);

        this->m_pConfigSettings->setValue(ITEM_NAME, pTicket->m_itemName);
        this->m_pConfigSettings->setValue(ITEM_TIMESTAMP, pTicket->m_timestamp);
        this->m_pConfigSettings->setValue(ITEM_INDEX, pTicket->m_index);

        this->m_pConfigSettings->setValue(TICKET_USER, pTicket->user);
        this->m_pConfigSettings->setValue(TICKET_USER_INDEX, pTicket->userIndex);
        this->m_pConfigSettings->setValue(TICKET_PLACE, pTicket->place);
        this->m_pConfigSettings->setValue(TICKET_DISCOUNT, pTicket->discount);
    }

    this->m_pConfigSettings->endArray();
    this->m_pConfigSettings->endGroup();

    qDebug().noquote() << QString("saved actual SeasonTicket List with %1 entries").arg(this->getNumberOfInternalList());
}

bool SeasonTicket::addNewTicketInfo(QString user, quint32 userIndex, QString ticketName, qint64 datetime, quint8 discount, QString place, quint32 index, bool checkTicket)
{
    if (checkTicket) {
        if (ticketName.size() < 5) {
            qWarning().noquote() << QString("Ticket with index %1 has to short name %2 remove it").arg(index).arg(ticketName);
            return false;
        }
        if (index == 0 || this->itemExists(index)) {
            qWarning().noquote() << QString("Ticket \"%1\" with index \"%2\" already exists, saving with new index").arg(ticketName).arg(index);
            this->addNewTicketInfo(user, userIndex, ticketName, datetime, discount, place, index, &this->m_lAddItemProblems);
            return false;
        }
    }

    this->addNewTicketInfo(user, userIndex, ticketName, datetime, discount, place, index, &this->m_lInteralList);
    return true;
}

void SeasonTicket::addNewTicketInfo(QString user, quint32 userIndex, QString ticketName, qint64 datetime, quint8 discount, QString place, quint32 index, QList<ConfigItem*>* pList)
{
    QMutexLocker locker(&this->m_mInternalInfoMutex);

    TicketInfo* ticket  = new TicketInfo;
    ticket->m_itemName  = ticketName;
    ticket->m_timestamp = datetime;
    ticket->m_index     = index;

    ticket->user      = user;
    ticket->userIndex = userIndex;
    ticket->discount  = discount;
    ticket->place     = place;

    pList->append(ticket);
}


SeasonTicket::~SeasonTicket()
{
    if (this->m_pConfigSettings != NULL)
        delete this->m_pConfigSettings;
}
