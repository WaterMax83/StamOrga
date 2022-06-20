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

#include <QtCore/QDateTime>
#include <QtCore/QDebug>
#include <QtCore/QFile>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonValue>

#include "../Common/General/globalfunctions.h"
#include "../General/globaldata.h"
#include "readonlinegames.h"

#define GETMATCH_DATA "https://www.openligadb.de/api/getmatchdata"

#define MINUTE_IN_MSEC 60 * qint64(1000)
#define HOUR_IN_MSEC 60 * 60 * qint64(1000)

extern GlobalData* g_GlobalData;

ReadOnlineGames::ReadOnlineGames(QObject* parent)
    : BackgroundWorker(parent)
{
    this->m_networkTimout = NULL;
    this->m_networkUpdate = NULL;
}

ReadOnlineGames::~ReadOnlineGames()
{
}

qint32 ReadOnlineGames::initialize(QString comp, qint32 max, qint32 season)
{
    this->m_currentRequestIndex = -1;
    this->m_comp                = comp;
    this->m_maxIndex            = max;
    this->m_season              = season;
    this->m_timeOutCounter      = 0;

    this->m_initialized = true;

    return ERROR_CODE_SUCCESS;
}

int ReadOnlineGames::DoBackgroundWork()
{
    this->m_currentRequestIndex = -1;

    //    OnlineGamesRequestList* list1 = new OnlineGamesRequestList();
    //    list1->m_comp      = "bl3";
    //    list1->m_maxIndex  = 38;
    //    list1->m_season    = 2017;
    //    this->m_requestList.append(list1);

    //    OnlineGamesRequestList* list2 = new OnlineGamesRequestList();
    //    list2->m_comp      = "dfb2017";
    //    list2->m_maxIndex  = 1;
    //    list2->m_season    = 2017;
    //    this->m_requestList.append(list2);

    this->m_netAccess = new QNetworkAccessManager();
    connect(this->m_netAccess, &QNetworkAccessManager::finished, this, &ReadOnlineGames::slotNetWorkRequestFinished);

    this->m_networkTimout = new QTimer(this);
    this->m_networkTimout->setSingleShot(true);
    this->m_networkTimout->setInterval(10000);
    connect(this->m_networkTimout, &QTimer::timeout, this, &ReadOnlineGames::slotNetWorkRequestTimeout);

    this->m_networkUpdate = new QTimer(this);
    this->m_networkUpdate->setSingleShot(true);
    connect(this->m_networkUpdate, &QTimer::timeout, this, &ReadOnlineGames::slotNetWorkUpdateTimeout);

    //    connect(this, &ReadOnlineGames::signalFinishThreadSafe, this, &ReadOnlineGames::slotFinishThreadSafe);

#ifdef QT_DEBUG
    qInfo().noquote() << "Did not use ReadOnlineGame because of debugging";
    return 0;
#endif

    qInfo().noquote() << QString("Start new read online game info for \"%1\" with %2 entries").arg(this->m_comp).arg(this->m_maxIndex);
    this->checkNewNetworkRequest(false);

    return 0;
}

qint32 ReadOnlineGames::getTotalCountOfRequest()
{
    //    qint32 rValue = 0;
    //    foreach (OnlineGamesRequestList* request, this->m_requestList)
    //        rValue += request->m_maxIndex;

    return this->m_maxIndex;
}

OnlineGameInfo* ReadOnlineGames::getNextRequest(OnlineGameInfo* currentGame)
{
    //    if (this->m_requestList.size() == 0)
    //        return NULL;

    if (currentGame == NULL) {
        OnlineGameInfo* info = new OnlineGameInfo();
        info->m_competition  = this->m_comp;
        info->m_season       = this->m_season;
        info->m_index        = 1;
        return info;
    }

    if (currentGame->m_index < this->m_maxIndex) {
        OnlineGameInfo* info = new OnlineGameInfo();
        info->m_competition  = this->m_comp;
        info->m_season       = this->m_season;
        info->m_index        = currentGame->m_index + 1;
        return info;
    }

    return NULL;
}

void ReadOnlineGames::startNetWorkRequest(OnlineGameInfo* info)
{
    OnlineGameInfo* duplex = this->existCurrentGameInfo(info);
    QString         request;
    if (duplex != NULL) {
        this->m_currentGameInfo = duplex;
        if (!this->m_onlineGames.contains(info))
            delete info;

        request = QString("%1/%2").arg(GETMATCH_DATA).arg(duplex->m_matchID);
    } else {
        this->m_currentGameInfo = info;
        request                 = QString("%1/%2/%3/%4").arg(GETMATCH_DATA, info->m_competition).arg(info->m_season).arg(info->m_index);
    }

    this->m_bRequestCanceled = false;

    qInfo().noquote() << QString("Request for game %1:%2:%3")
                             .arg(this->m_currentGameInfo->m_index)
                             .arg(this->m_currentGameInfo->m_competition)
                             .arg(this->m_currentGameInfo->m_season);

#ifdef QT_DEBUG
    qInfo().noquote() << QString("Single game answer for game %1:%2:%3")
                             .arg(this->m_currentGameInfo->m_index)
                             .arg(this->m_currentGameInfo->m_competition)
                             .arg(this->m_currentGameInfo->m_season);
    this->checkNewNetworkRequest(true);
#else
#if (QT_VERSION < QT_VERSION_CHECK(5, 8, 0))
    qInfo().noquote() << "Did not use ReadOnlineGame because of version problem";
#else
    this->m_networkTimout->start();
    this->m_netAccess->get(QNetworkRequest(QUrl(request)));
#endif
#endif
}

OnlineGameInfo* ReadOnlineGames::existCurrentGameInfo(OnlineGameInfo* info)
{
    for (int i = 0; i < this->m_onlineGames.size(); i++) {
        if (this->m_onlineGames[i]->m_competition == info->m_competition) {
            if (this->m_onlineGames[i]->m_season == info->m_season) {
                if (this->m_onlineGames[i]->m_index == info->m_index)
                    return this->m_onlineGames[i];
            }
        }
    }
    return NULL;
}

void ReadOnlineGames::checkNewNetworkRequest(bool checkLastItem)
{
    bool   fastUpdate;
    qint64 nextUpdate;

    if (this->m_currentRequestIndex < 0) {
        /* catch as long there are not all games listed */
        if (this->m_onlineGames.size() < this->getTotalCountOfRequest()) {
            OnlineGameInfo* gameInfo = NULL;
            if (checkLastItem)
                gameInfo = this->getNextRequest(this->m_currentGameInfo);
            else
                gameInfo = this->getNextRequest(NULL);

            if (gameInfo != NULL) {
                gameInfo->m_matchID = 0;

                this->startNetWorkRequest(gameInfo);
                return;
            }
        } else {
            this->m_currentRequestIndex = 0;
            this->m_timeOutCounter      = 0;
            qInfo().noquote() << "Got all games for internal list";
        }
        nextUpdate = this->getNextGameInMilliSeconds(fastUpdate);
    } else {
        nextUpdate          = this->getNextGameInMilliSeconds(fastUpdate);
        qint64 oneMonthsAgo = QDateTime::currentDateTime().addMonths(-1).toMSecsSinceEpoch();
        while (this->m_currentRequestIndex < this->m_onlineGames.size()) {
            OnlineGameInfo* gameInfo = this->m_onlineGames[this->m_currentRequestIndex];
            this->m_currentRequestIndex++;
            /* if game was 1 month ago, forget it */
            if (gameInfo->m_gameFinished && gameInfo->m_timeStamp < oneMonthsAgo)
                continue;

            /* Filter games which are not listed in this fast period */
            if (fastUpdate) {
                qint64 diffTime = nextUpdate - gameInfo->m_timeStamp;
                if (diffTime > 8 * HOUR_IN_MSEC || diffTime < 0)
                    continue;
            }
            this->startNetWorkRequest(gameInfo);
            return;
        }
        this->m_currentRequestIndex = 0;
        this->m_timeOutCounter      = 0;
        qInfo().noquote() << "Updated all games for internal list";
    }

    qint64 now = QDateTime::currentMSecsSinceEpoch();
    if (nextUpdate > now) {
        qInfo().noquote() << QString("Sleeping and reading again in %1 seconds").arg((nextUpdate - now) / 1000);
        this->m_networkUpdate->start(nextUpdate - now);
    } else
        this->slotNetWorkUpdateTimeout();
}

qint64 ReadOnlineGames::getNextGameInMilliSeconds(bool& fastUpdate)
{
    qint64 now = QDateTime::currentDateTime().toMSecsSinceEpoch();

    /* if list is not full, update every 15 min */
    if (this->m_currentRequestIndex < 0)
        return now + (15 * MINUTE_IN_MSEC);

#undef DEBUG_UPDATE
//#define DEBUG_UPDATE
#ifdef DEBUG_UPDATE
    qint64 rValue = now + (2 * MINUTE_IN_MSEC);
#else
    qint64 rValue = now + (6 * HOUR_IN_MSEC);  // 6h - 1000ms * 60s * 60min * 4
#endif

    fastUpdate = false;

    for (int i = 0; i < this->m_onlineGames.size(); i++) {
        qint64 timestamp = this->m_onlineGames[i]->m_timeStamp;
        /* game was in past */
        if (now > timestamp) {
            if (this->m_onlineGames[i]->m_gameFinished)
                continue;

            if ((now - timestamp) < (8 * HOUR_IN_MSEC)) {
                rValue     = now + (5 * MINUTE_IN_MSEC);
                fastUpdate = true;
                return rValue;
            }
            continue;
        }

        /* If game is less than 6 hours in future, take start of game (plus 10msec) */
        if ((timestamp - now) < (6 * HOUR_IN_MSEC))
            rValue = timestamp + 10;
    }
    return rValue;
}

void ReadOnlineGames::slotNetWorkUpdateTimeout()
{
    qInfo().noquote() << QString("Start new read online game info for \"%1\" with %2 entries").arg(this->m_comp).arg(this->m_maxIndex);

    this->checkNewNetworkRequest(false);
}

void ReadOnlineGames::slotNetWorkRequestTimeout()
{
    this->m_bRequestCanceled = true;
    /* 20.06.22
     * As request is handled even if timeout occurs, this will lead to undefined behaviour with seasonIndex
     * So better restart whole reading games in a few seconds
     *
    this->checkNewNetworkRequest(true);
     */

    this->m_timeOutCounter++;
    qint32 iUpdate = m_timeOutCounter * 60;
    if (this->m_timeOutCounter > 10) {
        iUpdate = 3 * 60 * 60;
    }
    qInfo().noquote() << QString("Request Timeout: Sleeping and reading again in %1 seconds").arg(iUpdate);
    this->m_networkUpdate->start(iUpdate * 1000);
}

void ReadOnlineGames::slotNetWorkRequestFinished(QNetworkReply* reply)
{
    if (this->m_bRequestCanceled) {
        qInfo().noquote() << "Read online games, last request was canceled!";
        return;
    }

    this->m_networkTimout->stop();

    QByteArray    arr = reply->readAll();
    QJsonDocument d   = QJsonDocument::fromJson(arr);

    if (d.isArray()) { /* all Games */
        qInfo().noquote() << QString("Request answer for game %1:%2:%3")
                                 .arg(this->m_currentGameInfo->m_index)
                                 .arg(this->m_currentGameInfo->m_competition)
                                 .arg(this->m_currentGameInfo->m_season);
        QJsonArray array = d.array();
        for (int i = 0; i < array.size(); i++) {
            QJsonObject gameObj = array[i].toObject();
            if (this->readSingleGame(gameObj))
                break;
        }
    } else { /* just single game */
        QJsonObject gameObj = d.object();
        qInfo().noquote() << QString("Single game answer for game %1:%2:%3")
                                 .arg(this->m_currentGameInfo->m_index)
                                 .arg(this->m_currentGameInfo->m_competition)
                                 .arg(this->m_currentGameInfo->m_season);
        this->readSingleGame(gameObj);
    }

    this->checkNewNetworkRequest(true);
}

bool ReadOnlineGames::readSingleGame(QJsonObject& json)
{
    if (!json.contains("LastUpdateDateTime"))
        return false;
    QString lastUpdateDateTime = json.value("LastUpdateDateTime").toString();
    while (lastUpdateDateTime.size() - lastUpdateDateTime.lastIndexOf(".") < 4)
        lastUpdateDateTime.append("0");
    QDateTime lastUpdate = QDateTime::fromString(lastUpdateDateTime, "yyyy-MM-ddThh:mm:ss.zzz");

    /* if nothing changed */
    if (this->m_currentGameInfo->m_matchID != 0 && this->m_currentGameInfo->m_lastUpdate == lastUpdate.toMSecsSinceEpoch())
        return true;

    /* read team 1 */
    if (!json.contains("Team1"))
        return false;
    QJsonObject objTeam = json.value("Team1").toObject();
    QString     team1   = this->readSingleTeam(objTeam);

    /* read date time from game */
    if (!json.contains("Team2"))
        return false;
    objTeam       = json.value("Team2").toObject();
    QString team2 = this->readSingleTeam(objTeam);

    if (team1 == "Karlsruher SC")
        team1 = "KSC";
    else if (team2 == "Karlsruher SC")
        team2 = "KSC";
    else
        return false;

    if (!json.contains(("Group")))
        return false;

    QJsonObject groupObj     = json.value("Group").toObject();
    qint32      groupOrderId = groupObj.value("GroupOrderID").toInt(-1);
    if (groupOrderId != this->m_currentGameInfo->m_index) {
        qInfo().noquote() << "Error reading game. Index is different: " << groupOrderId << " " << this->m_currentGameInfo->m_index;
        return false;
    }

    /* read date time from game */
    if (!json.contains("MatchDateTimeUTC"))
        return false;
    QString   matchDateTimeUtc = json.value("MatchDateTimeUTC").toString();
    QDateTime gameDate         = QDateTime::fromString(matchDateTimeUtc, "yyyy-MM-ddThh:mm:ssZ");
    gameDate.setTimeSpec(Qt::UTC);
    gameDate = gameDate.toLocalTime();

    if (!json.contains("MatchID"))
        return false;
    quint32 matchID = json.value("MatchID").toInt();

    if (!json.contains("MatchIsFinished"))
        return false;
    bool matchIsFinished = json.value("MatchIsFinished").toBool();

    QString score = "";
    if (json.contains("MatchResults")) {
        QJsonArray arrResults = json.value("MatchResults").toArray();
        score                 = this->readSingleGameResult(arrResults);
    }

    this->m_currentGameInfo->m_team1        = team1;
    this->m_currentGameInfo->m_team2        = team2;
    this->m_currentGameInfo->m_score        = score;
    this->m_currentGameInfo->m_timeStamp    = gameDate.toMSecsSinceEpoch();
    this->m_currentGameInfo->m_lastUpdate   = lastUpdate.toMSecsSinceEpoch();
    this->m_currentGameInfo->m_gameFinished = matchIsFinished;
    if (this->m_currentGameInfo->m_matchID == 0)
        this->m_currentGameInfo->m_matchID = matchID;
    else if (this->m_currentGameInfo->m_matchID != matchID)
        return false;

    if (!this->m_onlineGames.contains(this->m_currentGameInfo))
        this->m_onlineGames.append(this->m_currentGameInfo);

    CompetitionIndex comp = NO_COMPETITION;
    if (this->m_currentGameInfo->m_competition == "bl1")
        comp = BUNDESLIGA_1;
    else if (this->m_currentGameInfo->m_competition == "bl2" || this->m_currentGameInfo->m_competition == "bl_2")
        comp = BUNDESLIGA_2;
    else if (this->m_currentGameInfo->m_competition == "bl3")
        comp = LIGA_3;
    else if (this->m_currentGameInfo->m_competition == "dfb2017")
        comp = DFB_POKAL;
    g_GlobalData->m_GamesList.addNewGame(this->m_currentGameInfo->m_team1,
                                         this->m_currentGameInfo->m_team2,
                                         this->m_currentGameInfo->m_timeStamp,
                                         this->m_currentGameInfo->m_index,
                                         this->m_currentGameInfo->m_score,
                                         comp,
                                         this->m_currentGameInfo->m_season,
                                         this->m_currentGameInfo->m_lastUpdate);

    return true;
    /*
     * TODO: read Spieltag / Group = Bl1/2/3
     */
}

QString ReadOnlineGames::readSingleTeam(QJsonObject& json)
{
    if (!json.contains("TeamName"))
        return "";
    return json.value("TeamName").toString();
}

QString ReadOnlineGames::readSingleGameResult(QJsonArray& json)
{
    if (json.isEmpty())
        return "";

    QString rValue;
    int     typeID = 0;
    for (int i = 0; i < json.size(); i++) {
        QJsonObject objResult = json[i].toObject();
        if (!objResult.contains("PointsTeam1") || !objResult.contains("PointsTeam2") || !objResult.contains("ResultTypeID"))
            continue;
        if (objResult.value("ResultTypeID").toInt() > typeID) {
            typeID = objResult.value("ResultTypeID").toInt();
            rValue = QString::number(objResult.value("PointsTeam1").toInt());
            rValue.append(":");
            rValue.append(QString::number(objResult.value("PointsTeam2").toInt()));
        }
    }

    return rValue;
}

qint32 ReadOnlineGames::terminate()
{
    this->m_initialized = false;

    if (this->m_networkTimout != NULL)
        delete this->m_networkTimout;
    this->m_networkTimout = NULL;

    if (this->m_networkUpdate != NULL)
        delete this->m_networkUpdate;
    this->m_networkUpdate = NULL;

    //    if (this->m_ctrl->IsRunning())
    //        this->m_ctrl->Stop(true);

    qInfo() << "Shutting down readOnline";

    return ERROR_CODE_SUCCESS;
}
