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

#include <QtCore/QDebug>

#include "../../Common/Network/messagecommand.h"
#include "userinterface.h"

UserInterface::UserInterface(QObject* parent)
    : QObject(parent)
{
    this->m_pConHandle = new ConnectionHandling();
    connect(this->m_pConHandle, &ConnectionHandling::sNotifyConnectionFinished,
            this, &UserInterface::slConnectionRequestFinished);
    connect(this->m_pConHandle, &ConnectionHandling::sNotifyVersionRequest,
            this, &UserInterface::slVersionRequestFinished);
    connect(this->m_pConHandle, &ConnectionHandling::sNotifyUpdatePasswordRequest,
            this, &UserInterface::slUpdatePasswordRequestFinished);


    connect(this->m_pConHandle, &ConnectionHandling::sNotifyCommandFinished,
            this, &UserInterface::slCommandFinished);
}

qint32 UserInterface::startMainConnection(QString name, QString passw)
{
    return this->m_pConHandle->startMainConnection(name, passw);
}

qint32 UserInterface::startUpdateUserPassword(QString newPassw)
{
    return this->m_pConHandle->startUpdatePassword(newPassw);
}

qint32 UserInterface::startUpdateReadableName(QString name)
{
    return this->m_pConHandle->startUpdateReadableName(name);
}

qint32 UserInterface::startListGettingGames()
{
    return this->m_pConHandle->startListGettingGames();
}

qint32 UserInterface::startAddSeasonTicket(QString name, quint32 discount)
{
    return this->m_pConHandle->startAddSeasonTicket(name, discount);
}

qint32 UserInterface::startRemoveSeasonTicket(quint32 index)
{
    return this->m_pConHandle->startRemoveSeasonTicket(index);
}

qint32 UserInterface::startNewPlaceSeasonTicket(quint32 index, QString place)
{
    return this->m_pConHandle->startNewPlaceSeasonTicket(index, place);
}

qint32 UserInterface::startListSeasonTickets()
{
    return this->m_pConHandle->startListSeasonTickets();
}

qint32 UserInterface::startChangeAvailableTicketState(quint32 ticketIndex, quint32 gameIndex, quint32 state, QString name)
{
    return this->m_pConHandle->startChangeSeasonTicketState(ticketIndex, gameIndex, state, name);
}

qint32 UserInterface::startRequestAvailableTickets(quint32 gameIndex)
{
    return this->m_pConHandle->startListAvailableTicket(gameIndex);
}

qint32 UserInterface::startChangeGame(const quint32 index, const quint32 sIndex, const QString competition,
                                      const QString home, const QString away, const QString date, const QString score)
{
    return this->m_pConHandle->startChangeGame(index, sIndex, competition, home, away, date, score);
}

void UserInterface::slConnectionRequestFinished(qint32 result)
{
    //this->ui->btnSendData->setEnabled(true);
    emit this->notifyConnectionFinished(result);
    if (result > ERROR_CODE_NO_ERROR) {
        this->m_pConHandle->startGettingVersionInfo();
        this->m_pConHandle->startGettingUserProps();
    }
}

void UserInterface::slVersionRequestFinished(qint32 result, QString msg)
{
    emit this->notifyVersionRequestFinished(result, msg);
}


void UserInterface::slUpdatePasswordRequestFinished(qint32 result, QString newPassWord)
{
    emit this->notifyUpdatePasswordRequestFinished(result, newPassWord);
}


void UserInterface::slCommandFinished(quint32 command, qint32 result)
{
    switch (command) {
    case OP_CODE_CMD_REQ::REQ_GET_USER_PROPS:
        emit this->notifyUserPropertiesFinished(result);
        break;

    case OP_CODE_CMD_REQ::REQ_USER_CHANGE_READNAME:
        emit this->notifyUpdateReadableNameRequest(result);
        break;

    case OP_CODE_CMD_REQ::REQ_GET_GAMES_LIST:
        emit this->notifyGamesListFinished(result);
        break;

    case OP_CODE_CMD_REQ::REQ_ADD_TICKET:
        emit this->notifySeasonTicketAddFinished(result);
        break;

    case OP_CODE_CMD_REQ::REQ_REMOVE_TICKET:
        emit this->notifySeasonTicketRemoveFinished(result);
        break;

    case OP_CODE_CMD_REQ::REQ_GET_TICKETS_LIST:
        emit this->notifySeasonTicketListFinished(result);
        break;

    case OP_CODE_CMD_REQ::REQ_NEW_TICKET_PLACE:
        emit this->notifySeasonTicketNewPlaceFinished(result);
        break;

    case OP_CODE_CMD_REQ::REQ_STATE_CHANGE_SEASON_TICKET:
        emit this->notifyAvailableTicketStateChangedFinished(result);
        break;

    case OP_CODE_CMD_REQ::REQ_GET_AVAILABLE_TICKETS:
        emit this->notifyAvailableTicketListFinsished(result);
        break;

    case OP_CODE_CMD_REQ::REQ_CHANGE_GAME:
        emit this->notifyChangedGameFinished(result);
        break;

    default:
        qWarning().noquote() << QString("Unknown acknowledge: 0x%1").arg(QString::number(command, 16));
        break;
    }
}
