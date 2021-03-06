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

#ifndef CMEETINGINFOMANAGER_H
#define CMEETINGINFOMANAGER_H

#include <QObject>

#include "../Common/General/cgendisposer.h"
#include "../Common/Network/messageprotocol.h"
#include "../Network/connectiondata.h"

class cMeetingInfoManager : public cGenDisposer
{
    Q_OBJECT
public:
    explicit cMeetingInfoManager(QObject* parent = nullptr);

    qint32 initialize();

    MessageProtocol* getMeetingInfo(UserConData* pUserCon, MessageProtocol* request);

    MessageProtocol* getChangeMeetingInfo(UserConData* pUserCon, MessageProtocol* request);

    MessageProtocol* getAcceptMeetingInfo(UserConData* pUserCon, MessageProtocol* request);

    MessageProtocol* getSendComment(UserConData* pUserCon, MessageProtocol* request);

signals:

public slots:
};

extern cMeetingInfoManager g_MeetingInfoManager;

#endif // CMEETINGINFOMANAGER_H
