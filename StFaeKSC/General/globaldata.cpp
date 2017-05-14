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

#include <QtCore/QDir>
#include <QtCore/QFileInfo>
#include <QtCore/QDateTime>
#include <QtCore/QDataStream>

#include "../Common/General/globalfunctions.h"
#include "globaldata.h"

GlobalData::GlobalData()
{
    QString userSetDirPath = getUserHomeConfigPath() + "/Settings/";

    QDir userSetDir(userSetDirPath);
    if (!userSetDir.exists())
        return;

    QStringList nameFilter;
    nameFilter << "Tickets_Game_*.ini";

    QStringList infoConfigList = userSetDir.entryList(nameFilter, QDir::Files | QDir::Readable);

    foreach (QString file, infoConfigList) {
        AvailableGameTickets* tickets = new AvailableGameTickets();

        if (tickets->initialize(userSetDirPath + file) >= 0)
            this->m_availableTickets.append(tickets);
        else
            delete tickets;
    }
}

qint32 GlobalData::requestFreeSeasonTicket(quint32 ticketIndex, quint32 gameIndex, const QString userName)
{
    GamesPlay*  pGame   = (GamesPlay*)this->m_GamesList.getItem(gameIndex);
    TicketInfo* pTicket = (TicketInfo*)this->m_SeasonTicket.getItem(ticketIndex);
    if (pGame == NULL || pTicket == NULL)
        return ERROR_CODE_NOT_FOUND;

    if (pGame->m_timestamp < QDateTime::currentMSecsSinceEpoch())
        return ERROR_CODE_IN_PAST;

    qint32  result;
    quint32 userID = this->m_UserList.getItemIndex(userName);
    foreach (AvailableGameTickets* ticket, this->m_availableTickets) {
        if (ticket->getGameIndex() == gameIndex) {
            quint32 state = ticket->getTicketState(ticketIndex);
            if (state == TICKET_STATE_NOT_POSSIBLE) {
                /* Not found, add new */
                result = ticket->addNewTicket(ticketIndex, userID, TICKET_STATE_FREE);

            } else if (state == TICKET_STATE_NOT_POSSIBLE) {
                qInfo().noquote() << QString("Ticket %1 at game %2 already has state %3").arg(pTicket->m_itemName, pGame->m_index).arg(TICKET_STATE_FREE);
            } else {
                /* ticket found, just change actual state */
                result = ticket->changeTicketState(ticketIndex, userID, TICKET_STATE_FREE);
            }
            if (result == ERROR_CODE_SUCCESS)
                qInfo().noquote() << QString("Changed ticketState from %1 at game %2 to %3").arg(pTicket->m_itemName).arg(pGame->m_index).arg(TICKET_STATE_FREE);
            else
                qWarning().noquote() << QString("Error setting ticket state %1: %2").arg(TICKET_STATE_FREE).arg(result);
            return result;
        }
    }

    AvailableGameTickets* ticket = new AvailableGameTickets();
    if (ticket->initialize(pGame->m_saison, pGame->m_competition, pGame->m_saisonIndex, pGame->m_index)) {
        this->m_availableTickets.append(ticket);
        result = ticket->addNewTicket(ticketIndex, userID, TICKET_STATE_FREE);
        qInfo().noquote() << QString("Changed ticketState from %1 at game %2 to %3").arg(pTicket->m_itemName).arg(pGame->m_index).arg(TICKET_STATE_FREE);
    } else {
        delete ticket;
        qWarning().noquote() << QString("Error creating available ticket file for game %1").arg(pGame->m_index);
        return ERROR_CODE_COMMON;
    }

    return ERROR_CODE_SUCCESS;
}


/*  answer
 * 0                Header          12
 * 12   quint32     result          4
 * 16   quint16     freeCount       2
 * 18   quint32     ticketIndex1    4
 * 22   quint32     ticketIndex2    8
 */
qint32 GlobalData::requestGetAvailableSeasonTicket(const quint32 gameIndex, const QString userName, QByteArray &data)
{
    GamesPlay*  pGame   = (GamesPlay*)this->m_GamesList.getItem(gameIndex);
    if (pGame == NULL)
        return ERROR_CODE_NOT_FOUND;

    foreach (AvailableGameTickets* ticket, this->m_availableTickets) {
        if (ticket->getGameIndex() == gameIndex) {
            quint16 totalCount = ticket->startRequestGetItemList();

            QByteArray freeTicket;
            QDataStream wFreeTicket(&freeTicket, QIODevice::WriteOnly);
            wFreeTicket.setByteOrder(QDataStream::BigEndian);
            for(int i= 0; i < totalCount; i++) {
                AvailableTicketInfo *info = (AvailableTicketInfo*)ticket->getRequestConfigItemFromListIndex(i);
                if (info == NULL) {
                    wFreeTicket << quint16(0x0);
                    continue;
                }
                if (info->m_state == TICKET_STATE_FREE)
                    wFreeTicket << info->m_ticketID;
            }

            ticket->stopRequestGetItemList();

            QDataStream wData(&data, QIODevice::WriteOnly);
            wData.setByteOrder(QDataStream::BigEndian);

            wData << quint32(ERROR_CODE_SUCCESS) << quint16(freeTicket.size() / 2);
            data.append(freeTicket);

            ERROR_CODE_SUCCESS;
        }
    }

    return ERROR_CODE_NOT_FOUND;
}
