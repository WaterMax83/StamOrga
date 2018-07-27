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

#ifndef CDATAGAMESMANAGER_H
#define CDATAGAMESMANAGER_H

#include <QObject>
#include <QtCore/QList>
#include <QtCore/QMutex>

#include "../Common/General/cgendisposer.h"
#include "../Common/Network/messageprotocol.h"
#include "gameplay.h"

class cDataGamesManager : public cGenDisposer
{
    Q_OBJECT
public:
    explicit cDataGamesManager(QObject* parent = nullptr);

    qint32 initialize();

    void addNewGamesPlay(GamePlay* sGame, const quint16 updateIndex = 0);

    GamePlay* getGamePlay(qint32 gameIndex);

    bool setGamePlayItemHasEvent(qint32 gameIndex);
    void resetAllGamePlayEvents();

    Q_INVOKABLE qint32 getGamePlayLength();
    Q_INVOKABLE GamePlay* getGamePlayFromArrayIndex(int index);
    Q_INVOKABLE QString getGamePlayLastLocalUpdateString();

    Q_INVOKABLE qint32 startListGames(qint32 pastGames = -1);
    qint32             handleListGamesResponse(MessageProtocol* msg);

    Q_INVOKABLE qint32 startListGamesInfo();
    qint32             handleListGamesInfoResponse(MessageProtocol* msg);

    Q_INVOKABLE qint32 startChangeGame(const qint32 index, const qint32 sIndex, const QString competition,
                                       const QString home, const QString away, const QString date,
                                       const QString score, const bool fixedTime);
    qint32             handleChangeGameResponse(MessageProtocol* msg);

    Q_INVOKABLE bool getSkipedOldGames() { return this->m_bSkipedOldGames; }

    qint32 stateChangeCheckUdpate();

signals:
    void sendAppStateChangedToActive(qint32 state);

public slots:

private:
    QList<GamePlay*> m_lGames;
    QMutex           m_mutex;

    qint64 m_stLastLocalUpdateTimeStamp;
    qint64 m_stLastServerUpdateTimeStamp;
    qint64 m_LastGameInfoUpdate;
    bool   m_bSkipedOldGames;
    bool   m_bLoadedGamesAfterRestart;
};

extern cDataGamesManager* g_DataGamesManager;

#endif // CDATAGAMESMANAGER_H
