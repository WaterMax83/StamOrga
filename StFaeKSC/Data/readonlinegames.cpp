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

#include <QtCore/QDebug>
#include <QtCore/QFile>

#include "../Common/General/globalfunctions.h"
#include "readonlinegames.h"

ReadOnlineGames::ReadOnlineGames(QObject *parent) : BackgroundWorker(parent)
{

}

void ReadOnlineGames::initialize(GlobalData *globalData)
{
    this->m_globalData = globalData;
}


int ReadOnlineGames::DoBackgroundWork()
{
    this->m_netAccess = new QNetworkAccessManager();
    connect(m_netAccess, &QNetworkAccessManager::finished, this, &ReadOnlineGames::slotNetWorkReplayFinished);

    this->m_netAccess->get(QNetworkRequest(QUrl("https://www.openligadb.de/api/getmatchdata/bl1/2016/8")));



    return 0;
}


void ReadOnlineGames::slotNetWorkReplayFinished(QNetworkReply* reply)
{
    QByteArray arr = reply->readAll();


    QString filePath = getUserHomeConfigPath() + "/egal.json";
    QFile file(filePath);
    file.open(QFile::WriteOnly);
    file.write(arr);
    file.flush();
    file.close();


}
