/*
*	This file is part of StamOrga
*   Copyright (C) 2017 Markus Schneider
*
*	This program is free software; you can redistribute it and/or modify
*   it under the terms of the GNU General Public License as published by
*   the Free Software Foundation; either version 3 of the License, or
*   (at your option) any later version.
*
*	Foobar is distributed in the hope that it will be useful,
*    but WITHOUT ANY WARRANTY; without even the implied warranty of
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*    GNU General Public License for more details.

*    You should have received a copy of the GNU General Public License
*    along with Foobar.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef LISTEDUSER_H
#define LISTEDUSER_H


#include <QtCore/QList>
#include <QtCore/QMutex>
#include <QtCore/QSettings>

#include "configlist.h"

class UserLogin :public ConfigItem
{
public:
    QString password;
    QString readName;
    quint32 properties;
};

#define LOGIN_PASSWORD "password"
#define LOGIN_READNAME "readname"
#define LOGIN_PROPERTIES "properties"

#define USER_INDEX_GROUP    "IndexCount"
#define USER_MAX_COUNT      "CurrentCount"

#define DEFAULT_LOGIN_PROPS 0x0

#define MIN_SIZE_USERNAME 5

class ListedUser : public ConfigList
{
public:
    ListedUser();
    ~ListedUser();

    int addNewUser(const QString& name, const QString& password = "", quint32 props = DEFAULT_LOGIN_PROPS);
    int removeUser(const QString& name);
    int showAllUsers();

    quint32 getNumberOfInternalList() { return this->m_lInteralList.size(); }

    bool userExists(QString name);
    bool userExists(quint32 index);
    bool userCheckPassword(QString name, QString passw);
    bool userChangePassword(QString name, QString passw);
    bool userChangeProperties(QString name, quint32 props);
    bool userChangeReadName(QString name, QString readName);
    quint32 getUserProperties(QString name);
    qint32 getUserLoginIndex(const QString& name);
    QString getReadableName(QString name);

    ConfigItem *getRequestConfigItem(int index) {Q_UNUSED(index) return NULL;}

private:
    QList<UserLogin> m_lInteralList;
    QList<UserLogin> m_lAddItemProblems;

    void saveCurrentInteralList() override;

    bool addNewUserLogin(QString name, qint64 timestamp, quint32 index, QString password, quint32 prop, QString readname, bool checkUser = true);
    void addNewUserLogin(QString name, qint64 timestamp, quint32 index, QString password, quint32 prop, QString readname, QList<UserLogin>* pList);

    bool updateUserLoginValue(UserLogin* pUserLog, QString key, QVariant value);
};

#endif // LISTEDUSER_H
