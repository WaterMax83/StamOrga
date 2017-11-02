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

#ifndef DATAHANDLING_H
#define DATAHANDLING_H

#include <QtCore/QString>

#include "../Common/Network/messageprotocol.h"
#include "../Data/globaldata.h"

class DataHandling
{
public:
    DataHandling(GlobalData* pData);

    qint32 getHandleLoginResponse(MessageProtocol* msg);
    qint32 getHandleVersionResponse(MessageProtocol* msg, QString* version);
    qint32 getHandleUserPropsResponse(MessageProtocol* msg);
    qint32 getHandleUserEventsResponse(MessageProtocol* msg);
    qint32 getHandleGamesListResponse(MessageProtocol* msg);
    qint32 getHandleGamesInfoListResponse(MessageProtocol* msg);
    qint32 getHandleSeasonTicketListResponse(MessageProtocol* msg);
    qint32 getHandleChangeTicketStateResponse(MessageProtocol* msg);
    qint32 getHandleAvailableTicketListResponse(MessageProtocol* msg, const quint32 gameIndex);
    qint32 getHandleChangeMeetingResponse(MessageProtocol* msg);
    qint32 getHandleAcceptMeetingResponse(MessageProtocol* msg);
    qint32 getHandleLoadMeetingInfo(MessageProtocol* msg);
    qint32 getHandleFanclubNewsChangeResponse(MessageProtocol* msg, QString& returnData);
    qint32 getHandleFanclubNewsListResponse(MessageProtocol* msg);
    qint32 getHandleFanclubNewsItemResponse(MessageProtocol* msg);

private:
    GlobalData* m_pGlobalData;
};

#endif // DATAHANDLING_H
