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

#include <QtCore/QDate>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>

#include "../Common/General/globalfunctions.h"
#include "../Common/General/globaltiming.h"
#include "../Common/Network/messagecommand.h"
#include "../General/globaldata.h"
#include "cgamesmanager.h"

extern GlobalData* g_GlobalData;

cGamesManager g_GamesManager;

cGamesManager::cGamesManager(QObject* parent)
    : cGenDisposer(parent)
{
}


qint32 cGamesManager::initialize()
{
    this->m_initialized = true;

    return ERROR_CODE_SUCCESS;
}

MessageProtocol* cGamesManager::getGamesList(UserConData* pUserCon, MessageProtocol* request)
{
    if (!this->m_initialized)
        return NULL;

    QByteArray  data    = QByteArray(request->getPointerToData());
    QJsonObject rootObj = QJsonDocument::fromJson(data).object();

    qint32 updateIndex            = rootObj.value("index").toInt(UpdateIndex::UpdateAll);
    qint64 lastUpdateGamesFromApp = (qint64)rootObj.value("timestamp").toDouble(0);
    qint32 loadGameInPastCount    = rootObj.value("pastGames").toInt(-1);
    bool   bSkipDiffForPast       = rootObj.value("skipDiffForPast").toBool(false);

    if (lastUpdateGamesFromApp == 0)
        updateIndex = UpdateIndex::UpdateAll;

    QJsonObject rootAns;
    rootAns.insert("ack", ERROR_CODE_SUCCESS);
    rootAns.insert("index", updateIndex);

    quint16 numbOfGames = g_GlobalData->m_GamesList.getNumberOfInternalList();

    qint64     currentTimeStamp = QDateTime::currentMSecsSinceEpoch() + 2 * MSEC_PER_HOUR;
    qint32     gameInPastCount  = 0;
    bool       bSkipedOldGames  = false;
    QJsonArray arrGames;
    for (quint32 i = 0; i < numbOfGames; i++) {
        GamesPlay* pGame = (GamesPlay*)(g_GlobalData->m_GamesList.getRequestConfigItemFromListIndex(i));
        if (pGame == NULL)
            continue;
        if (pGame->m_timestamp < currentTimeStamp)
            gameInPastCount++;
        if (loadGameInPastCount > 0 && gameInPastCount > loadGameInPastCount) {
            bSkipedOldGames = true;
            continue;
        }

        if (!(bSkipDiffForPast && pGame->m_timestamp < currentTimeStamp)
            && updateIndex == UpdateIndex::UpdateDiff && pGame->m_lastUpdate <= lastUpdateGamesFromApp)
            continue; // Skip ticket because user already has all info

        if (pGame->m_competition == CompetitionIndex::ONLY_MEETING && request->getVersion() < MSG_HEADER_ADD_FANCLUB_MEETING)
            continue;

        QJsonObject gameObj;
        gameObj.insert("home", pGame->m_itemName);
        gameObj.insert("away", pGame->m_away);
        gameObj.insert("score", pGame->m_score);
        gameObj.insert("index", pGame->m_index);
        gameObj.insert("timestamp", pGame->m_timestamp);
        gameObj.insert("competition", pGame->m_competition);
        gameObj.insert("seasonIndex", pGame->m_seasonIndex);
        gameObj.insert("season", pGame->m_season);
        if (request->getVersion() < MSG_HEADER_ADD_FANCLUB_MEETING)
            gameObj.insert("fixed", IS_PLAY_FIXED(pGame->m_options));
        else
            gameObj.insert("option", pGame->m_options);

        arrGames.append(gameObj);
    }

    rootAns.insert("skipOldGames", bSkipedOldGames);
    rootAns.insert("timestamp", g_GlobalData->m_GamesList.getLastUpdateTime());
    rootAns.insert("games", arrGames);

    QByteArray answer = QJsonDocument(rootAns).toJson(QJsonDocument::Compact);

    qInfo().noquote() << QString("User %1 request Games List with %2 entries").arg(pUserCon->m_userName).arg(arrGames.size());

    return new MessageProtocol(OP_CODE_CMD_RES::ACK_GET_GAMES_LIST, answer);
}

MessageProtocol* cGamesManager::getGamesInfoList(UserConData* pUserCon, MessageProtocol* request)
{
    if (!this->m_initialized)
        return NULL;

    QByteArray  data    = QByteArray(request->getPointerToData());
    QJsonObject rootObj = QJsonDocument::fromJson(data).object();

    qint64 lastUpdateGamesFromApp = (qint64)rootObj.value("timestamp").toDouble(0);

    QJsonObject rootAns;
    if (g_GlobalData->m_GamesList.getLastUpdateTime() > lastUpdateGamesFromApp) {
        rootAns.insert("ack", ERROR_CODE_UPDATE_LIST);
    } else {
        rootAns.insert("ack", ERROR_CODE_SUCCESS);

        qint32 numbOfGames = g_GlobalData->m_GamesList.getNumberOfInternalList();
        //    #ifndef QT_DEBUG
        qint64 currentTime = QDateTime::currentMSecsSinceEpoch();
        //    #endif
        QJsonArray gameInfoArr;
        for (qint32 i = 0; i < numbOfGames; i++) {
            GamesPlay* pGame = (GamesPlay*)(g_GlobalData->m_GamesList.getRequestConfigItemFromListIndex(i));
            if (pGame == NULL)
                continue;
            //    #ifndef QT_DEBUG
            if (pGame->m_timestamp + 2 * MSEC_PER_HOUR < currentTime)
                continue;
            //    #endif
            if (pGame->m_competition == CompetitionIndex::ONLY_MEETING && request->getVersion() < MSG_HEADER_ADD_FANCLUB_MEETING)
                continue;

            qint16 freeTickets     = g_GlobalData->getTicketNumber(pGame->m_index, TICKET_STATE_FREE);
            qint16 blockTickets    = g_GlobalData->getTicketNumber(pGame->m_index, TICKET_STATE_BLOCKED);
            qint16 reservedTickets = g_GlobalData->getTicketNumber(pGame->m_index, TICKET_STATE_RESERVED);
            qint16 acceptedMeeting = g_GlobalData->getAcceptedNumber(MEETING_TYPE_MEETING, pGame->m_index, ACCEPT_STATE_ACCEPT);
            qint16 interestMeeting = g_GlobalData->getAcceptedNumber(MEETING_TYPE_MEETING, pGame->m_index, ACCEPT_STATE_MAYBE);
            qint16 declinedMeeting = g_GlobalData->getAcceptedNumber(MEETING_TYPE_MEETING, pGame->m_index, ACCEPT_STATE_DECLINE);
            qint16 meetInfo        = g_GlobalData->getMeetingInfoValue(MEETING_TYPE_MEETING, pGame->m_index);
            qint16 acceptedTrip    = g_GlobalData->getAcceptedNumber(MEETING_TYPE_AWAYTRIP, pGame->m_index, ACCEPT_STATE_ACCEPT);
            qint16 interestTrip    = g_GlobalData->getAcceptedNumber(MEETING_TYPE_AWAYTRIP, pGame->m_index, ACCEPT_STATE_MAYBE);
            qint16 declinedTrip    = g_GlobalData->getAcceptedNumber(MEETING_TYPE_AWAYTRIP, pGame->m_index, ACCEPT_STATE_DECLINE);
            qint16 tripInfo        = g_GlobalData->getMeetingInfoValue(MEETING_TYPE_AWAYTRIP, pGame->m_index);

            if (freeTickets == 0 && reservedTickets == 0 && acceptedMeeting == 0 && interestMeeting == 0 && declinedMeeting == 0 && meetInfo == 0 && acceptedTrip == 0 && interestTrip == 0 && declinedTrip == 0 && tripInfo == 0)
                continue;

            QJsonObject infoObj;
            infoObj.insert("index", pGame->m_index);
            infoObj.insert("free", freeTickets);
            infoObj.insert("blocked", blockTickets);
            infoObj.insert("reserved", reservedTickets);
            QJsonObject meet;
            meet.insert("accepted", acceptedMeeting);
            meet.insert("interest", interestMeeting);
            meet.insert("declined", declinedMeeting);
            meet.insert("info", meetInfo);
            QJsonObject trip;
            trip.insert("accepted", acceptedTrip);
            trip.insert("interest", interestTrip);
            trip.insert("declined", declinedTrip);
            trip.insert("info", tripInfo);
            infoObj.insert("meet", meet);
            infoObj.insert("trip", trip);

            gameInfoArr.append(infoObj);
        }
        rootAns.insert("infos", gameInfoArr);

        qInfo().noquote() << QString("User %1 request Games Info List").arg(pUserCon->m_userName);
    }
    QByteArray answer = QJsonDocument(rootAns).toJson(QJsonDocument::Compact);

    return new MessageProtocol(OP_CODE_CMD_RES::ACK_GET_GAMES_INFO_LIST, answer);
}

MessageProtocol* cGamesManager::getChangeGameRequest(UserConData* pUserCon, MessageProtocol* request)
{
    if (!this->m_initialized)
        return NULL;

    if (request->getVersion() < MSG_HEADER_ADD_FANCLUB_MEETING)
        return new MessageProtocol(OP_CODE_CMD_RES::ACK_CHANGE_GAME_TCP, ERROR_CODE_UPDATE_APP);

    QByteArray  data    = QByteArray(request->getPointerToData());
    QJsonObject rootObj = QJsonDocument::fromJson(data).object();

    qint32           index     = rootObj.value("index").toInt(-1);
    QString          home      = rootObj.value("home").toString();
    QString          away      = rootObj.value("away").toString();
    QString          score     = rootObj.value("score").toString();
    CompetitionIndex comp      = (CompetitionIndex)rootObj.value("competition").toInt();
    qint32           sIndex    = rootObj.value("seasonIndex").toInt();
    qint64           timestamp = (qint64)rootObj.value("timestamp").toDouble();
    qint32           option    = rootObj.value("option").toInt();

    /* game already exists, should only be changed */
    if (index > 0) {
        quint16    saison = getSeasonFromTimeStamp(timestamp);
        GamesPlay* pGame  = g_GlobalData->m_GamesList.gameExists(sIndex, comp, saison, timestamp);
        if (pGame == NULL) {
            qWarning().noquote() << QString("user %1 tried to change game %2, but game would be added, abort it").arg(pUserCon->m_userName).arg(index);
            return new MessageProtocol(OP_CODE_CMD_RES::ACK_CHANGE_GAME_TCP, ERROR_CODE_NOT_FOUND);
        }
    }

    qint32 result = g_GlobalData->m_GamesList.addNewGame(home, away, timestamp, sIndex, score, comp);
    if (result < 0)
        return new MessageProtocol(OP_CODE_CMD_RES::ACK_CHANGE_GAME_TCP, result);
    if (index > 0 && index != result) {
        qWarning().noquote() << QString("user %1 tried to changed game %2, but added game %3").arg(pUserCon->m_userName).arg(index).arg(result);
    }
    result = g_GlobalData->m_GamesList.changeOptionValue(result, option);

    return new MessageProtocol(OP_CODE_CMD_RES::ACK_CHANGE_GAME_TCP, result);
}
