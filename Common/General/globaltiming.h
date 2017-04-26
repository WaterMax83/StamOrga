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

#ifndef GLOBALTIMING_H
#define GLOBALTIMING_H


#include <QtCore/QDateTime>

#define MSEC_PER_HOUR   3600000


#define CON_RESET_TIMEOUT_MSEC        (15 * 60 * 1000)
#define CON_LOGIN_TIMEOUT_MSEC        (2 * 60 * 1000)


inline uint CalcMSecEpochToUsHour(quint64 msec64)
{
    uint msec32 = msec64 % MSEC_PER_HOUR;

    return msec32 * 1000;
}

inline quint64 CalcTimeStamp()
{
    return CalcMSecEpochToUsHour(QDateTime::currentDateTime().toMSecsSinceEpoch());
}

#endif // GLOBALTIMING_H
