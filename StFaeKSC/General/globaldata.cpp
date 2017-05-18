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

#include <QtCore/QDataStream>
#include <QtCore/QDateTime>
#include <QtCore/QDir>
#include <QtCore/QFileInfo>

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

qint32 GlobalData::requestChangeStateSeasonTicket(quint32 ticketIndex, quint32 gameIndex, quint32 newState, QString reserveName, const QString userName)
{
    GamesPlay*  pGame   = (GamesPlay*)this->m_GamesList.getItem(gameIndex);
    TicketInfo* pTicket = (TicketInfo*)this->m_SeasonTicket.getItem(ticketIndex);
    if (pGame == NULL || pTicket == NULL)
        return ERROR_CODE_NOT_FOUND;

    if (newState == TICKET_STATE_NOT_POSSIBLE)
        return ERROR_CODE_NOT_FOUND;

#undef ENABLE_PAST_CHECK
//#define ENABLE_PAST_CHECK
#ifdef ENABLE_PAST_CHECK
    if (pGame->m_timestamp < QDateTime::currentMSecsSinceEpoch())
        return ERROR_CODE_IN_PAST;
#endif

    qint32  result = ERROR_CODE_SUCCESS;
    quint32 userID = this->m_UserList.getItemIndex(userName);
    foreach (AvailableGameTickets* ticket, this->m_availableTickets) {
        if (ticket->getGameIndex() == gameIndex) {
            quint32 currentState = ticket->getTicketState(ticketIndex);
            QString currentName  = ticket->getTicketName(ticketIndex);
            if (currentState == newState && currentName == reserveName)
                qInfo().noquote() << QString("Ticket %1 at game %3 already has state %4 and name %2").arg(pTicket->m_itemName, currentName).arg(pGame->m_index).arg(currentState);
            else if (newState == TICKET_STATE_FREE || newState == TICKET_STATE_BLOCKED) {
                if (currentState == TICKET_STATE_NOT_POSSIBLE) /* Not found, add new */
                    result = ticket->addNewTicket(ticketIndex, userID, newState);
                else /* ticket found, just change actual state */
                    result = ticket->changeTicketState(ticketIndex, userID, newState);
            } else if (newState == TICKET_STATE_RESERVED) {
                if (currentState == TICKET_STATE_FREE || currentState == TICKET_STATE_RESERVED)
                    result = ticket->changeTicketState(ticketIndex, userID, newState, reserveName);
                else
                    result = ERROR_CODE_NOT_POSSIBLE;
                /* Anything else is not possible */
            } else
                result = ERROR_CODE_NOT_POSSIBLE;

            if (result == ERROR_CODE_SUCCESS)
                qInfo().noquote() << QString("Changed ticketState from %1 at game %2 to %3").arg(pTicket->m_itemName).arg(pGame->m_index).arg(newState);
            else
                qWarning().noquote() << QString("Error setting ticket state %1: %2").arg(newState).arg(result);
            return result;
        }
    }

    if (newState != TICKET_STATE_FREE) {
        qWarning().noquote() << QString("Error setting ticket state %1: %2").arg(newState).arg("Not possible because it does not exist");
        return ERROR_CODE_NOT_POSSIBLE;
    }

    AvailableGameTickets* ticket = new AvailableGameTickets();
    if (ticket->initialize(pGame->m_saison, pGame->m_competition, pGame->m_saisonIndex, pGame->m_index)) {
        this->m_availableTickets.append(ticket);
        result = ticket->addNewTicket(ticketIndex, userID, TICKET_STATE_FREE, reserveName);
        qInfo().noquote() << QString("Changed ticketState from %1 at game %2 to %3").arg(pTicket->m_itemName).arg(pGame->m_index).arg(TICKET_STATE_FREE);
    } else {
        delete ticket;
        qWarning().noquote() << QString("Error creating available ticket file for game %1").arg(pGame->m_index);
        return ERROR_CODE_NOT_POSSIBLE;
    }

    return ERROR_CODE_SUCCESS;
}


/*  answer
 * 0                Header          12
 * 12   quint32     result          4
 * 16   quint16     freeCount       2
 * 18   quint16     reserveCount    2
 * 20   quint32     fticketIndex1   4
 * 24   quint32     fticketIndex2   4
 *
 * X    quint32     rticketIndex1   4
 * X+4  quint32     rTicketName     Y
 * x+4+Yquint8      0x0             1
 */
qint32 GlobalData::requestGetAvailableSeasonTicket(const quint32 gameIndex, const QString userName, QByteArray& data)
{
    GamesPlay* pGame = (GamesPlay*)this->m_GamesList.getItem(gameIndex);
    if (pGame == NULL)
        return ERROR_CODE_NOT_FOUND;

    Q_UNUSED(userName);

    foreach (AvailableGameTickets* ticket, this->m_availableTickets) {
        if (ticket->getGameIndex() == gameIndex) {
            quint16 totalCount = ticket->startRequestGetItemList();

            QByteArray  freeTickets;
            QDataStream wFreeTickets(&freeTickets, QIODevice::WriteOnly);
            wFreeTickets.setByteOrder(QDataStream::BigEndian);

            QByteArray  reservedTickets;
            QDataStream wReserveds(&reservedTickets, QIODevice::WriteOnly);
            wReserveds.setByteOrder(QDataStream::BigEndian);

            quint16 freeTicktetCount    = 0;
            quint16 reservedTicketCount = 0;
            for (int i = 0; i < totalCount; i++) {
                AvailableTicketInfo* info = (AvailableTicketInfo*)ticket->getRequestConfigItemFromListIndex(i);
                if (info == NULL) {
                    wFreeTickets << quint32(0x0);
                    continue;
                }
                if (info->m_state == TICKET_STATE_FREE) {
                    wFreeTickets << quint32(info->m_ticketID);
                    freeTicktetCount++;
                } else if (info->m_state == TICKET_STATE_RESERVED) {
                    wReserveds.device()->seek(reservedTickets.size());
                    wReserveds << quint32(info->m_ticketID);
                    reservedTickets.append(info->m_itemName);
                    reservedTickets.append(char(0x00));
                    reservedTicketCount++;
                }
            }

            ticket->stopRequestGetItemList();

            QDataStream wData(&data, QIODevice::WriteOnly);
            wData.setByteOrder(QDataStream::BigEndian);

            wData << quint32(ERROR_CODE_SUCCESS) << quint16(freeTicktetCount) << quint16(reservedTicketCount);
            data.append(freeTickets);
            data.append(reservedTickets);

            return ERROR_CODE_SUCCESS;
        }
    }

    QDataStream wData(&data, QIODevice::WriteOnly);
    wData.setByteOrder(QDataStream::BigEndian);

    wData << quint32(ERROR_CODE_SUCCESS) << quint16(0);

    return ERROR_CODE_SUCCESS;
}
