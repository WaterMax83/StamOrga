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

#ifndef CMEDIAMANAGER_H
#define CMEDIAMANAGER_H

#include <QObject>
#include <QtCore/QJsonArray>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>
#include <QtCore/QList>
#include <QtCore/QMutex>

#include "../Common/General/cgendisposer.h"
#include "../Common/Network/messageprotocol.h"
#include "../Data/mediainfo.h"
#include "../General/globaldata.h"
#include "../Network/connectiondata.h"

class cMediaManager : public cGenDisposer
{
    Q_OBJECT
public:
    explicit cMediaManager(QObject* parent = nullptr);

    qint32 initialize();

    MessageProtocol* handleMediaCommand(UserConData* pUserCon, MessageProtocol* request);

signals:

public slots:

private:
    QMutex            m_mutex;
    QList<MediaInfo*> m_mediaInfos;

    qint32 handleMediaAddCommand(UserConData* pUserCon, GamesPlay* pGame, QString format, QByteArray& data);
    qint32 handleMediaGetListCommand(GamesPlay* pGame, QJsonObject& rootObjAnswer);
    qint32 handleMediaDeleteCommand(UserConData* pUserCon, GamesPlay* pGame, QStringList& lPics);
};

extern cMediaManager g_MediaManager;

#endif // CMEDIAMANAGER_H
