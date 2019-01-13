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

#ifndef CDATAUSERMANAGER_H
#define CDATAUSERMANAGER_H

#include <QObject>
#include <QtCore/QDateTime>
#include <QtCore/QMutex>

#include "../../Common/General/globalfunctions.h"
#include "../Common/General/cgendisposer.h"
#include "../Common/Network/messageprotocol.h"


class UserInformation : public QObject
{
    Q_OBJECT
    Q_PROPERTY(qint32 index READ index)
    Q_PROPERTY(QString user READ user)
    Q_PROPERTY(QString userType READ userType)
    Q_PROPERTY(QString readName READ readName)
    Q_PROPERTY(QString onlineTime READ onlineTime)
    Q_PROPERTY(quint32 prop READ prop)
    Q_PROPERTY(qint64 timestamp READ timestamp)
    Q_PROPERTY(bool owner READ owner)
    Q_PROPERTY(bool admin READ admin)
public:
    qint32 m_index;
    qint32 index() { return this->m_index; }

    QString m_user;
    QString user() { return this->m_user; }

    QString m_userType;
    QString userType() { return this->m_userType; }

    QString m_readName;
    QString readName() { return this->m_readName; }

    qint64  m_lastOnline;
    QString onlineTime()
    {
        QDateTime time = QDateTime::fromMSecsSinceEpoch(this->m_lastOnline);
        return getTimeStampSinceString(time);
    }

    quint32 m_prop;
    quint32 prop() { return this->m_prop; }

    qint64 m_timestamp;
    qint64 timestamp() { return this->m_timestamp; }

    bool m_owner;
    bool owner() { return this->m_owner; }

    bool m_admin;
    bool admin() { return this->m_admin; }

    static bool compareTimeStampFunctionDescending(UserInformation* p1, UserInformation* p2)
    {
        if (p1->m_timestamp < p2->m_timestamp)
            return false;
        return true;
    }

    static bool compareTimeStampFunctionAscending(UserInformation* p1, UserInformation* p2)
    {
        if (p1->m_timestamp > p2->m_timestamp)
            return false;
        return true;
    }

    Q_INVOKABLE QString getUserShortString()
    {
        if (this->m_owner)
            return "Du";
        return this->m_readName.left(1);
    }
};

class cDataUserManager : public cGenDisposer
{
    Q_OBJECT
public:
    explicit cDataUserManager(QObject* parent = nullptr);

    qint32 initialize() override;

    Q_INVOKABLE qint32 getUserInformationLength();
    Q_INVOKABLE UserInformation* getUserInfoFromArrayIndex(int index);
    //    Q_INVOKABLE QString getUserInfoLastLocalUpdateString();

    Q_INVOKABLE qint32 startUpdateReadableName(QString name);
    Q_INVOKABLE qint32 startUpdatePassword(QString password);
    Q_INVOKABLE qint32 startListUserOverview();

    qint32 handleUserCommandResponse(MessageProtocol* msg);
    qint32 handleUpdatePasswordResponse(MessageProtocol* msg);

signals:

public slots:

private:
    QMutex m_mutex;

    QString m_newReadableName;
    QString m_currentRandomValue;
    QString m_newPassWord;

    QList<UserInformation*> m_lUser;
};

extern cDataUserManager* g_DataUserManager;

#endif // CDATAUSERMANAGER_H
