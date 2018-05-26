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

#ifndef SEASONTICKET_H
#define SEASONTICKET_H


#include <QtCore/QMutex>
#include <QtCore/QSettings>
#include <QtCore/QString>

#include "configlist.h"

struct TicketInfo : public ConfigItem {
    QString m_user;
    qint32  m_userIndex;
    quint8  m_discount;
    QString m_place;
    qint64  m_creation;
    qint64  m_deleteTimeStamp;

    TicketInfo(QString user, qint32 userIndex,
               QString ticketName, qint64 timestamp,
               quint8 discount, QString place,
               quint32 index, qint64 creation,
               qint64 deleteTimeStamp = 0)
    {
        this->m_itemName  = ticketName;
        this->m_timestamp = timestamp;
        this->m_index     = index;

        this->m_user            = user;
        this->m_userIndex       = userIndex;
        this->m_discount        = discount;
        this->m_place           = place;
        this->m_creation        = creation;
        this->m_deleteTimeStamp = deleteTimeStamp;
    }

    bool isTicketRemoved() { return this->m_deleteTimeStamp != 0 ? true : false; }
};


#define TICKET_USER "user"
#define TICKET_USER_INDEX "userIndex"
#define TICKET_DISCOUNT "discount"
#define TICKET_PLACE "place"
#define TICKET_CREATE "creation"
#define TICKET_DELETE "delete"

#define TICKET_INDEX_GROUP "IndexCount"
#define TICKET_MAX_COUNT "CurrentCount"

class SeasonTicket : public ConfigList
{
public:
    SeasonTicket();
    ~SeasonTicket();


    int addNewSeasonTicket(QString user, qint32 userIndex, QString ticketName, quint8 discount);
    int changeSeasonTicketInfos(const qint32 index, const qint32 discount, const QString name, const QString place);
    QString showAllSeasonTickets();

    int removeTicketItem(qint32 index);

    virtual qint32 checkConsistency() { return -12; }


private:
    void saveCurrentInteralList() override;

    bool addNewTicketInfo(TicketInfo* info, bool checkItem = true);
};

#endif // SEASONTICKET_H
