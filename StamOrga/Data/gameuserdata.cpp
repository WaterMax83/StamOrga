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

#include "gameuserdata.h"
#include "gameplay.h"
#include "globaldata.h"

extern GlobalData* g_GlobalData;


#define GROUP_FAVGAME "FavoriteGames"
#define VAL_GAME_INDEX "GameIndex"
#define VAL_FAV_INDEX "FavIndex"

GameUserData::GameUserData(QObject* parent)
    : QObject(parent)
{
    this->m_initialized = false;
    this->m_settings    = NULL;
}

GameUserData::~GameUserData()
{
    if (this->m_settings != NULL)
        delete this->m_settings;

    for (int i = 0; i < this->m_lFavGames.count(); i++)
        delete this->m_lFavGames.at(i);
    this->m_lFavGames.clear();
}

int GameUserData::initialize()
{
    QMutexLocker lock(&m_mutex);

    this->m_settings = new QSettings();
    this->m_settings->setIniCodec(("UTF-8"));

    //    qint32 currentSeason;
    //    QDate  date = QDate::currentDate();
    //    if (date.month() >= 6)
    //        currentSeason = date.year();
    //    else
    //        currentSeason = date.year() - 1;

    //    bool bSaveAgain = false;
    this->m_settings->beginGroup(GROUP_FAVGAME);
    int count = this->m_settings->beginReadArray("item");
    for (int i = 0; i < count; i++) {
        this->m_settings->setArrayIndex(i);
        FavGameInfo* fGame = new FavGameInfo();
        fGame->m_gameIndex = this->m_settings->value(VAL_GAME_INDEX).toInt();
        fGame->m_favIndex  = this->m_settings->value(VAL_FAV_INDEX).toInt();

        //        /* remove games which are older */
        //        GamePlay* game = g_GlobalData->getGamePlay(fGame->m_gameIndex);
        //        if (game != NULL) {
        //            qDebug() << game->seasonIndex();
        //            qDebug() << currentSeason;
        //            if (game->seasonIndex() < currentSeason) {
        //                bSaveAgain = true;
        //                continue;
        //            }
        //        }

        this->m_lFavGames.append(fGame);
    }

    this->m_settings->endArray();
    this->m_settings->endGroup();

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

    return 0;
}


int GameUserData::getFavoriteGameIndex(qint32 gameIndex)
{
    if (!this->m_initialized)
        return -1;

    return this->getGameIndex(&this->m_lFavGames, gameIndex);
}

int GameUserData::setFavoriteGameIndex(qint32 gameIndex, qint32 favIndex)
{
    if (!this->m_initialized)
        return -1;

    return this->setGameIndex(&this->m_lFavGames, gameIndex, favIndex, true);
}

int GameUserData::getTicketGameIndex(qint32 gameIndex)
{
    if (!this->m_initialized)
        return -1;

    return this->getGameIndex(&this->m_lTicketGames, gameIndex);
}

int GameUserData::setTicketGameIndex(qint32 gameIndex, qint32 favIndex)
{
    if (!this->m_initialized)
        return -1;

    return this->setGameIndex(&this->m_lTicketGames, gameIndex, favIndex);
}

void GameUserData::clearTicketGameList()
{
    QMutexLocker lock(&m_mutex);

    for (int i = 0; i < this->m_lTicketGames.count(); i++)
        delete this->m_lTicketGames.at(i);
    this->m_lTicketGames.clear();
}

int GameUserData::getGameIndex(QList<FavGameInfo*>* pList, const qint32 gameIndex)
{
    QMutexLocker lock(&m_mutex);

    for (int i = 0; i < pList->count(); i++) {
        if (pList->at(i)->m_gameIndex == gameIndex)
            return pList->at(i)->m_favIndex;
    }

    return 0;
}

int GameUserData::setGameIndex(QList<FavGameInfo*>* pList, const qint32 gameIndex,
                               qint32 favIndex, bool writeToStorage)
{
    QMutexLocker lock(&m_mutex);

    for (int i = 0; i < pList->count(); i++) {
        if (pList->at(i)->m_gameIndex == gameIndex) {
            pList->at(i)->m_favIndex = favIndex;
            if (writeToStorage) {
                this->m_settings->beginGroup(GROUP_FAVGAME);
                int count = this->m_settings->beginReadArray("item");
                for (int i = 0; i < count; i++) {
                    this->m_settings->setArrayIndex(i);
                    qint32 gIndex = this->m_settings->value(VAL_GAME_INDEX).toInt();
                    if (gIndex == gameIndex) {
                        this->m_settings->setValue(VAL_FAV_INDEX, favIndex);
                        break;
                    }
                }
                this->m_settings->endArray();
                this->m_settings->endGroup();
            }
            return 0;
        }
    }

    if (writeToStorage) {
        this->m_settings->beginGroup(GROUP_FAVGAME);
        this->m_settings->beginWriteArray("item");
        this->m_settings->setArrayIndex(pList->count());

        this->m_settings->setValue(VAL_GAME_INDEX, gameIndex);
        this->m_settings->setValue(VAL_FAV_INDEX, favIndex);

        this->m_settings->endArray();
        this->m_settings->endGroup();
        this->m_settings->sync();
    }


    FavGameInfo* favGame = new FavGameInfo();
    favGame->m_gameIndex = gameIndex;
    favGame->m_favIndex  = favIndex;
    pList->append(favGame);

    return 0;
}


int GameUserData::terminate()
{
    QMutexLocker lock(&m_mutex);

    this->m_initialized = false;

    if (this->m_settings != NULL)
        delete this->m_settings;
    this->m_settings = NULL;

    for (int i = 0; i < this->m_lFavGames.count(); i++)
        delete this->m_lFavGames.at(i);
    this->m_lFavGames.clear();

    for (int i = 0; i < this->m_lTicketGames.count(); i++)
        delete this->m_lTicketGames.at(i);
    this->m_lTicketGames.clear();

    return 0;
}
