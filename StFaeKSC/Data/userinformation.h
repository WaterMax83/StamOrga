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

#ifndef USERINFORMATION_H
#define USERINFORMATION_H

#include <QtCore/QList>
#include <QtCore/QMutex>
#include <QtCore/QSettings>

#include "configlist.h"

class UserStats : public ConfigItem
{
public:
    QString m_readName;
    qint64  m_lastOnline;

    UserStats(QString name, qint64 timestamp,
              quint32 index)
    {
        this->m_itemName  = name;
        this->m_timestamp = timestamp;
        this->m_index     = index;

        this->m_lastOnline = timestamp;
        this->m_readName   = name;
    }
};

class UserInformation : public ConfigList
{

public:
    UserInformation();
    ~UserInformation();

    int addUserInfo(const QString& name, const qint64 timestamp, const qint32 index);

    QString getReadableName(const qint32 userIndex);

    bool userChangeReadName(const qint32 userIndex, const QString& readName);
    bool userChangeOnlineTime(const qint32 userIndex, const qint64 timestamp);

    virtual qint32 checkConsistency() { return -12; }

private:
    void saveCurrentInteralList() override;

    bool addNewUserStats(UserStats* pUser, bool checkItem = true);
};

#endif // USERINFORMATION_H
