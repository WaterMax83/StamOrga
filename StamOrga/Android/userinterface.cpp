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
#include "Connection/cconmanager.h"
#include "Connection/cconusersettings.h"
#include "userinterface.h"

UserInterface::UserInterface(QObject* parent)
    : QObject(parent)
{
    connect(&g_ConManager, &cConManager::signalNotifyConnectionFinished,
            this, &UserInterface::slotConnectionRequestFinished);

    connect(&g_ConManager, &cConManager::signalNotifyCommandFinished,
            this, &UserInterface::slotCommandFinished);
}

qint32 UserInterface::startMainConnection(QString name, QString passw)
{
    if (name != g_ConUserSettings.getUserName()) {
        g_ConUserSettings.setUserName("");
        g_ConUserSettings.setPassWord("");
        g_ConUserSettings.setSalt("");
    }
    if (passw == "dEf1AuLt") {
        passw = g_ConUserSettings.getPassWord();
    } else {
        g_ConUserSettings.setPassWord("");
        g_ConUserSettings.setSalt("");
    }

    return g_ConManager.startMainConnection(name, passw);
}

qint32 UserInterface::startGetUserEvents()
{
    //    return this->m_pConHandle->startGettingUserEvents();
    return ERROR_CODE_NOT_IMPLEMENTED;
}

qint32 UserInterface::startSetUserEvents(qint64 eventID, qint32 status)
{
    //    return this->m_pConHandle->startSettingUserEvents(eventID, status);
    return ERROR_CODE_NOT_IMPLEMENTED;
}

qint32 UserInterface::startListGettingGames()
{
    //    return this->m_pConHandle->startListGettingGames();
    return ERROR_CODE_NOT_IMPLEMENTED;
}

qint32 UserInterface::startListGettingGamesInfo()
{
    //    return this->m_pConHandle->startListGettingGamesInfo();
    return ERROR_CODE_NOT_IMPLEMENTED;
}

qint32 UserInterface::startSetFixedGameTime(const quint32 gameIndex, const quint32 fixed)
{
    //    return this->m_pConHandle->startSetFixedGameTime(gameIndex, fixed);
    return ERROR_CODE_NOT_IMPLEMENTED;
}

qint32 UserInterface::startChangeAvailableTicketState(quint32 ticketIndex, quint32 gameIndex, quint32 state, QString name)
{
    //    return this->m_pConHandle->startChangeSeasonTicketState(ticketIndex, gameIndex, state, name);
    return ERROR_CODE_NOT_IMPLEMENTED;
}

qint32 UserInterface::startRequestAvailableTickets(quint32 gameIndex)
{
    //    return this->m_pConHandle->startListAvailableTicket(gameIndex);
    return ERROR_CODE_NOT_IMPLEMENTED;
}

qint32 UserInterface::startChangeGame(const quint32 index, const quint32 sIndex, const QString competition,
                                      const QString home, const QString away, const QString date, const QString score)
{
    //    return this->m_pConHandle->startChangeGame(index, sIndex, competition, home, away, date, score);
    return ERROR_CODE_NOT_IMPLEMENTED;
}

qint32 UserInterface::startSaveMeetingInfo(const quint32 gameIndex, const QString when, const QString where, const QString info,
                                           const quint32 type)
{
    //    return this->m_pConHandle->startSaveMeetingInfo(gameIndex, when, where, info, type);
    return ERROR_CODE_NOT_IMPLEMENTED;
}

qint32 UserInterface::startLoadMeetingInfo(const quint32 gameIndex, const quint32 type)
{
    //    return this->m_pConHandle->startLoadMeetingInfo(gameIndex, type);
    return ERROR_CODE_NOT_IMPLEMENTED;
}

qint32 UserInterface::startAcceptMeetingInfo(const quint32 gameIndex, const quint32 accept,
                                             const QString name, const quint32 type,
                                             const quint32 acceptIndex)
{
    //    return this->m_pConHandle->startAcceptMeetingInfo(gameIndex, accept, name, type, acceptIndex);
    return ERROR_CODE_NOT_IMPLEMENTED;
}

qint32 UserInterface::startStatisticsCommand(const QByteArray& command)
{
    //    return this->m_pConHandle->startStatisticsCommand(command);
    return ERROR_CODE_NOT_IMPLEMENTED;
}

void UserInterface::slotConnectionRequestFinished(qint32 result, const QString msg)
{
    emit this->notifyConnectionFinished(result, msg);
}


void UserInterface::slotCommandFinished(quint32 command, qint32 result)
{
    QMutexLocker lock(&this->m_notifyMutex);

    //    qInfo() << "New Command " << command << " " << result;

    switch (command) {
    case OP_CODE_CMD_REQ::REQ_GET_VERSION:
        emit this->notifyVersionRequestFinished(result);
        break;

    case OP_CODE_CMD_REQ::REQ_USER_CHANGE_LOGIN:
        emit this->notifyUpdatePasswordRequestFinished(result);
        break;

    case OP_CODE_CMD_REQ::REQ_GET_USER_PROPS:
        emit this->notifyUserPropertiesFinished(result);
        break;

    case OP_CODE_CMD_REQ::REQ_USER_CHANGE_READNAME:
        emit this->notifyUpdateReadableNameRequest(result);
        break;

    case OP_CODE_CMD_REQ::REQ_GET_GAMES_LIST:
        emit this->notifyGamesListFinished(result);
        //        if (result == ERROR_CODE_SUCCESS) {
        //            this->m_pConHandle->startGettingUserEvents();
        //            this->m_pConHandle->startListGettingGamesInfo();
        //        }
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

    case OP_CODE_CMD_REQ::REQ_CHANGE_AWAYTRIP_INFO:
        emit this->notifyChangedAwayTripInfoFinished(result);
        break;

    case OP_CODE_CMD_REQ::REQ_GET_MEETING_INFO:
        emit this->notifyLoadMeetingInfoFinished(result);
        break;

    case OP_CODE_CMD_REQ::REQ_GET_AWAYTRIP_INFO:
        emit this->notifyLoadAwayTripInfoFinished(result);
        break;

    case OP_CODE_CMD_REQ::REQ_ACCEPT_MEETING:
        emit this->notifyAcceptMeetingFinished(result);
        break;

    case OP_CODE_CMD_REQ::REQ_ACCEPT_AWAYTRIP:
        emit this->notifyAcceptAwayTripFinished(result);
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
        emit this->notifyGetUserEvents(result);
        break;

    case OP_CODE_CMD_REQ::REQ_SET_USER_EVENTS:
        if (result == ERROR_CODE_SUCCESS)
            this->startGetUserEvents();
        break;

    case OP_CODE_CMD_REQ::REQ_CMD_STATISTIC:
        emit this->notifyStatisticsCommandFinished(result);
        break;

    default:
        qWarning().noquote() << QString("Unknown acknowledge: 0x%1").arg(QString::number(command, 16));
        break;
    }
}
