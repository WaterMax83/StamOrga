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

#ifndef GAMES_H
#define GAMES_H


#include <QtCore/QList>


#include "../Common/General/globalfunctions.h"
#include "configlist.h"


class GamesPlay : public ConfigItem
{
public:
    QString          m_away;
    quint8           m_seasonIndex;
    CompetitionIndex m_competition;
    qint32           m_season;
    QString          m_score;
    qint64           m_lastUpdate;
    qint32           m_options;

    GamesPlay(QString home, QString away, qint64 timestamp,
              quint8 sIndex, QString score, CompetitionIndex comp,
              qint32 season, qint32 index, qint64 lastUpdate,
              qint32 options)
    {
        this->m_itemName  = home;
        this->m_index     = index;
        this->m_timestamp = timestamp;

        this->m_away        = away;
        this->m_seasonIndex = sIndex;
        this->m_score       = score;
        this->m_competition = comp;
        this->m_season      = season;
        this->m_lastUpdate  = lastUpdate;
        this->m_options     = options;
    }
};

// clang-format off
#define PLAY_AWAY           "away"
#define PLAY_SAISON_INDEX   "sIndex"
#define PLAY_SAISON         "saison"
#define PLAY_SCORE          "score"
#define PLAY_COMPETITION    "competition"
#define PLAY_LAST_UDPATE    "lastUpdate"
#define PLAY_OPTIONS        "scheduled"

#define PLAY_OPTIONS_FIXED      0x1
#define PLAY_OPTIONS_FANCLUB    0x2

#define IS_PLAY_FIXED(value)            (value & PLAY_OPTIONS_FIXED) ? true : false
#define IS_PLAY_ONLY_FANCLUB(value)     (value & PLAY_OPTIONS_FANCLUB) ? true : false
// clang-format on

class Games : public ConfigList
{
public:
    Games();
    ~Games();

    int addNewGame(QString home, QString away, qint64 timestamp,
                   quint8 sIndex, QString score, CompetitionIndex comp,
                   qint32 season = 0, qint64 lastUpdate = 0);
    QString showAllGames(const bool showUpdate);

    int changeOptionValue(const quint32 gameIndex, const qint32 option);

    qint64 getTimeStampofFirstTicketGame(const qint32 season);

    GamesPlay* gameExists(quint8 sIndex, CompetitionIndex comp, quint16 saison, qint64 timestamp);

    virtual qint32 checkConsistency() { return -12; }

private:
    void saveCurrentInteralList() override;

    bool addNewGamesPlay(GamesPlay* play, bool checkItem = true);
};

#endif // GAMES_H
