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
    QString          away;
    quint8           m_saisonIndex;
    CompetitionIndex m_competition;
    quint16          m_saison;
    QString          m_score;
};

#define PLAY_AWAY "away"
#define PLAY_SAISON_INDEX "sIndex"
#define PLAY_SAISON "saison"
#define PLAY_SCORE "score"
#define PLAY_COMPETITION "competition"

class Games : public ConfigList
{
public:
    Games();
    ~Games();

    int addNewGame(QString home, QString away, qint64 timestamp, quint8 sIndex, QString score, CompetitionIndex comp, quint16 season = 0);
    int showAllGames();


    GamesPlay* gameExists(quint8 sIndex, CompetitionIndex comp, quint16 saison, qint64 timestamp);

    void sortGamesListByTime();


private:
    void saveCurrentInteralList() override;

    bool addNewGamesPlay(QString home, QString away, qint64 timestamp, quint8 sIndex, QString score, CompetitionIndex comp, quint16 season, quint32 index, bool checkGame = true);
    void addNewGamesPlay(QString home, QString away, qint64 timestamp, quint8 sIndex, QString score, CompetitionIndex comp, quint16 season, quint32 index, QList<ConfigItem*>* pList);
};

#endif // GAMES_H
