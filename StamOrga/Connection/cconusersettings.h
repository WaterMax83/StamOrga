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

#ifndef CCONSETTINGS_H
#define CCONSETTINGS_H

#include <QObject>
#include <QtCore/QCryptographicHash>

#include "../Common/General/cgendisposer.h"
#include "../Common/Network/messageprotocol.h"

class cConUserSettings : public cGenDisposer
{
    Q_OBJECT
public:
    explicit cConUserSettings(QObject* parent = nullptr);
    ~cConUserSettings();

    qint32 initialize() override;

    Q_INVOKABLE QString getIPAddr();
    void                setIPAddr(const QString ipAddr);

    Q_INVOKABLE QString getPassWord();
    void                setPassWord(const QString password);

    Q_INVOKABLE QString getUserName();
    void                setUserName(const QString name);

    Q_INVOKABLE QString getReadableName();
    void                setReadableName(const QString name);

    Q_INVOKABLE qint32 getEmailNotification();
    void               setEmailNotification(const qint32 notify);
    Q_INVOKABLE QString getEmailNotifyString();

    QString getSalt();
    void    setSalt(const QString salt);

    qint32  getUserIndex();
    quint32 getUserProperties();
    void    resetUserProperties();

    qint32 getMasterConPort() { return 55000; }

    Q_INVOKABLE qint32 startGettingUserProps(const bool loadEverything = false);
    qint32             handleUserPropsResponse(MessageProtocol* msg);

    void    setRandomLoginValue(const QString random) { this->m_currentRandomValue = random; }
    QString getRandomLoginValue() { return this->m_currentRandomValue; }

    QString createHashValue(const QString first, const QString second);

    Q_INVOKABLE bool userIsLoggingEnabled();
    Q_INVOKABLE bool userIsGameAddingEnabled();
    Q_INVOKABLE bool userIsGameFixedTimeEnabled();
    Q_INVOKABLE bool userIsFanclubEnabled();
    Q_INVOKABLE bool userIsFanclubEditEnabled();
    Q_INVOKABLE bool userIsConsoleEnabled();
    Q_INVOKABLE bool userIsAdminEnabled();

signals:

public slots:

private:
    QString m_ipAddr;
    QString m_login;
    QString m_passWord;
    QString m_userName;
    QString m_readableName;
    QString m_salt;
    qint32  m_userIndex;
    quint32 m_userProperties;
    qint32  m_emailNotify;

    QString m_currentRandomValue;

    QCryptographicHash* m_hash = nullptr;

    void setUserIndex(const qint32 userIndex);
    void setUserProperties(const quint32 userProperties);
};

extern cConUserSettings* g_ConUserSettings;

#endif // CCONSETTINGS_H
