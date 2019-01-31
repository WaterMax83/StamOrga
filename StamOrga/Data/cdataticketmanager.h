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

#ifndef CDATATICKETMANAGER_H
#define CDATATICKETMANAGER_H

#include <QObject>
#include <QtCore/QList>
#include <QtCore/QMutex>

#include "../Common/General/cgendisposer.h"
#include "../Common/Network/messageprotocol.h"
#include "seasonticket.h"

class cDataTicketManager : public cGenDisposer
{
    Q_OBJECT
public:
    explicit cDataTicketManager(QObject* parent = nullptr);

    qint32 initialize();

    void addNewSeasonTicket(SeasonTicketItem* sTicket, const quint16 updateIndex = 0);

    SeasonTicketItem* getSeasonTicket(qint32 ticketIndex);

    Q_INVOKABLE qint32 getSeasonTicketLength();
    Q_INVOKABLE SeasonTicketItem* getSeasonTicketFromArrayIndex(int index);
    Q_INVOKABLE QString getSeasonTicketLastLocalUpdateString();

    Q_INVOKABLE qint32 startListSeasonTickets();
    qint32             handleListSeasonTicketsResponse(MessageProtocol* msg);
    qint32             clearTicketList();

    Q_INVOKABLE qint32 startAddSeasonTicket(const qint32 index, const QString name, const QString place, const qint32 discount);
    qint32             handleAddSeasonTicketResponse(MessageProtocol* msg);

    Q_INVOKABLE qint32 startRemoveSeasonTicket(const qint32 index);
    qint32             handleRemoveSeasonTicketResponse(MessageProtocol* msg);

    Q_INVOKABLE qint32 startListAvailableTickets(const qint32 gameIndex);
    qint32             handleListAvailableSeasonTicketResponse(MessageProtocol* msg);

    Q_INVOKABLE qint32 startChangeAvailableTicketState(const qint32 ticketIndex, const qint32 gameIndex,
                                                       const qint32 state, const QString name = "");
    qint32             handleChangeAvailableTicketResponse(MessageProtocol* msg);

signals:

public slots:

private:
    QList<SeasonTicketItem*> m_lTickets;
    QMutex                   m_mutex;

    qint64 m_stLastLocalUpdateTimeStamp;
    qint64 m_stLastServerUpdateTimeStamp;
};

extern cDataTicketManager* g_DataTicketManager;

#endif // CDATATICKETMANAGER_H
