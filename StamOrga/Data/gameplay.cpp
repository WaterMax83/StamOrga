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

#include "gameplay.h"

GamePlay::GamePlay(QObject* parent)
    : QObject(parent)
{
    this->m_freeTickets              = 0;
    this->m_blockedTickets           = 0;
    this->m_reservedTickets          = 0;
    this->m_acceptedMeeting          = 0;
    this->m_interestedMeeting        = 0;
    this->m_declinedMeeting          = 0;
    this->m_meetingInfo              = 0;
    this->m_bIsUserGameAddingEnabled = false;
}


bool GamePlay::isGameInPast()
{
    QDateTime now = QDateTime::currentDateTime();
    if (now.toMSecsSinceEpoch() > this->m_timestamp + (2 * 60 * 60 * qint64(1000)))
        return true;

    return false;
}

bool GamePlay::isGameRunning()
{
    QDateTime now = QDateTime::currentDateTime();
    if (now.toMSecsSinceEpoch() > this->m_timestamp) {
        if (now.toMSecsSinceEpoch() < this->m_timestamp + (2 * 60 * 60 * qint64(1000)))
            return true;
    }
    return false;
}

bool GamePlay::isGameAHomeGame()
{
    if (this->m_home == "KSC")
        return true;
    return false;
}

bool GamePlay::isGameASeasonTicketGame()
{
    if (!this->isGameAHomeGame())
        return false;

    switch (this->m_comp) {
    case BUNDESLIGA_1:
    case BUNDESLIGA_2:
    case LIGA_3:
    case DFB_POKAL:
        return true;
    default:
        return false;
    }

    return false;
}

QString GamePlay::getCompetitionLine()
{
    if (this->m_comp == BUNDESLIGA_1 || this->m_comp == BUNDESLIGA_2 || this->m_comp == LIGA_3)
        return QString("%1. Spieltag - ").arg(this->m_seasonIndex);
    else if (this->m_comp == DFB_POKAL || this->m_comp == BADISCHER_POKAL)
        return QString("%1. Runde - ").arg(this->m_seasonIndex);
    else if (this->m_comp == TESTSPIEL) {
#ifdef QT_DEBUG
        return QString("%1. ").arg(this->m_seasonIndex);
#else
        if (this->m_bIsUserGameAddingEnabled)
            return QString("%1. ").arg(this->m_seasonIndex);
        else
            return "";
#endif
    }

    return "not implemented";
}

void GamePlay::setEnableAddGame(bool enable)
{
    this->m_bIsUserGameAddingEnabled = enable;
}

bool GamePlay::compareTimeStampFunction(GamePlay* p1, GamePlay* p2)
{
    if (p1->m_timestamp > p2->m_timestamp)
        return false;
    return true;
}
