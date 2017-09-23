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


#ifndef MEETINGINFO_H
#define MEETINGINFO_H

#include <QtCore/QMutex>
#include <QtCore/QSettings>
#include <QtCore/QString>

#include "configlist.h"

struct AcceptMeetingInfo : public ConfigItem {
    quint32 m_state;
    quint32 m_userID;

    AcceptMeetingInfo(QString name, qint64 timestamp,
                      quint32 index, quint32 state,
                      quint32 userID)
    {
        this->m_itemName  = name;
        this->m_timestamp = timestamp;
        this->m_index     = index;

        this->m_state  = state;
        this->m_userID = userID;
    }

    static bool compareAcceptMeetingInfo(ConfigItem* p1, ConfigItem* p2)
    {
        AcceptMeetingInfo* a1 = (AcceptMeetingInfo*)p1;
        AcceptMeetingInfo* a2 = (AcceptMeetingInfo*)p2;
        if (a1->m_state > a2->m_state)
            return false;
        else if (a1->m_state < a2->m_state)
            return true;
        if (a1->m_itemName.compare(a2->m_itemName, Qt::CaseInsensitive) >= 0)
            return false;
        return true;
    }
};

class MeetingInfo : public ConfigList
{

public:
    explicit MeetingInfo();

    qint32 initialize(quint32 year, quint32 competition, quint32 seasonIndex, quint32 index);
    qint32 initialize(QString filePath);

    qint32 addNewAcceptation(const quint32 acceptState, const quint32 userID, QString name = "");
    qint32 changeAcceptation(const quint32 acceptIndex, const quint32 acceptState, const quint32 userID, QString name = "");

    qint32 changeMeetingInfo(const QString when, const QString where, const QString info);
    qint32 getMeetingInfo(QString& when, QString& where, QString& info);

    quint32 getGameIndex() { return this->m_gameIndex; }

    quint16 getAcceptedNumber(const quint32 state);

    //    qint32 getTicketState(quint32 ticketID);
    //    QString getTicketName(quint32 ticketID);


private:
    quint32 m_year;
    quint32 m_competition;
    quint32 m_seasonIndex;
    quint32 m_gameIndex;

    QString m_when;
    QString m_where;
    QString m_info;

    void saveCurrentInteralList() override;

    void sortAcceptations();

    bool updateHeaderValue(QString key, QVariant value);

    bool addNewAcceptInfo(AcceptMeetingInfo* info, bool checkItem = true);
    //    void addNewAcceptInfo(QString name, qint64 timestamp, quint32 index, quint32 state, quint32 userID, QList<ConfigItem*>* pList);
};

#endif // MEETINGINFO_H
