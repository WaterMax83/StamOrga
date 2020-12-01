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

#include <QtCore/QJsonArray>
#include <QtCore/QJsonObject>

#include "../cstasettingsmanager.h"
#include "cdatagameuserdata.h"
#include "gameplay.h"

cDataGameUserData* g_DataGameUserData;

#define GROUP_FAVGAME "FavoriteGames"
#define VAL_GAME_INDEX "GameIndex"
#define VAL_FAV_INDEX "FavIndex"

cDataGameUserData::cDataGameUserData(QObject* parent)
    : QObject(parent)
{
    this->m_initialized = false;
}

cDataGameUserData::~cDataGameUserData()
{
    for (int i = 0; i < this->m_lFavGames.count(); i++)
        delete this->m_lFavGames.at(i);
    this->m_lFavGames.clear();
}

int cDataGameUserData::initialize()
{
    QMutexLocker lock(&m_mutex);

    //    qint32 currentSeason;
    //    QDate  date = QDate::currentDate();
    //    if (date.month() >= 6)
    //        currentSeason = date.year();
    //    else
    //        currentSeason = date.year() - 1;

    //    bool bSaveAgain = false;


    qint32 index = 0;
    qint64 iValue;
    while (g_StaSettingsManager->getInt64Value(GROUP_FAVGAME, VAL_GAME_INDEX, index, iValue) == ERROR_CODE_SUCCESS) {
        FavGameInfo* fGame = new FavGameInfo();
        fGame->m_gameIndex = iValue;
        g_StaSettingsManager->getInt64Value(GROUP_FAVGAME, VAL_FAV_INDEX, index, iValue);
        fGame->m_favIndex = iValue;

        //        /* remove games which are older */
        //        GamePlay* game = g_GlobalData->getGamePlay(fGame->m_gameIndex);
        //        if (game != nullptr) {
        //            qDebug() << game->seasonIndex();
        //            qDebug() << currentSeason;
        //            if (game->seasonIndex() < currentSeason) {
        //                bSaveAgain = true;
        //                continue;
        //            }
        //        }
        index++;

        this->m_lFavGames.append(fGame);
    }

    //    if (bSaveAgain) {
    //        this->m_settings->beginGroup(GROUP_FAVGAME);
    //        this->m_settings->remove("");
    //        this->m_settings->beginWriteArray("item");

    //        for (int i = 0; i < this->m_lFavGames.count(); i++) {
    //            this->m_settings->setArrayIndex(i);

    //            this->m_settings->setValue(VAL_GAME_INDEX, this->m_lFavGames.at(i)->m_gameIndex);
    //            this->m_settings->setValue(VAL_FAV_INDEX, this->m_lFavGames.at(i)->m_favIndex);
    //        }

    //        this->m_settings->endArray();
    //        this->m_settings->endGroup();
    //    }

    this->m_initialized = true;

    return ERROR_CODE_SUCCESS;
}

qint32 cDataGameUserData::clearUserData()
{
    if (!this->m_initialized)
        return ERROR_CODE_NOT_INITIALIZED;

    this->m_mutex.lock();

    g_StaSettingsManager->removeGroup(GROUP_FAVGAME);

    for (int i = 0; i < this->m_lFavGames.count(); i++)
        delete this->m_lFavGames.at(i);
    this->m_lFavGames.clear();

    this->m_mutex.unlock();

    this->clearTicketGameList();

    return ERROR_CODE_SUCCESS;
}


int cDataGameUserData::getFavoriteGameIndex(qint32 gameIndex)
{
    if (!this->m_initialized)
        return ERROR_CODE_NOT_INITIALIZED;

    return this->getGameIndex(&this->m_lFavGames, gameIndex);
}

int cDataGameUserData::setFavoriteGameIndex(qint32 gameIndex, qint32 favIndex)
{
    if (!this->m_initialized)
        return ERROR_CODE_NOT_INITIALIZED;

    return this->setGameIndex(&this->m_lFavGames, gameIndex, favIndex, true);
}

int cDataGameUserData::getTicketGameIndex(qint32 gameIndex)
{
    if (!this->m_initialized)
        return ERROR_CODE_NOT_INITIALIZED;

    return this->getGameIndex(&this->m_lTicketGames, gameIndex);
}

int cDataGameUserData::setTicketGameIndex(qint32 gameIndex, qint32 favIndex)
{
    if (!this->m_initialized)
        return ERROR_CODE_NOT_INITIALIZED;

    return this->setGameIndex(&this->m_lTicketGames, gameIndex, favIndex);
}

void cDataGameUserData::clearTicketGameList()
{
    QMutexLocker lock(&m_mutex);

    for (int i = 0; i < this->m_lTicketGames.count(); i++)
        delete this->m_lTicketGames.at(i);
    this->m_lTicketGames.clear();
}

int cDataGameUserData::getGameIndex(QList<FavGameInfo*>* pList, const qint32 gameIndex)
{
    QMutexLocker lock(&m_mutex);

    for (int i = 0; i < pList->count(); i++) {
        if (pList->at(i)->m_gameIndex == gameIndex)
            return pList->at(i)->m_favIndex;
    }

    return 0;
}

int cDataGameUserData::setGameIndex(QList<FavGameInfo*>* pList, const qint32 gameIndex,
                                    qint32 favIndex, bool writeToStorage)
{
    QMutexLocker lock(&m_mutex);

    for (int i = 0; i < pList->count(); i++) {
        if (pList->at(i)->m_gameIndex == gameIndex) {
            pList->at(i)->m_favIndex = favIndex;
            if (writeToStorage) {
                for (int i = 0; i < pList->size(); i++) {
                    qint64 gIndex;
                    g_StaSettingsManager->getInt64Value(GROUP_FAVGAME, VAL_GAME_INDEX, i, gIndex);
                    if (gIndex == gameIndex) {
                        g_StaSettingsManager->setInt64Value(GROUP_FAVGAME, VAL_FAV_INDEX, i, favIndex);
                        break;
                    }
                }
            }
            return 0;
        }
    }

    if (writeToStorage) {
        g_StaSettingsManager->setInt64Value(GROUP_FAVGAME, VAL_GAME_INDEX, pList->size(), gameIndex);
        g_StaSettingsManager->setInt64Value(GROUP_FAVGAME, VAL_FAV_INDEX, pList->size(), favIndex);
    }


    FavGameInfo* favGame = new FavGameInfo();
    favGame->m_gameIndex = gameIndex;
    favGame->m_favIndex  = favIndex;
    pList->append(favGame);

    return 0;
}

qint32 cDataGameUserData::handleUserPropTickets(QJsonArray& arrTickets)
{
    this->clearTicketGameList();

    if (arrTickets.isEmpty() || arrTickets.count() == 0)
        return ERROR_CODE_NOT_FOUND;

    for (int i = 0; i < arrTickets.count(); i++) {
        QJsonObject ticket    = arrTickets.at(i).toObject();
        qint32      gameIndex = ticket.value("gameIndex").toInt(-1);
        if (ticket.value("type").toString("") == "reserved")
            this->setTicketGameIndex(gameIndex, TICKET_STATE_RESERVED);
        else if (ticket.value("type").toString("") == "free")
            this->setTicketGameIndex(gameIndex, TICKET_STATE_FREE);
    }

    return ERROR_CODE_SUCCESS;
}


int cDataGameUserData::terminate()
{
    QMutexLocker lock(&m_mutex);

    this->m_initialized = false;

    for (int i = 0; i < this->m_lFavGames.count(); i++)
        delete this->m_lFavGames.at(i);
    this->m_lFavGames.clear();

    for (int i = 0; i < this->m_lTicketGames.count(); i++)
        delete this->m_lTicketGames.at(i);
    this->m_lTicketGames.clear();

    return 0;
}
