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

#ifndef AVAILABLEGAMETICKET_H
#define AVAILABLEGAMETICKET_H


#include <QtCore/QMutex>
#include <QtCore/QSettings>
#include <QtCore/QString>

#include "configlist.h"

struct AvailableTicketInfo : public ConfigItem {
    quint32 m_ticketID;
    quint32 m_userID;
    quint32 m_state;

    AvailableTicketInfo(QString name, qint64 timestamp,
                        quint32 index, quint32 ticketID,
                        quint32 userID, quint32 state)
    {
        this->m_itemName  = name;
        this->m_index     = index;
        this->m_timestamp = timestamp;

        this->m_ticketID = ticketID;
        this->m_userID   = userID;
        this->m_state    = state;
    }
};

class AvailableGameTickets : public ConfigList
{
public:
    AvailableGameTickets();
    ~AvailableGameTickets() {}

    qint32 initialize(quint32 year, quint32 competition, quint32 seasonIndex, quint32 index);
    qint32 initialize(QString filePath);

    qint32 addNewTicket(quint32 ticketID, quint32 userID, quint32 state, QString name = "");
    qint32 changeTicketState(quint32 ticketID, quint32 userID, quint32 state, QString name = "");

    quint32 getGameIndex() { return this->m_gameIndex; }

    quint16 getTicketNumber(const quint32 state);

    qint32 getTicketState(quint32 ticketID);
    QString getTicketName(quint32 ticketID);


private:
    quint32 m_year;
    quint32 m_competition;
    quint32 m_seasonIndex;
    quint32 m_gameIndex;


    void saveCurrentInteralList() override;

    bool addNewAvailableTicket(AvailableTicketInfo* ticket, bool checkItem = true);
};

#endif // AVAILABLEGAMETICKET_H
