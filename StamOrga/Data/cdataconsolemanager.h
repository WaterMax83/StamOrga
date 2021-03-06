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


#ifndef CDATACONSOLEMANAGER_H
#define CDATACONSOLEMANAGER_H

#include <QObject>
#include <QtCore/QJsonArray>
#include <QtCore/QJsonObject>

#include "../Common/General/cgendisposer.h"
#include "../Common/Network/messageprotocol.h"

class cDataConsoleManager : public cGenDisposer
{
    Q_OBJECT
public:
    explicit cDataConsoleManager(QObject* parent = nullptr);

    qint32 initialize();

    Q_INVOKABLE QString getLastConsoleOutput();

    Q_INVOKABLE qint32 startSendConsoleCommand(const QString command);
    qint32 handleConsoleCommandResponse(MessageProtocol* msg);

private:
    QString m_consoleOutput;
};

extern cDataConsoleManager* g_DataConsoleManager;

#endif // CDATACONSOLEMANAGER_H
