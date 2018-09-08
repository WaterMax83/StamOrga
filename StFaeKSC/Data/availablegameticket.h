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
    qint32  m_ticketID;
    qint32  m_userID;
    quint32 m_state;

    AvailableTicketInfo(QString name, qint64 timestamp,
                        qint32 index, qint32 ticketID,
                        qint32 userID, quint32 state)
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
    qint32 changeTicketState(qint32 ticketID, qint32 userID, quint32 state, QString name = "");

    qint32 getGameIndex() { return this->m_gameIndex; }

    quint16 getTicketNumber(const quint32 state);

    qint32 getTicketState(qint32 ticketID);
    QString getTicketName(qint32 ticketID);

    virtual qint32 checkConsistency();


private:
    quint32 m_year;
    quint32 m_competition;
    qint32  m_seasonIndex;
    qint32  m_gameIndex;

    void saveCurrentInteralList() override;

    bool addNewAvailableTicket(AvailableTicketInfo* ticket, bool checkItem = true);

    AvailableTicketInfo* getWrittenTicketInfo(const qint32 arrayIndex);
};

#endif // AVAILABLEGAMETICKET_H
