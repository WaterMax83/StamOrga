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

#ifndef LISTEDUSER_H
#define LISTEDUSER_H


#include <QtCore/QCryptographicHash>
#include <QtCore/QList>
#include <QtCore/QMutex>
#include <QtCore/QSettings>

#include "configlist.h"
#include "userinformation.h"

class UserLogin : public ConfigItem
{
public:
    QString m_password;
    QString m_salt;
    quint32 m_properties;

    UserLogin(QString name, qint64 timestamp,
              quint32 index, QString password,
              QString salt, quint32 prop)
    {
        this->m_itemName  = name;
        this->m_timestamp = timestamp;
        this->m_index     = index;

        this->m_password   = password;
        this->m_salt       = salt;
        this->m_properties = prop;
    }
};

// clang-format off
#define LOGIN_PASSWORD      "password"
#define LOGIN_SALT          "salt"
#define LOGIN_READNAME      "readname"
#define LOGIN_PROPERTIES    "properties"

#define USER_INDEX_GROUP    "IndexCount"
#define USER_MAX_COUNT      "CurrentCount"

#define DEFAULT_LOGIN_PROPS 0x0

#define MIN_SIZE_USERNAME   5
// clang-format on

class ListedUser : public ConfigList
{
public:
    ListedUser();
    ~ListedUser();

    int addNewUser(const QString& name, const QString& password = "", quint32 props = DEFAULT_LOGIN_PROPS);
    QString showAllUsers();

    bool userCheckPassword(const QString& name, const QString& passw);
    qint32 userCheckPasswordHash(const QString& name, const QString& hash, const QString& random);
    bool userChangePassword(const QString& name, const QString& passw);
    bool userChangePasswordHash(const QString& name, const QString& passw);
    bool userChangeProperties(const QString& name, quint32 props);
    bool userChangeReadName(const qint32 userIndex, const QString& readName);
    quint32 getUserProperties(const qint32 userIndex);
    QString getReadableName(const qint32 userIndex);
    QString getSalt(const QString& name);

    ConfigItem* getRequestConfigItemFromListIndex(int index) override;

    virtual qint32 checkConsistency() { return -12; }

private:
    void saveCurrentInteralList() override;

    bool addNewUserLogin(UserLogin* login, bool checkItem = true);

    QString createHashPassword(const QString passWord, const QString salt);
    QCryptographicHash* m_hash;

    UserInformation* m_pUserInfo;
};
#endif // LISTEDUSER_H
