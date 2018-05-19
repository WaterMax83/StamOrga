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

#include <QtCore/QJsonArray>
#include <QtCore/QJsonObject>
#include <QtCore/QList>
#include <QtCore/QMutex>

#include "../Data/availablegameticket.h"
#include "../Data/awaytripinfo.h"
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

    qint32 requestChangeStateSeasonTicket(const qint32 ticketIndex, const qint32 gameIndex,
                                          const qint32 state, const QString reserveName,
                                          const qint32 userID, qint64& messageID);
    qint32 requestBlockSeasonTicket(qint32 ticketIndex, qint32 gameIndex, const QString userName);
    qint32 requestGetAvailableSeasonTicket(const qint32 gameIndex, const QString userName, QByteArray& data);
    qint32 requestGetAvailableTicketFromUser(const qint32 userID, QJsonArray& arrTickets);

    qint32 requestChangeMeetingInfo(const qint32 gameIndex, const qint32 version,
                                    const QString when, const QString where, const QString info,
                                    const qint32 userID, const qint32 type, qint64& messageID);
    qint32 requestGetMeetingInfo(const qint32 gameIndex, const qint32 version, char* pData, const qint32 type, qint32& size);
    qint32 requestAcceptMeetingInfo(const qint32 gameIndex, const qint32 version, const qint32 acceptValue,
                                    const qint32 acceptIndex, const QString name,
                                    const qint32 userID, const qint32 type, qint64& messageID);
    qint32 requestSendCommentMeeting(const qint32 gameIndex, const qint32 type, const qint32 userID,
                                     const QString comment, qint64& messageID);

    quint16 getTicketNumber(const qint32 gamesIndex, const qint32 state);
    quint16 getAcceptedNumber(const qint32 type, const qint32 gamesIndex, const qint32 state);
    quint16 getMeetingInfoValue(const qint32 type, const qint32 gamesIndex);

    qint32 addNewUserEvent(const QString type, const QString info, const qint32 userID);
    qint32 getCurrentUserEvents(QJsonArray& destArray, const qint32 userID);
    qint32 acceptUserEvent(const qint64 eventID, const qint32 userID, const qint32 status);

    ListedUser                   m_UserList;
    Games                        m_GamesList;
    SeasonTicket                 m_SeasonTicket;
    FanclubNews                  m_fanclubNews;
    QList<AvailableGameTickets*> m_availableTickets;
    QList<MeetingInfo*>          m_meetingInfos;
    QList<AwayTripInfo*>         m_awayTripInfos;
    QList<UserEvents*>           m_userEvents;
    QMutex                       m_globalDataMutex;
    qint32                       m_currentSeason;

private:
    bool m_initalized;
};

#endif // GLOBALDATA_H
