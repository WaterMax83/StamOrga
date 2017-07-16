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
    quint32 m_userIndex;
    quint8  m_discount;
    QString m_place;
};


#define TICKET_USER "user"
#define TICKET_USER_INDEX "userIndex"
#define TICKET_DISCOUNT "discount"
#define TICKET_PLACE "place"

#define TICKET_INDEX_GROUP "IndexCount"
#define TICKET_MAX_COUNT "CurrentCount"

class SeasonTicket : public ConfigList
{
public:
    SeasonTicket();
    ~SeasonTicket();


    int addNewSeasonTicket(QString user, quint32 userIndex, QString ticketName, quint8 discount);
    int changeSeasonTicketInfos(const quint32 index, const qint32 discount, const QString name, const QString place);
    int showAllSeasonTickets();


private:
    void saveCurrentInteralList() override;

    bool addNewTicketInfo(QString user, quint32 userIndex, QString ticketName, qint64 timestamp, quint8 discount, QString place, quint32 index, bool checkTicket = true);
    void addNewTicketInfo(QString user, quint32 userIndex, QString ticketName, qint64 timestamp, quint8 discount, QString place, quint32 index, QList<ConfigItem*>* pList);
};

#endif // SEASONTICKET_H
