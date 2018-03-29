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
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>
#include <QtQml/QQmlEngine>

#include "cdatagamesmanager.h"

#include "../Common/General/globalfunctions.h"
#include "../Common/General/globaltiming.h"
#include "../Common/Network/messagecommand.h"
#include "../Connection/cconmanager.h"
#include "../Connection/cconusersettings.h"
#include "../cstaglobalsettings.h"
#include "../cstasettingsmanager.h"


// clang-format off

#define ITEM_INDEX          "index"

#define GAMES_GROUP         "GAMES_LIST"
#define PLAY_HOME           "home"
#define PLAY_AWAY           "away"
#define PLAY_DATETIME       "datetime"
#define PLAY_SAISON_INDEX   "sIndex"
#define PLAY_SCORE          "score"
#define PLAY_COMPETITION    "competition"
#define PLAY_TIME_FIXED     "timeFixed"

#define LOCAL_GAMES_UDPATE "LocalGamesUpdateTime"
#define SERVER_GAMES_UDPATE "ServerGamesUpdateTime"

// clang-format on

cDataGamesManager g_DataGamesManager;

cDataGamesManager::cDataGamesManager(QObject* parent)
    : cGenDisposer(parent)
{
}

qint32 cDataGamesManager::initialize()
{
    if (!g_StaGlobalSettings.getSaveInfosOnApp())
        g_StaSettingsManager.removeGroup(GAMES_GROUP);

    qint64 iValue;
    g_StaSettingsManager.getInt64Value(GAMES_GROUP, LOCAL_GAMES_UDPATE, iValue);
    this->m_stLastLocalUpdateTimeStamp = iValue;
    g_StaSettingsManager.getInt64Value(GAMES_GROUP, SERVER_GAMES_UDPATE, iValue);
    this->m_stLastServerUpdateTimeStamp = iValue;

    this->m_initialized = true;

    QString value;
    bool    bValue;
    qint32  index = 0;
    while (g_StaSettingsManager.getValue(GAMES_GROUP, PLAY_HOME, index, value) == ERROR_CODE_SUCCESS) {
        GamePlay* pGame = new GamePlay();
        pGame->setHome(value);
        g_StaSettingsManager.getValue(GAMES_GROUP, PLAY_AWAY, index, value);
        pGame->setAway(value);
        g_StaSettingsManager.getInt64Value(GAMES_GROUP, PLAY_DATETIME, index, iValue);
        pGame->setTimeStamp(iValue);
        g_StaSettingsManager.getInt64Value(GAMES_GROUP, ITEM_INDEX, index, iValue);
        pGame->setIndex(iValue);
        g_StaSettingsManager.getInt64Value(GAMES_GROUP, PLAY_SAISON_INDEX, index, iValue);
        pGame->setSeasonIndex(iValue);
        g_StaSettingsManager.getValue(GAMES_GROUP, PLAY_SCORE, index, value);
        pGame->setScore(value);
        g_StaSettingsManager.getInt64Value(GAMES_GROUP, PLAY_COMPETITION, index, iValue);
        pGame->setCompetition((CompetitionIndex)iValue);
        g_StaSettingsManager.getBoolValue(GAMES_GROUP, PLAY_TIME_FIXED, index, bValue);
        pGame->setTimeFixed(bValue);

        QQmlEngine::setObjectOwnership(pGame, QQmlEngine::CppOwnership);
        this->addNewGamesPlay(pGame);
        index++;
    }

    return ERROR_CODE_SUCCESS;
}


void cDataGamesManager::addNewGamesPlay(GamePlay* sGame, const quint16 updateIndex)
{
    if (!this->m_initialized)
        return;

    GamePlay* pGame = this->getGamePlay(sGame->index());
    if (pGame == NULL) {
        QMutexLocker lock(&this->m_mutex);

        //        gPlay->setEnableAddGame(this->userIsGameAddingEnabled());
        this->m_lGames.append(sGame);
        return;
    } else if (updateIndex == UpdateIndex::UpdateDiff) {
        if (pGame->home() != sGame->home()) {
            pGame->setHome(sGame->home());
        }
        if (pGame->away() != sGame->away()) {
            pGame->setAway(sGame->away());
        }
        if (pGame->score() != sGame->score()) {
            pGame->setScore(sGame->score());
        }
        if (pGame->timestamp64Bit() != sGame->timestamp64Bit()) {
            pGame->setTimeStamp(sGame->timestamp64Bit());
        }
        if (pGame->seasonIndex() != sGame->seasonIndex()) {
            pGame->setSeasonIndex(sGame->seasonIndex());
        }
        if (pGame->competitionValue() != sGame->competitionValue()) {
            pGame->setCompetition((CompetitionIndex)sGame->competitionValue());
        }
        if (pGame->timeFixed() != sGame->timeFixed()) {
            pGame->setTimeFixed(sGame->timeFixed());
        }
        delete sGame;
    }
}

GamePlay* cDataGamesManager::getGamePlay(qint32 gameIndex)
{
    if (!this->m_initialized)
        return NULL;

    QMutexLocker lock(&this->m_mutex);

    for (int i = 0; i < this->m_lGames.size(); i++) {
        if (this->m_lGames[i]->index() == gameIndex)
            return this->m_lGames[i];
    }
    return NULL;
}

qint32 cDataGamesManager::getGamePlayLength()
{
    QMutexLocker lock(&this->m_mutex);
    return this->m_lGames.size();
}

GamePlay* cDataGamesManager::getGamePlayFromArrayIndex(int index)
{
    QMutexLocker lock(&this->m_mutex);

    if (index < this->m_lGames.size()) {
        return this->m_lGames.at(index);
    }
    return NULL;
}

QString cDataGamesManager::getGamePlayLastLocalUpdateString()
{
    if (!this->m_initialized)
        return "";

    QMutexLocker lock(&this->m_mutex);
    return QDateTime::fromMSecsSinceEpoch(this->m_stLastLocalUpdateTimeStamp).toString("dd.MM.yy hh:mm:ss");
}

//qint32 cDataGamesManager::startListSeasonTickets()
//{
//    if (!this->m_initialized)
//        return ERROR_CODE_NOT_INITIALIZED;

//    QMutexLocker lock(&this->m_mutex);

//    QJsonObject rootObj;
//    if (this->m_stLastLocalUpdateTimeStamp + TIMEOUT_UPDATE_TICKETS > QDateTime::currentMSecsSinceEpoch() && this->m_lTickets.count() > 0)
//        rootObj.insert("index", UpdateIndex::UpdateDiff);
//    else
//        rootObj.insert("index", UpdateIndex::UpdateAll);

//    rootObj.insert("timestamp", this->m_stLastServerUpdateTimeStamp);

//    TcpDataConRequest* req = new TcpDataConRequest(OP_CODE_CMD_REQ::REQ_GET_TICKETS_LIST);
//    req->m_lData           = QJsonDocument(rootObj).toJson(QJsonDocument::Compact);

//    g_ConManager.sendNewRequest(req);
//    return ERROR_CODE_SUCCESS;
//}

//qint32 cDataGamesManager::handleListSeasonTicketsResponse(MessageProtocol* msg)
//{
//    if (!this->m_initialized)
//        return ERROR_CODE_NOT_INITIALIZED;

//    QByteArray  data(msg->getPointerToData());
//    QJsonObject rootObj = QJsonDocument::fromJson(data).object();

//    qint32     result      = rootObj.value("ack").toInt(ERROR_CODE_NOT_FOUND);
//    qint32     updateIndex = rootObj.value("index").toInt(UpdateAll);
//    qint64     timestamp   = (qint64)rootObj.value("timestamp").toDouble(0);
//    QJsonArray arrTickets  = rootObj.value("tickets").toArray();

//    this->m_mutex.lock();

//    if (updateIndex == UpdateIndex::UpdateAll) {
//        for (int i = 0; i < this->m_lTickets.size(); i++)
//            delete this->m_lTickets[i];
//        this->m_lTickets.clear();
//    }
//    this->m_stLastLocalUpdateTimeStamp = QDateTime::currentMSecsSinceEpoch();

//    this->m_mutex.unlock();

//    for (int i = 0; i < arrTickets.count(); i++) {
//        QJsonObject       ticketObj = arrTickets.at(i).toObject();
//        SeasonTicketItem* pTicket   = new SeasonTicketItem();

//        pTicket->setDiscount(ticketObj.value("discount").toInt());
//        pTicket->setIndex(ticketObj.value("index").toInt());
//        pTicket->setUserIndex(ticketObj.value("userIndex").toInt());
//        pTicket->setName(ticketObj.value("name").toString());
//        pTicket->setPlace(ticketObj.value("place").toString());
//        pTicket->checkTicketOwn(g_ConUserSettings.getUserIndex());

//        QQmlEngine::setObjectOwnership(pTicket, QQmlEngine::CppOwnership);
//        this->addNewSeasonTicket(pTicket, updateIndex);
//    }

//    QMutexLocker lock(&this->m_mutex);

//    if (timestamp == this->m_stLastServerUpdateTimeStamp && arrTickets.count() == 0 && updateIndex != UpdateAll) {
//        if (!g_StaGlobalSettings.getSaveInfosOnApp())
//            return result;

//        g_StaSettingsManager.setInt64Value(GAMES_GROUP, LOCAL_GAMES_UDPATE, this->m_stLastLocalUpdateTimeStamp);
//        return result;
//    }

//    this->m_stLastServerUpdateTimeStamp = timestamp;

//    if (!g_StaGlobalSettings.getSaveInfosOnApp())
//        return result;

//    g_StaSettingsManager.setInt64Value(GAMES_GROUP, LOCAL_GAMES_UDPATE, this->m_stLastLocalUpdateTimeStamp);
//    g_StaSettingsManager.setInt64Value(GAMES_GROUP, SERVER_GAMES_UDPATE, this->m_stLastServerUpdateTimeStamp);

//    for (int i = 0; i < this->m_lTickets.size(); i++) {
//        g_StaSettingsManager.setValue(GAMES_GROUP, TICKET_NAME, i, this->m_lTickets[i]->name());
//        g_StaSettingsManager.setValue(GAMES_GROUP, TICKET_PLACE, i, this->m_lTickets[i]->place());
//        g_StaSettingsManager.setInt64Value(GAMES_GROUP, TICKET_DISCOUNT, i, this->m_lTickets[i]->discount());
//        g_StaSettingsManager.setInt64Value(GAMES_GROUP, TICKET_USER_INDEX, i, this->m_lTickets[i]->userIndex());
//        g_StaSettingsManager.setInt64Value(GAMES_GROUP, ITEM_INDEX, i, this->m_lTickets[i]->index());
//    }

//    return result;
//}


//qint32 cDataGamesManager::startAddSeasonTicket(const qint32 index, const QString name, const QString place, const qint32 discount)
//{
//    if (!this->m_initialized)
//        return ERROR_CODE_NOT_INITIALIZED;

//    QJsonObject rootObj;
//    rootObj.insert("index", index);
//    rootObj.insert("name", name);
//    rootObj.insert("place", place);
//    rootObj.insert("discount", discount);

//    TcpDataConRequest* req = new TcpDataConRequest(OP_CODE_CMD_REQ::REQ_ADD_TICKET);
//    req->m_lData           = QJsonDocument(rootObj).toJson(QJsonDocument::Compact);

//    g_ConManager.sendNewRequest(req);
//    return ERROR_CODE_SUCCESS;
//}

//qint32 cDataGamesManager::handleAddSeasonTicketResponse(MessageProtocol* msg)
//{
//    if (!this->m_initialized)
//        return ERROR_CODE_NOT_INITIALIZED;

//    return msg->getIntData();
//}

//qint32 cDataGamesManager::startRemoveSeasonTicket(const qint32 index)
//{
//    if (!this->m_initialized)
//        return ERROR_CODE_NOT_INITIALIZED;

//    QJsonObject rootObj;
//    rootObj.insert("index", index);

//    TcpDataConRequest* req = new TcpDataConRequest(OP_CODE_CMD_REQ::REQ_REMOVE_TICKET);
//    req->m_lData           = QJsonDocument(rootObj).toJson(QJsonDocument::Compact);

//    g_ConManager.sendNewRequest(req);
//    return ERROR_CODE_SUCCESS;
//}

//qint32 cDataGamesManager::handleRemoveSeasonTicketResponse(MessageProtocol* msg)
//{
//    if (!this->m_initialized)
//        return ERROR_CODE_NOT_INITIALIZED;

//    QMutexLocker lock(&this->m_mutex);

//    this->m_stLastServerUpdateTimeStamp = 0;

//    return msg->getIntData();
//}
