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
    QString configSetFilePath = getUserHomeConfigPath() + "/Settings/SeasonTicket.ini";

    if (!checkFilePathExistAndCreate(configSetFilePath)) {
        CONSOLE_CRITICAL(QString("Could not create File for SeasonTicket setting"));
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
            QString ticketName = this->m_pConfigSettings->value(ITEM_NAME, "").toString();
            qint64  timestamp  = this->m_pConfigSettings->value(ITEM_TIMESTAMP, 0x0).toLongLong();
            quint32 index      = this->m_pConfigSettings->value(ITEM_INDEX, 0).toInt();

            QString user      = this->m_pConfigSettings->value(TICKET_USER, "").toString();
            qint32  userIndex = this->m_pConfigSettings->value(TICKET_USER_INDEX, 0).toInt();
            quint8  discount  = quint8(this->m_pConfigSettings->value(TICKET_DISCOUNT, 0).toUInt());
            QString place     = this->m_pConfigSettings->value(TICKET_PLACE, "").toString();
            qint64  creation  = this->m_pConfigSettings->value(TICKET_CREATE, 0).toLongLong();
            qint64  delts     = this->m_pConfigSettings->value(TICKET_DELETE, 0).toLongLong();


            if (creation == 0) {
                creation  = QDateTime::fromString("10.07.2017", "dd.MM.yyyy").toMSecsSinceEpoch();
                bProblems = true;
            }

            if (delts != 0 && !ticketName.endsWith("DELETED")) {
                ticketName.append("_DELETED");
                bProblems = true;
            }

            TicketInfo* info = new TicketInfo(user, userIndex, ticketName, timestamp, discount, place, index, creation, delts);
            if (!this->addNewTicketInfo(info))
                bProblems = true;
        }
        this->m_pConfigSettings->endArray();
        this->m_pConfigSettings->endGroup();
    }

    if (this->readLastUpdateTime() == 0)
        this->setNewUpdateTime();

    for (int i = 0; i < this->m_lAddItemProblems.size(); i++) {
        bProblems           = true;
        TicketInfo* pTicket = (TicketInfo*)(this->getProblemItemFromArrayIndex(i));
        if (pTicket == NULL)
            continue;
        pTicket->m_index = this->getNextInternalIndex();
        this->addNewTicketInfo(pTicket);
    }
    this->m_lAddItemProblems.clear();

    if (bProblems)
        this->saveCurrentInteralList();
}

int SeasonTicket::addNewSeasonTicket(QString user, qint32 userIndex, QString ticketName, quint8 discount)
{
    if (this->itemExists(ticketName)) {
        qInfo() << QString("SeasonTicket \"%1\" already exists, cannot add").arg(ticketName);
        return ERROR_CODE_ALREADY_EXIST;
    }

    int newIndex = this->getNextInternalIndex();

    qint64 timestamp = QDateTime::currentDateTime().toMSecsSinceEpoch();

    this->m_mConfigIniMutex.lock();

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
    this->m_pConfigSettings->setValue(TICKET_CREATE, timestamp);
    this->m_pConfigSettings->setValue(TICKET_DELETE, 0);

    this->m_pConfigSettings->endArray();
    this->m_pConfigSettings->endGroup();
    this->m_pConfigSettings->sync();

    this->m_mConfigIniMutex.unlock();

    this->setNewUpdateTime();

    TicketInfo* info = new TicketInfo(user, userIndex, ticketName, timestamp, discount, ticketName, newIndex, timestamp);
    this->addNewTicketInfo(info, false);

    qInfo() << (QString("Added new ticket: %1").arg(ticketName));
    return newIndex;
}

int SeasonTicket::changeSeasonTicketInfos(const qint32 index, const qint32 discount, const QString name, const QString place)
{
    QMutexLocker locker(&this->m_mInternalInfoMutex);

    for (int i = 0; i < this->getNumberOfInternalList(); i++) {
        TicketInfo* pTicket = (TicketInfo*)(this->getItemFromArrayIndex(i));
        if (pTicket == NULL)
            continue;
        if (pTicket->m_index == index) {
            bool updated = false;
            if (name != "" && pTicket->m_itemName != name && this->updateItemValue(pTicket, ITEM_NAME, QVariant(name))) {
                pTicket->m_itemName = name;
                updated             = true;
                qInfo().noquote() << (QString("changed name of Ticket %1 to %2").arg(index).arg(name));
            }
            if (place != "" && pTicket->m_place != place && this->updateItemValue(pTicket, TICKET_PLACE, QVariant(place))) {
                pTicket->m_place = place;
                updated          = true;
                qInfo().noquote() << (QString("changed place of Ticket %1 to %2").arg(index).arg(place));
            }
            if (discount >= 0 && pTicket->m_discount != discount && this->updateItemValue(pTicket, TICKET_DISCOUNT, QVariant(quint32(discount)))) {
                pTicket->m_discount = discount;
                updated             = true;
                qInfo().noquote() << (QString("changed name of Ticket %1 to %2").arg(index).arg(name));
            }
            if (updated) {
                qint64 timestamp = QDateTime::currentMSecsSinceEpoch();
                if (this->updateItemValue(pTicket, ITEM_TIMESTAMP, QVariant(timestamp)))
                    pTicket->m_timestamp = timestamp;
            }

            return ERROR_CODE_SUCCESS;
        }
    }

    qWarning() << (QString("Could not find season ticket with \"%1\" to change infos").arg(index));
    return ERROR_CODE_NOT_FOUND;
}

QString SeasonTicket::showAllSeasonTickets()
{
    QString      rValue;
    QMutexLocker locker(&this->m_mInternalInfoMutex);

    qint32 maxSizeName = 0, maxSizePlace = 0;
    for (int i = 0; i < this->getNumberOfInternalList(); i++) {
        TicketInfo* pTicket = (TicketInfo*)(this->getItemFromArrayIndex(i));
        if (pTicket == NULL)
            continue;

        if (pTicket->m_itemName.size() > maxSizeName)
            maxSizeName = pTicket->m_itemName.size();

        if (pTicket->m_place.size() > maxSizePlace)
            maxSizePlace = pTicket->m_place.size();
    }

    for (int i = 0; i < this->getNumberOfInternalList(); i++) {
        TicketInfo* pTicket = (TicketInfo*)(this->getItemFromArrayIndex(i));
        if (pTicket == NULL)
            continue;
        QString date   = QDateTime::fromMSecsSinceEpoch(pTicket->m_timestamp).toString("dd.MM.yyyy hh:mm");
        QString creation = QDateTime::fromMSecsSinceEpoch(pTicket->m_creation).toString("dd.MM.yyyy hh:mm");
        QString deleteTS = QDateTime::fromMSecsSinceEpoch(pTicket->m_deleteTimeStamp).toString("dd.MM.yyyy hh:mm");
        QString output = QString("%1: %2").arg(pTicket->m_itemName, -maxSizeName).arg(pTicket->m_discount);
        output.append(QString(" - %1 - %2").arg(date, creation));
        if (pTicket->m_deleteTimeStamp != 0)
            output.append(QString(" - %1").arg(deleteTS));
        output.append(QString("\n  %1 - %2\n").arg(pTicket->m_place, -maxSizePlace).arg(pTicket->m_user));
        rValue.append(output);
    }
    return rValue;
}

qint32 SeasonTicket::removeTicketItem(const qint32 index)
{
    TicketInfo* pTicket = (TicketInfo*)this->getItem(index);
    if (pTicket == NULL)
        return ERROR_CODE_NOT_FOUND;

    QMutexLocker locker(&this->m_mInternalInfoMutex);

    pTicket->m_deleteTimeStamp = QDateTime::currentMSecsSinceEpoch();
    this->updateItemValue(pTicket, TICKET_DELETE, QVariant(pTicket->m_deleteTimeStamp));

    pTicket->m_itemName = QString("%1_%2").arg(pTicket->m_itemName, "DELETED");
    this->updateItemValue(pTicket, ITEM_NAME, QVariant(pTicket->m_itemName));

    qInfo().noquote() << QString("Removed ticket item \"%1\"").arg(pTicket->m_itemName);
    return ERROR_CODE_SUCCESS;
}

void SeasonTicket::saveCurrentInteralList()
{
    this->m_mConfigIniMutex.lock();

    this->m_pConfigSettings->beginGroup(GROUP_LIST_ITEM);
    this->m_pConfigSettings->remove(""); // clear all elements

    this->m_pConfigSettings->beginWriteArray(CONFIG_LIST_ARRAY);
    for (int i = 0; i < this->getNumberOfInternalList(); i++) {
        TicketInfo* pItem = (TicketInfo*)(this->getItemFromArrayIndex(i));
        if (pItem == NULL)
            continue;
        this->m_pConfigSettings->setArrayIndex(i);

        this->m_pConfigSettings->setValue(ITEM_NAME, pItem->m_itemName);
        this->m_pConfigSettings->setValue(ITEM_TIMESTAMP, pItem->m_timestamp);
        this->m_pConfigSettings->setValue(ITEM_INDEX, pItem->m_index);

        this->m_pConfigSettings->setValue(TICKET_USER, pItem->m_user);
        this->m_pConfigSettings->setValue(TICKET_USER_INDEX, pItem->m_userIndex);
        this->m_pConfigSettings->setValue(TICKET_PLACE, pItem->m_place);
        this->m_pConfigSettings->setValue(TICKET_DISCOUNT, pItem->m_discount);
        this->m_pConfigSettings->setValue(TICKET_CREATE, pItem->m_creation);
        this->m_pConfigSettings->setValue(TICKET_DELETE, pItem->m_deleteTimeStamp);
    }

    this->m_pConfigSettings->endArray();
    this->m_pConfigSettings->endGroup();
    this->m_mConfigIniMutex.unlock();

    this->setNewUpdateTime();

    qDebug().noquote() << QString("saved actual SeasonTicket List with %1 entries").arg(this->getNumberOfInternalList());
}

bool SeasonTicket::addNewTicketInfo(TicketInfo* info, bool checkItem)
{
    if (checkItem) {
        if (info->m_itemName.size() < 4) {
            qWarning().noquote() << QString("Ticket with index %1 has to short name %2 remove it").arg(info->m_index).arg(info->m_itemName);
            return false;
        }
        if (info->m_index == 0 || this->itemExists(info->m_index)) {
            qWarning().noquote() << QString("Ticket \"%1\" with index \"%2\" already exists, saving with new index").arg(info->m_itemName).arg(info->m_index);
            this->addNewConfigItem(info, &this->m_lAddItemProblems);
            return false;
        }
    }

    this->addNewConfigItem(info, &this->m_lInteralList);
    return true;
}

SeasonTicket::~SeasonTicket()
{
    if (this->m_pConfigSettings != NULL)
        delete this->m_pConfigSettings;
}
