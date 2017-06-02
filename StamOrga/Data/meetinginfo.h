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

#include <QtCore/QObject>

class MeetingInfo : public QObject
{
    Q_OBJECT
public:
    explicit MeetingInfo(QObject* parent = 0);

    Q_INVOKABLE QString when() { return this->m_when; }
    Q_INVOKABLE QString where() { return this->m_where; }
    Q_INVOKABLE QString info() { return this->m_info; }

private:
    QString m_when;
    QString m_where;
    QString m_info;
};

#endif // MEETINGINFO_H
