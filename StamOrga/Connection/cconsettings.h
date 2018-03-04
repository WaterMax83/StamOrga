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

#include "../Common/General/cgendisposer.h"

class cConSettings : public cGenDisposer
{
    Q_OBJECT
public:
    explicit cConSettings(QObject* parent = nullptr);

    qint32 initialize() override;

    QString getIPAddr();
    void setIPAddr(const QString ipAddr);

    QString getPassWord() { return this->m_passWord; }

    QString getUserName() { return this->m_userName; }

    QString getReadableName() { return this->m_readableName; }

    QString getSalt() { return this->m_salt; }

    qint32 getMasterConPort() { return 55000; }

signals:

public slots:

private:
    QString m_ipAddr;
    QString m_login;
    QString m_passWord;
    QString m_userName;
    QString m_readableName;
    QString m_salt;
};

extern cConSettings g_ConSettings;

#endif // CCONSETTINGS_H
