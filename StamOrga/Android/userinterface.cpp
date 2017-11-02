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
    bool saveSettings = false;
    if (name != this->m_pConHandle->getGlobalData()->userName()) {
        saveSettings = true;
        this->m_pConHandle->getGlobalData()->setUserName("");
        this->m_pConHandle->getGlobalData()->setPassWord("");
        this->m_pConHandle->getGlobalData()->setSalt("");
    }
    if (passw == "dEf1AuLt") {
        passw = this->m_pConHandle->getGlobalData()->passWord();
    } else {
        this->m_pConHandle->getGlobalData()->setPassWord("");
        this->m_pConHandle->getGlobalData()->setSalt("");
        saveSettings = true;
    }

    if (saveSettings)
        this->m_pConHandle->getGlobalData()->saveGlobalUserSettings();

    return this->m_pConHandle->startMainConnection(name, passw);
}

qint32 UserInterface::startSetUserEvents(qint64 eventID, qint32 status)
{
    return this->m_pConHandle->startSettingUserEvents(eventID, status);
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

qint32 UserInterface::startListGettingGamesInfo()
{
    return this->m_pConHandle->startListGettingGamesInfo();
}

qint32 UserInterface::startSetFixedGameTime(const quint32 gameIndex, const quint32 fixed)
{
    return this->m_pConHandle->startSetFixedGameTime(gameIndex, fixed);
}

qint32 UserInterface::startAddSeasonTicket(QString name, quint32 discount)
{
    return this->m_pConHandle->startAddSeasonTicket(name, discount);
}

qint32 UserInterface::startRemoveSeasonTicket(quint32 index)
{
    return this->m_pConHandle->startRemoveSeasonTicket(index);
}

qint32 UserInterface::startEditSeasonTicket(quint32 index, QString name, QString place, quint32 discount)
{
    return this->m_pConHandle->startEditSeasonTicket(index, name, place, discount);
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

qint32 UserInterface::startSaveMeetingInfo(const quint32 gameIndex, const QString when, const QString where, const QString info)
{
    return this->m_pConHandle->startSaveMeetingInfo(gameIndex, when, where, info);
}

qint32 UserInterface::startLoadMeetingInfo(const quint32 gameIndex)
{
    return this->m_pConHandle->startLoadMeetingInfo(gameIndex);
}

qint32 UserInterface::startAcceptMeetingInfo(const quint32 gameIndex, const quint32 accept, const QString name, const quint32 acceptIndex)
{
    return this->m_pConHandle->startAcceptMeetingInfo(gameIndex, accept, name, acceptIndex);
}

qint32 UserInterface::startChangeFanclubNews(const quint32 newsIndex, const QString header, const QString info)
{
    return this->m_pConHandle->startChangeFanclubNews(newsIndex, header, info);
}

qint32 UserInterface::startGetFanclubNewsItem(const quint32 newsIndex)
{
    return this->m_pConHandle->startGetFanclubNewsItem(newsIndex);
}

qint32 UserInterface::startDeleteFanclubNewsItem(const quint32 newsIndex)
{
    return this->m_pConHandle->startDeleteFanclubNewsItem(newsIndex);
}

qint32 UserInterface::startListFanclubNews()
{
    return this->m_pConHandle->startListFanclubNews();
}

void UserInterface::slConnectionRequestFinished(qint32 result)
{
    emit this->notifyConnectionFinished(result);
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
        if (result == ERROR_CODE_SUCCESS)
            this->m_pConHandle->startListGettingGamesInfo();
        break;

    case OP_CODE_CMD_REQ::REQ_GET_GAMES_INFO_LIST:
        emit this->notifyGamesInfoListFinished(result);
        break;

    case OP_CODE_CMD_REQ::REQ_SET_FIXED_GAME_TIME:
        emit this->notifySetGamesFixedTimeFinished(result);
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

    case OP_CODE_CMD_REQ::REQ_CHANGE_TICKET:
        emit this->notifySeasonTicketEditFinished(result);
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

    case OP_CODE_CMD_REQ::REQ_CHANGE_MEETING_INFO:
        emit this->notifyChangedMeetingInfoFinished(result);
        break;

    case OP_CODE_CMD_REQ::REQ_GET_MEETING_INFO:
        emit this->notifyLoadMeetingInfoFinished(result);
        break;

    case OP_CODE_CMD_REQ::REQ_ACCEPT_MEETING:
        emit this->notifyAcceptMeetingFinished(result);
        break;

    case OP_CODE_CMD_REQ::REQ_CHANGE_NEWS_DATA:
        emit this->notifyChangeNewsDataFinished(result);
        break;

    case OP_CODE_CMD_REQ::REQ_GET_NEWS_DATA_LIST:
        emit this->notifyFanclubNewsListFinished(result);
        break;

    case OP_CODE_CMD_REQ::REQ_GET_NEWS_DATA_ITEM:
        emit this->notifyGetFanclubNewsItemFinished(result);
        break;

    case OP_CODE_CMD_REQ::REQ_DEL_NEWS_DATA_ITEM:
        emit this->notifyDeleteFanclubNewsItemFinished(result);
        break;

    case OP_CODE_CMD_REQ::REQ_GET_USER_EVENTS:
    case OP_CODE_CMD_REQ::REQ_SET_USER_EVENTS:
        qDebug() << "acknowledge " << command;
        break;

    default:
        qWarning().noquote() << QString("Unknown acknowledge: 0x%1").arg(QString::number(command, 16));
        break;
    }
}
