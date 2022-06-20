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

#ifndef READONLINEGAMES_H
#define READONLINEGAMES_H

#include <QtCore/QJsonArray>
#include <QtCore/QJsonObject>
#include <QtCore/QList>
#include <QtCore/QObject>
#include <QtCore/QTimer>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>

#include "../Common/General/backgroundcontroller.h"
#include "../Common/General/backgroundworker.h"

struct OnlineGameInfo {
    QString m_team1;
    QString m_team2;
    QString m_competition;
    QString m_score;
    qint32  m_season;
    qint32  m_index;
    quint32 m_matchID;
    qint64  m_timeStamp;
    qint64  m_lastUpdate;
    bool    m_gameFinished;

    OnlineGameInfo()
    {
        this->m_competition = "bl";
        this->m_season      = 2016;
    }

    //    bool m_checkUpdate;
};

struct OnlineGamesRequestList {
    QString m_comp;
    qint32  m_season;
    qint32  m_maxIndex;
};

class ReadOnlineGames : public BackgroundWorker
{
    Q_OBJECT
public:
    explicit ReadOnlineGames(QObject* parent = 0);
    virtual ~ReadOnlineGames();

    qint32 initialize(QString comp, qint32 max, qint32 season);

    QString getCompetition() { return this->m_comp; }
    qint32  getMaxIndex() { return this->m_maxIndex; }
    qint32  getSeason() { return this->m_season; }

    qint32 terminate();

signals:

public slots:
    void slotNetWorkRequestFinished(QNetworkReply* reply);
    void slotNetWorkRequestTimeout();
    void slotNetWorkUpdateTimeout();

protected:
    int DoBackgroundWork();

    /* Starting in own thread does not work at the moment -> segfault in desctructor of controller */
    //    BackgroundController*  m_ctrl;
    QNetworkAccessManager* m_netAccess;
    QTimer*                m_networkTimout;
    bool                   m_bRequestCanceled;
    qint32                 m_currentRequestIndex;

    QList<OnlineGameInfo*> m_onlineGames;
    OnlineGameInfo*        m_currentGameInfo;

    //    OnlineGamesRequestList m_requestList;
    QString         m_comp;
    qint32          m_season;
    qint32          m_maxIndex;
    qint32          getTotalCountOfRequest();
    OnlineGameInfo* getNextRequest(OnlineGameInfo* currentGame);

    QTimer* m_networkUpdate;
    qint32  m_timeOutCounter;

    void startNetWorkRequest(OnlineGameInfo* info);

    OnlineGameInfo* existCurrentGameInfo(OnlineGameInfo* info);

    qint64 getNextGameInMilliSeconds(bool& fastUpdate);

    void checkNewNetworkRequest(bool checkLastItem);

    bool    readSingleGame(QJsonObject& json);
    QString readSingleTeam(QJsonObject& json);
    QString readSingleGameResult(QJsonArray& json);
};

#endif  // READONLINEGAMES_H
