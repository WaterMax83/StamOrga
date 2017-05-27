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

#include "../Common/General/backgroundworker.h"
#include "../General/globaldata.h"

struct OnlineGameInfo {
    QString m_team1;
    QString m_team2;
    QString m_competition;
    QString m_score;
    quint32 m_season;
    quint32 m_index;
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

struct RequestList {
    QString m_comp;
    quint32 m_season;
    quint32 m_maxIndex;
};

class ReadOnlineGames : public BackgroundWorker
{
    Q_OBJECT
public:
    explicit ReadOnlineGames(QObject* parent = 0);


    void initialize(GlobalData* globalData);

signals:

public slots:
    void slotNetWorkRequestFinished(QNetworkReply* reply);
    void slotNetWorkRequestTimeout();
    void slotNetWorkUpdateTimeout();


protected:
    int DoBackgroundWork();

    GlobalData*            m_globalData;
    QNetworkAccessManager* m_netAccess;
    QTimer*                m_networkTimout;
    bool                   m_bRequestCanceled;
    qint32                 m_currentRequestIndex;

    QList<OnlineGameInfo*> m_onlineGames;
    OnlineGameInfo*        m_currentGameInfo;

    QList<RequestList*> m_requestList;
    qint32              getTotalCountOfRequest();
    OnlineGameInfo* getNextRequest(OnlineGameInfo* currentGame);

    QTimer* m_networkUpdate;

    void startNetWorkRequest(OnlineGameInfo* info);

    OnlineGameInfo* existCurrentGameInfo(OnlineGameInfo* info);

    qint64 getNextGameInMilliSeconds(bool& fastUpdate);

    void checkNewNetworkRequest(bool checkLastItem);

    bool readSingleGame(QJsonObject& json);
    QString readSingleTeam(QJsonObject& json);
    QString readSingleGameResult(QJsonArray& json);
};

#endif // READONLINEGAMES_H
