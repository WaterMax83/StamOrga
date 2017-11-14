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

#ifndef GLOBALDATA_H
#define GLOBALDATA_H

#include <QtCore/QList>
#include <QtCore/QMutex>

#include "../Data/availablegameticket.h"
#include "../Data/fanclubnews.h"
#include "../Data/games.h"
#include "../Data/listeduser.h"
#include "../Data/meetinginfo.h"
#include "../Data/seasonticket.h"
#include "../Data/userevents.h"

class GlobalData
{
public:
    GlobalData();

    void initialize();

    qint32 requestChangeStateSeasonTicket(const quint32 ticketIndex, const quint32 gameIndex,
                                          const quint32 state, const QString reserveName,
                                          const qint32 userID, qint64& messageID);
    qint32 requestBlockSeasonTicket(quint32 ticketIndex, quint32 gameIndex, const QString userName);
    qint32 requestGetAvailableSeasonTicket(const quint32 gameIndex, const QString userName, QByteArray& data);

    qint32 requestChangeMeetingInfo(const quint32 gameIndex, const quint32 version,
                                    const QString when, const QString where, const QString info,
                                    const qint32 userID, qint64& messageID);
    qint32 requestGetMeetingInfo(const quint32 gameIndex, const quint32 version, char* pData, quint32& size);
    qint32 requestAcceptMeetingInfo(const quint32 gameIndex, const quint32 version, const quint32 acceptValue,
                                    const quint32 acceptIndex, const QString name,
                                    const qint32 userID, qint64& messageID);

    quint16 getTicketNumber(const quint32 gamesIndex, const quint32 state);
    quint16 getAcceptedNumber(const quint32 gamesIndex, const quint32 state);
    quint16 getMeetingInfoValue(const quint32 gamesIndex);

    qint32 addNewUserEvent(QString type, QString info, qint32 userID);
    qint32 getCurrentUserEvents(QByteArray& destArray, qint32 userID);

    ListedUser                   m_UserList;
    Games                        m_GamesList;
    SeasonTicket                 m_SeasonTicket;
    FanclubNews                  m_fanclubNews;
    QList<AvailableGameTickets*> m_availableTickets;
    QList<MeetingInfo*>          m_meetingInfos;
    QList<UserEvents*>           m_userEvents;
    QMutex                       m_globalDataMutex;
    bool                         m_initalized;
};

#endif // GLOBALDATA_H
