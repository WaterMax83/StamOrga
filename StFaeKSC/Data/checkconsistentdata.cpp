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

#include <QDateTime>

#include "../General/globaldata.h"
#include "checkconsistentdata.h"

extern GlobalData* g_GlobalData;

CheckConsistentData::CheckConsistentData(QObject* parent)
    : BackgroundWorker(parent)
{
}


qint32 CheckConsistentData::initialize()
{

    return ERROR_CODE_SUCCESS;
}

int CheckConsistentData::DoBackgroundWork()
{
    this->m_timer = new QTimer();
    this->m_timer->setSingleShot(true);

    connect(this->m_timer, &QTimer::timeout, this, &CheckConsistentData::slotTimerFired);

    this->m_timer->start(5000); /*Wait 5s at the beginning */

    return ERROR_CODE_SUCCESS;
}


void CheckConsistentData::slotTimerFired()
{
    qInfo().noquote() << "Starting consistency check";

    QMutexLocker lock(&g_GlobalData->m_globalDataMutex);

    /* Check if server data is consistent with savings */
    foreach (AvailableGameTickets* pGameTicket, g_GlobalData->m_availableTickets) {
        pGameTicket->checkConsistency();
    }

    quint32 gameIndex;
    qint64  yesterday = QDateTime::currentDateTime().addDays(-1).toMSecsSinceEpoch();
    for (int i = g_GlobalData->m_userEvents.count() - 1; i >= 0; i--) {
        if (g_GlobalData->m_userEvents.at(i)->checkIsEventForAGame(gameIndex) == ERROR_CODE_UPDATE_LIST) {
            GamesPlay* pGame = (GamesPlay*)g_GlobalData->m_GamesList.getItem(gameIndex);
            if (gameIndex > 0 && (pGame == NULL || pGame->m_timestamp < yesterday)) {
                QString fileName = g_GlobalData->m_userEvents.at(i)->getFileName();
                QFile   eventFile(fileName);
                if (eventFile.exists() && eventFile.remove()) {
                    g_GlobalData->m_userEvents.at(i)->terminate();
                    if (pGame != NULL)
                        qInfo().noquote() << QString("Delete Event of game %1:%2, because it is too old").arg(pGame->m_itemName, pGame->m_away);
                    else
                        qInfo().noquote() << QString("Delete Event of game with index %1, because its deleted").arg(gameIndex);
                    g_GlobalData->m_userEvents.removeAt(i);
                }
            }
        }
        else if (g_GlobalData->m_userEvents.at(i)->getType() == NOTIFY_TOPIC_NEW_FANCLUB_NEWS) {
            qint32 newsIndex = g_GlobalData->m_userEvents.at(i)->getInfo().toInt(0);
            if (newsIndex == 0)
                continue;
            NewsData* pNews = (NewsData*)g_GlobalData->m_fanclubNews.getItem(newsIndex);
            if (pNews == NULL) {
                QString fileName = g_GlobalData->m_userEvents.at(i)->getFileName();
                QFile   eventFile(fileName);
                if (eventFile.exists() && eventFile.remove()) {
                    g_GlobalData->m_userEvents.at(i)->terminate();
                    qInfo().noquote() << QString("Delete Event of news with index %1, because its deleted").arg(newsIndex);
                    g_GlobalData->m_userEvents.removeAt(i);
                }
            }
        }
    }

    this->m_timer->start(3 * 60 * 60 * 1000); // 3h
}
