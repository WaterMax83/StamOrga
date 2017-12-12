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
    quint8           m_saisonIndex;
    CompetitionIndex m_competition;
    quint16          m_saison;
    QString          m_score;
    qint64           m_lastUpdate;
    quint32          m_scheduled;

    GamesPlay(QString home, QString away, qint64 timestamp,
              quint8 sIndex, QString score, CompetitionIndex comp,
              quint16 saison, quint32 index, qint64 lastUpdate,
              quint32 scheduled)
    {
        this->m_itemName  = home;
        this->m_index     = index;
        this->m_timestamp = timestamp;

        this->m_away        = away;
        this->m_saisonIndex = sIndex;
        this->m_score       = score;
        this->m_competition = comp;
        this->m_saison      = saison;
        this->m_lastUpdate  = lastUpdate;
        this->m_scheduled   = scheduled;
    }
};

// clang-format off
#define PLAY_AWAY           "away"
#define PLAY_SAISON_INDEX   "sIndex"
#define PLAY_SAISON         "saison"
#define PLAY_SCORE          "score"
#define PLAY_COMPETITION    "competition"
#define PLAY_LAST_UDPATE    "lastUpdate"
#define PLAY_SCHEDULED      "scheduled"
// clang-format on

class Games : public ConfigList
{
public:
    Games();
    ~Games();

    int addNewGame(QString home, QString away, qint64 timestamp,
                   quint8 sIndex, QString score, CompetitionIndex comp,
                   quint16 season = 0, qint64 lastUpdate = 0);
    int showAllGames();

    int changeScheduledValue(const quint32 gameIndex, const quint32 fixedTime);


    GamesPlay* gameExists(quint8 sIndex, CompetitionIndex comp, quint16 saison, qint64 timestamp);

    virtual qint32 checkConsistency() { return -12; }

private:
    void saveCurrentInteralList() override;

    bool addNewGamesPlay(GamesPlay* play, bool checkItem = true);
};

#endif // GAMES_H
