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

#ifndef USEREVENTS_H
#define USEREVENTS_H


#include <QtCore/QCryptographicHash>
#include <QtCore/QList>
#include <QtCore/QMutex>
#include <QtCore/QSettings>

#include "configlist.h"

// clang-format off
#define EVENT_USER_ID       "userID"
// clang-format on

struct CompilerTest{

};

class UserEvents : public ConfigList
{
public:
    UserEvents();
    ~UserEvents();

    qint32 initialize(QString type, QString info);
    qint32 initialize(QString filePath);

    bool addNewUser(const quint32 userID);

    bool userGetEvent(const quint32 userID);


private:
    void saveCurrentInteralList() override;

    QList<quint32> m_lUserIDs;
    QString m_type;
    QString m_info;
    qint64  m_timestamp;
};
#endif // USEREVENTS_H
