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
#include "readonlinegames.h"

#define GETMATCH_DATA "https://www.openligadb.de/api/getmatchdata"

#define MINUTE_IN_MSEC 60 * 1000
#define HOUR_IN_MSEC 60 * 60 * 1000


ReadOnlineGames::ReadOnlineGames(QObject* parent)
    : BackgroundWorker(parent)
{
}

void ReadOnlineGames::initialize(GlobalData* globalData)
{
    this->m_globalData = globalData;
}


int ReadOnlineGames::DoBackgroundWork()
{
    OnlineGameInfo* gameInfo = new OnlineGameInfo();
    gameInfo->m_competition  = "bl2";
    gameInfo->m_season       = 2016;
    gameInfo->m_index        = 1;
    gameInfo->m_matchID      = 0;

    this->m_netAccess = new QNetworkAccessManager();
    connect(this->m_netAccess, &QNetworkAccessManager::finished, this, &ReadOnlineGames::slotNetWorkRequestFinished);

    this->m_networkTimout = new QTimer();
    this->m_networkTimout->setSingleShot(true);
    this->m_networkTimout->setInterval(5000);
    connect(this->m_networkTimout, &QTimer::timeout, this, &ReadOnlineGames::slotNetWorkRequestTimeout);

    this->m_networkUpdate = new QTimer();
    this->m_networkUpdate->setSingleShot(true);
    connect(this->m_networkUpdate, &QTimer::timeout, this, &ReadOnlineGames::slotNetWorkUpdateTimeout);

    this->startNetWorkRequest(gameInfo);


    //    QString filePath = getUserHomeConfigPath() + "/egal.json";
    //    QFile   file(filePath);
    //    if (!file.open(QFile::ReadOnly))
    //        return 0;

    //    QByteArray arr = file.readAll();

    //    QString     test     = d.object()["TimeZoneID"].toString();
    //    QJsonObject obj      = array[0].toObject();
    //    bool        contains = obj.contains("Goals");
    //    QJsonObject obj2     = obj["Goals"].toObject();

    //    QString    test2  = obj["TimeZoneID"].toString();
    //    QJsonValue val    = obj.value("TimeZoneID");
    //    QString    egal   = val.toString();
    //    quint32    number = val.toInt();


    return 0;
}

void ReadOnlineGames::startNetWorkRequest(OnlineGameInfo* info)
{
#if (QT_VERSION >= QT_VERSION_CHECK(5, 8, 0))


    OnlineGameInfo* duplex  = this->existCurrentGameInfo(info);
    QString         request = QString("%1/%2/%3/%4").arg(GETMATCH_DATA, info->m_competition).arg(info->m_season).arg(info->m_index);
    if (duplex != NULL) {
        this->m_currentGameInfo = duplex;
        delete info;

        qint64 now2Days = QDateTime::currentDateTime().addDays(2).toMSecsSinceEpoch();
        if (now2Days > duplex->m_timeStamp && duplex->m_lastUpdate > (duplex->m_timeStamp + 120 * 60 * 1000)) {
            /* game was 2 days in past and update was after the end of the game */
            this->checkNewNetworkRequest();
            return;
        }

        request = QString("%1/%2").arg(GETMATCH_DATA).arg(duplex->m_matchID);
    } else
        this->m_currentGameInfo = info;


    this->m_networkTimout->start();
    this->m_bRequestCanceled = false;

    qInfo().noquote() << QString("Request for game %1").arg(info->m_index);
    this->m_netAccess->get(QNetworkRequest(QUrl(request)));
#else
    Q_UNUSED(info);
    qInfo().noquote() << "Did not use ReadOnlineGame because of version problem";
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

void ReadOnlineGames::checkNewNetworkRequest()
{
    if (this->m_currentGameInfo->m_index < 34) {
        OnlineGameInfo* gameInfo = new OnlineGameInfo();
        gameInfo->m_competition  = this->m_currentGameInfo->m_competition;
        gameInfo->m_season       = this->m_currentGameInfo->m_season;
        gameInfo->m_index        = this->m_currentGameInfo->m_index + 1;
        gameInfo->m_matchID      = 0;

        this->startNetWorkRequest(gameInfo);
    } else {
        //        qInfo() << "Updated all Games from season 2016";
        qint64 nextUpdate = this->getNextGameInMilliSeconds();
        this->m_networkUpdate->start(nextUpdate);
    }
}

qint64 ReadOnlineGames::getNextGameInMilliSeconds()
{
    qint64 rValue = 4 * HOUR_IN_MSEC; // 4h - 1000ms * 60s * 60min * 4
    qint64 now    = QDateTime::currentDateTime().toMSecsSinceEpoch();
    for (int i = 0; i < this->m_onlineGames.size(); i++) {
        qint64 timestamp = this->m_onlineGames[i]->m_timeStamp;
        if (now > timestamp) {
            if ((now - timestamp) < 4 * HOUR_IN_MSEC)
                rValue = 15 * MINUTE_IN_MSEC;
            continue;
        }
        if ((timestamp - now) < rValue)
            rValue = (timestamp - now) + 15 * MINUTE_IN_MSEC;
    }
    return rValue;
}

void ReadOnlineGames::slotNetWorkUpdateTimeout()
{
    OnlineGameInfo* gameInfo = new OnlineGameInfo();
    gameInfo->m_competition  = "bl2";
    gameInfo->m_season       = 2016;
    gameInfo->m_index        = 1;

    this->startNetWorkRequest(gameInfo);
}

void ReadOnlineGames::slotNetWorkRequestTimeout()
{
    this->m_bRequestCanceled = true;
    this->checkNewNetworkRequest();
}

void ReadOnlineGames::slotNetWorkRequestFinished(QNetworkReply* reply)
{
    if (this->m_bRequestCanceled)
        return;

    this->m_networkTimout->stop();

    QByteArray    arr = reply->readAll();
    QJsonDocument d   = QJsonDocument::fromJson(arr);

    if (d.isArray()) { /* all Games */
        qInfo().noquote() << QString("Request answer for game %1").arg(this->m_currentGameInfo->m_index);
        QJsonArray array = d.array();
        for (int i = 0; i < array.size(); i++) {
            QJsonObject gameObj = array[i].toObject();
            if (this->readSingleGame(gameObj))
                break;
        }
    } else { /* just single game */
        QJsonObject gameObj = d.object();
        qInfo().noquote() << QString("Single game answer for game %1").arg(this->m_currentGameInfo->m_index);
        this->readSingleGame(gameObj);
    }

    this->checkNewNetworkRequest();
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

    QString score = "";
    if (json.contains("MatchResults")) {
        QJsonArray arrResults = json.value("MatchResults").toArray();
        score                 = this->readSingleGameResult(arrResults);
    }

    this->m_currentGameInfo->m_team1      = team1;
    this->m_currentGameInfo->m_team2      = team2;
    this->m_currentGameInfo->m_score      = score;
    this->m_currentGameInfo->m_timeStamp  = gameDate.toMSecsSinceEpoch();
    this->m_currentGameInfo->m_lastUpdate = lastUpdate.toMSecsSinceEpoch();
    if (this->m_currentGameInfo->m_matchID == 0)
        this->m_currentGameInfo->m_matchID = matchID;
    else if (this->m_currentGameInfo->m_matchID != matchID)
        return false;

    if (!this->m_onlineGames.contains(this->m_currentGameInfo))
        this->m_onlineGames.append(this->m_currentGameInfo);

    quint8 comp = 0;
    if (this->m_currentGameInfo->m_competition == "bl2")
        comp = 2;
    else if (this->m_currentGameInfo->m_competition == "bl3")
        comp = 3;
    this->m_globalData->m_GamesList.addNewGame(this->m_currentGameInfo->m_team1,
                                               this->m_currentGameInfo->m_team2,
                                               this->m_currentGameInfo->m_timeStamp,
                                               this->m_currentGameInfo->m_index,
                                               this->m_currentGameInfo->m_score,
                                               comp,
                                               this->m_currentGameInfo->m_season);

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
