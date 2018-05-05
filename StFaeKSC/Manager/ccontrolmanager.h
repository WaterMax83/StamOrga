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

#ifndef CCONTROLMANAGER_H
#define CCONTROLMANAGER_H

#include <QObject>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>
#include <QtCore/QSettings>

#include "../Common/General/cgendisposer.h"
#include "../Common/Network/messageprotocol.h"
#include "../Network/connectiondata.h"
#include "../Data/configlist.h"

class cControlManager : public cGenDisposer, public ConfigList
{
    Q_OBJECT
public:
    explicit cControlManager(QObject *parent = 0);

    qint32 initialize();

    MessageProtocol* getControlCommandResponse(UserConData* pUserCon, MessageProtocol* request);

    qint32 checkConsistency() { return ERROR_CODE_NOT_FOUND; }

signals:

public slots:

private:
    void saveCurrentInteralList() override;

    QList<qint32> m_statistic;

    qint32 handleRefreshCommand(QJsonObject& rootAns);
    qint32 handleSaveCommand(QJsonObject& rootObj);
};

extern cControlManager g_ControlManager;

#endif // CCONTROLMANAGER_H
