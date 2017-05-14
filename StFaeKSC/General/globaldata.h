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

#include "../Data/availablegameticket.h"
#include "../Data/games.h"
#include "../Data/listeduser.h"
#include "../Data/seasonticket.h"

class GlobalData
{
public:
    GlobalData();

    qint32 requestFreeSeasonTicket(quint32 ticketIndex, quint32 gameIndex, const QString userName);
    qint32 requestGetAvailableSeasonTicket(const quint32 gameIndex, const QString userName, QByteArray& data);

    ListedUser                   m_UserList;
    Games                        m_GamesList;
    SeasonTicket                 m_SeasonTicket;
    QList<AvailableGameTickets*> m_availableTickets;
};

#endif // GLOBALDATA_H
