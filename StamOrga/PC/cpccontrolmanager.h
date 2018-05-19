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


#ifndef CPCCONTROLMANAGER_H
#define CPCCONTROLMANAGER_H

#include <QObject>
#include <QtCore/QMutex>

#include "../Common/General/cgendisposer.h"
#include "../Common/Network/messageprotocol.h"

class cPCControlManager : public cGenDisposer
{
    Q_OBJECT
public:
    explicit cPCControlManager(QObject* parent = nullptr);

    qint32 initialize();

    qint32 terminate();

    qint32 refreshControlList();

    qint32 saveControlList();

    qint32 handleControlCommand(MessageProtocol* msg);

    qint32  setStatistic(QString stats);
    QString getStastistic();

    qint32  setOnlineGames(QString games);
    QString getOnlineGames();

    qint32 setSmtpData(QString login, QString password, QString addresses);
    qint32 getStmpData(QString& login, QString& password, QString& addresses);

signals:

public slots:

private:
    QStringList m_statistic;
    QStringList m_onlineGames;
    QString     m_smtpLogin;
    QString     m_smtpPassword;
    QStringList m_smtpAddresses;

    QMutex m_mutex;
};

extern cPCControlManager* g_PCControlManager;

#endif // CPCCONTROLMANAGER_H
