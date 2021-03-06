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

#include "../Connection/cconusersettings.h"
#include "gameplay.h"

GamePlay::GamePlay(QObject* parent)
    : QObject(parent)
{
    this->m_freeTickets       = 0;
    this->m_blockedTickets    = 0;
    this->m_reservedTickets   = 0;
    this->m_acceptedMeeting   = 0;
    this->m_interestedMeeting = 0;
    this->m_declinedMeeting   = 0;
    this->m_meetingInfo       = 0;
    this->m_acceptedTrip      = 0;
    this->m_interestedTrip    = 0;
    this->m_declinedTrip      = 0;
    this->m_driveInfo         = 0;
    this->m_eventCount        = 0;

    this->m_bHasTicketEvent   = false;
    this->m_bHasMeetingEvent  = false;
    this->m_bHasAwayTripEvent = false;
    this->m_bHasMediaEvent    = false;
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

    return gIsGameASeasonTicketGame(this->m_comp);
}

bool GamePlay::isGameAAwayGame()
{
    if (this->isGameAHomeGame())
        return false;

    if (this->competitionValue() == CompetitionIndex::OTHER_COMP)
        return false;

    return true;
}

QString GamePlay::getCompetitionRound()
{
    if (this->m_comp == BUNDESLIGA_1 || this->m_comp == BUNDESLIGA_2) {
        if (this->m_seasonIndex <= 34)
            return QString("%1. Spieltag - ").arg(this->m_seasonIndex);
        else
            return QString("Relegation %1 - ").arg(this->m_seasonIndex == 35 ? "Hinspiel" : "Rückspiel");
    } else if (this->m_comp == LIGA_3) {
        if (this->m_seasonIndex <= 38)
            return QString("%1. Spieltag - ").arg(this->m_seasonIndex);
        else
            return QString("Relegation %1 - ").arg(this->m_seasonIndex == 39 ? "Hinspiel" : "Rückspiel");
    } else if (this->m_comp == DFB_POKAL || this->m_comp == BADISCHER_POKAL) {
        if (this->m_seasonIndex < 6)
            return QString("%1. Runde - ").arg(this->m_seasonIndex);
        else if (this->m_seasonIndex == 7)
            return QString("Achtelfinale - ");
        else if (this->m_seasonIndex == 8)
            return QString("Viertelfinale - ");
        else if (this->m_seasonIndex == 9)
            return QString("Halbfinale - ");
        else if (this->m_seasonIndex == 10)
            return QString("Finale - ");
        else
            return QString("Unbekannte Runde: %1 - ").arg(this->m_seasonIndex);
    } else if (this->m_comp == TESTSPIEL) {
#ifdef QT_DEBUG
        return QString("%1. ").arg(this->m_seasonIndex);
#else
        if (g_ConUserSettings->userIsGameAddingEnabled())
            return QString("%1. ").arg(this->m_seasonIndex);
        else
            return "";
#endif
    }

    return "not implemented";
}

QString GamePlay::getCompetitionShortRound()
{
    if (this->m_comp == BUNDESLIGA_1 || this->m_comp == BUNDESLIGA_2) {
        if (this->m_seasonIndex <= 34)
            return QString("%1. Spieltag").arg(this->m_seasonIndex);
        else
            return QString("Relegation %1").arg(this->m_seasonIndex == 35 ? "Hinspiel" : "Rückspiel");
    } else if (this->m_comp == LIGA_3) {
        if (this->m_seasonIndex <= 38)
            return QString("%1. Spieltag").arg(this->m_seasonIndex);
        else
            return QString("Relegation %1").arg(this->m_seasonIndex == 39 ? "Hinspiel" : "Rückspiel");
    } else if (this->m_comp == DFB_POKAL || this->m_comp == BADISCHER_POKAL) {
        if (this->m_seasonIndex < 6)
            return QString("%1. Runde").arg(this->m_seasonIndex);
        else if (this->m_seasonIndex == 7)
            return QString("1/8 Finale");
        else if (this->m_seasonIndex == 8)
            return QString("1/4 Finale");
        else if (this->m_seasonIndex == 9)
            return QString("1/2 Finale");
        else if (this->m_seasonIndex == 10)
            return QString("Finale");
        else
            return QString("Unb. Runde: %1").arg(this->m_seasonIndex);
    } else if (this->m_comp == TESTSPIEL) {
#ifdef QT_DEBUG
        return QString("%1. TestSpiel").arg(this->m_seasonIndex);
#else
        if (g_ConUserSettings->userIsGameAddingEnabled())
            return QString("%1. TestSpiel").arg(this->m_seasonIndex);
        else
            return "TestSpiel";
#endif
    } else if (this->m_comp == OTHER_COMP) {
        return "";
    }

    return "n.i.";
}

bool GamePlay::compareTimeStampFunctionAscending(GamePlay* p1, GamePlay* p2)
{
    if (p1->m_timestamp > p2->m_timestamp)
        return false;
    return true;
}

bool GamePlay::compareTimeStampFunctionDescending(GamePlay* p1, GamePlay* p2)
{
    if (p1->m_timestamp < p2->m_timestamp)
        return false;
    return true;
}
