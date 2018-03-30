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

    QString getIPAddr();
    void setIPAddr(const QString ipAddr);

    QString getPassWord();
    void setPassWord(const QString password);

    QString getUserName();
    void setUserName(const QString name);

    Q_INVOKABLE QString getReadableName();
    void setReadableName(const QString name);

    QString getSalt();
    void setSalt(const QString salt);

    qint32  getUserIndex();
    quint32 getUserProperties();

    qint32 getMasterConPort() { return 55000; }

    qint32 startGettingUserProps(const bool loadEverything = false);
    qint32 handleUserPropsResponse(MessageProtocol* msg);

    Q_INVOKABLE qint32 startUpdateReadableName(QString name);
    qint32 handleUpdateReadableNameResponse(MessageProtocol* msg);

    Q_INVOKABLE qint32 startUpdatePassword(QString password);
    qint32 handleUpdatePasswordResponse(MessageProtocol* msg);

    void setRandomLoginValue(const QString random) { this->m_currentRandomValue = random; }

    QString createHashValue(const QString first, const QString second);

    Q_INVOKABLE bool userIsDebugEnabled();
    Q_INVOKABLE bool userIsGameAddingEnabled();
    Q_INVOKABLE bool userIsGameFixedTimeEnabled();
    Q_INVOKABLE bool userIsFanclubEnabled();
    Q_INVOKABLE bool userIsFanclubEditEnabled();

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

    QString m_newReadableName;
    QString m_currentRandomValue;
    QString m_newPassWord;

    QCryptographicHash* m_hash = NULL;

    void setUserIndex(const qint32 userIndex);
    void setUserProperties(const quint32 userProperties);
};

extern cConUserSettings g_ConUserSettings;

#endif // CCONSETTINGS_H
