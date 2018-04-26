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

#ifndef FAVORITEGAME_H
#define FAVORITEGAME_H

#include <QList>
#include <QMutex>
#include <QObject>
#include <QSettings>

struct FavGameInfo {
    qint32 m_gameIndex;
    qint32 m_favIndex;
};

class cDataGameUserData : public QObject
{
    Q_OBJECT
public:
    explicit cDataGameUserData(QObject* parent = nullptr);
    ~cDataGameUserData();

    int initialize();

    int terminate();

    Q_INVOKABLE int getFavoriteGameIndex(qint32 gameIndex);
    Q_INVOKABLE int setFavoriteGameIndex(qint32 gameIndex, qint32 favIndex);

    Q_INVOKABLE int getTicketGameIndex(qint32 gameIndex);
    int setTicketGameIndex(qint32 gameIndex, qint32 favIndex);


    qint32 handleUserPropTickets(QJsonArray& arrTickets);

signals:

public slots:

private:
    bool                m_initialized;
    QList<FavGameInfo*> m_lFavGames;
    QList<FavGameInfo*> m_lTicketGames;
    QMutex              m_mutex;

    void clearTicketGameList();
    int getGameIndex(QList<FavGameInfo*>* pList, const qint32 gameIndex);
    int setGameIndex(QList<FavGameInfo*>* pList, const qint32 gameIndex, qint32 favIndex, bool writeToStorage = false);
};

extern cDataGameUserData* g_DataGameUserData;

#endif // FAVORITEGAME_H
