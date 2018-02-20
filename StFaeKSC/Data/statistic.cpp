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
#include <QDebug>
#include <QtCore/QJsonArray>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>
#include <QtCore/QJsonValue>

#include "../General/globaldata.h"
#include "statistic.h"

extern GlobalData* g_GlobalData;

Statistic::Statistic(QObject* parent)
    : BackgroundWorker(parent)
{
}


int Statistic::initialize()
{
    this->m_bckGrndCtrl = new BackgroundController();
    this->m_bckGrndCtrl->Start(this, false);

    return 1;
}

int Statistic::DoBackgroundWork()
{
    this->m_cycleTimer = new QTimer();
    this->m_cycleTimer->setSingleShot(true);
    connect(this->m_cycleTimer, &QTimer::timeout, this, &Statistic::slotCycleTimerFired);
#ifdef QT_DEBUG
    this->m_cycleTimer->start(1000);
#else
    this->m_cycleTimer->start(10000);
#endif

    return ERROR_CODE_SUCCESS;
}

void Statistic::slotCycleTimerFired()
{
    QMutexLocker lock0(&g_GlobalData->m_globalDataMutex);
    QMutexLocker lock1(&this->m_statsMutex);

    /* First collect all tickets */
    for (int i = 0; i < g_GlobalData->m_SeasonTicket.getNumberOfInternalList(); i++) {
        TicketInfo* pTicket = (TicketInfo*)g_GlobalData->m_SeasonTicket.getRequestConfigItemFromListIndex(i);
        if (pTicket == NULL)
            continue;

        StatsTickets* pStats = new StatsTickets();
        pStats->m_userIndex  = pTicket->m_userIndex;
        pStats->m_name       = pTicket->m_itemName;
        pStats->m_timestamp  = pTicket->m_creation;

        this->m_statsTickets.append(pStats);
    }

    qint64 currentTimeStamp = QDateTime::currentMSecsSinceEpoch();

    /* get all games to get the number of tickets for blocked */
    for (int i = 0; i < g_GlobalData->m_GamesList.getNumberOfInternalList(); i++) {
        GamesPlay* pGame = (GamesPlay*)g_GlobalData->m_GamesList.getRequestConfigItemFromListIndex(i);
        if (pGame == NULL || pGame->m_saison != g_GlobalData->m_currentSeason || pGame->m_timestamp > currentTimeStamp)
            continue;

        foreach (StatsTickets* pStats, this->m_statsTickets) {
            if (pStats->m_timestamp > pGame->m_timestamp)
                continue;
            pStats->m_blocked++;
        }
    }


    foreach (AvailableGameTickets* pAvTick, g_GlobalData->m_availableTickets) {
        GamesPlay* pGame = (GamesPlay*)g_GlobalData->m_GamesList.getItem(pAvTick->getGameIndex());

        if (pGame == NULL || pGame->m_saison != g_GlobalData->m_currentSeason || pGame->m_timestamp > currentTimeStamp)
            continue;

        for (int i = 0; i < pAvTick->getNumberOfInternalList(); i++) {
            AvailableTicketInfo* pTicket = (AvailableTicketInfo*)pAvTick->getRequestConfigItemFromListIndex(i);
            if (pTicket == NULL)
                continue;

            /* add counter for free or reserved */
            foreach (StatsTickets* pStats, this->m_statsTickets) {
                if (pStats->m_userIndex != pTicket->m_userID)
                    continue;

                if (pTicket->m_state != TICKET_STATE_BLOCKED) {
                    pStats->m_blocked--;
                    if (pTicket->m_state == TICKET_STATE_FREE)
                        pStats->m_free++;
                    else if (pTicket->m_state == TICKET_STATE_RESERVED)
                        pStats->m_reserved++;
                }
            }
        }
    }

    this->m_cycleTimer->start(6 * 60 * 60 * 1000);
}


/*
 * Request
 * {
 *      "type": "Statistic",
 *      "version": "V1.0"       // not used at the moment
 *      "cmd":  "overview"  -> get groups of statistic
 * }
 *
 * Answer
 * {
 *      "type": "Statistic",
 *
 *      "cmd": "overview"   -> send groups of statistic
 *      "parameter": [{"value":"SeasonTickets"},{"value":"Stats2"}]
 */
qint32 Statistic::handleStatisticCommand(QByteArray& command, QByteArray& answer)
{
    QMutexLocker lock(&this->m_statsMutex);

    QJsonParseError jerror;
    QJsonObject     rootObj = QJsonDocument::fromJson(command, &jerror).object();
    if (jerror.error != QJsonParseError::NoError) {
        qWarning().noquote() << QString("Could not answer statistic command, json parse errror: %1 - %2").arg(jerror.errorString()).arg(jerror.offset);
        return ERROR_CODE_WRONG_PARAMETER;
    }

    QString cmd = rootObj.value("cmd").toString("");
    if (cmd.isEmpty()) {
        qWarning().noquote() << QString("No cmd found in JSON Statistics command");
        return ERROR_CODE_MISSING_PARAMETER;
    }

    QJsonObject rootObjAnswer;
    if (cmd == "overview") {
        QJsonArray parameter;
        parameter.append("Dauerkarten");
        parameter.append("FillBlock");

        rootObjAnswer.insert("parameter", parameter);

    } else if (cmd == "content") {
        QString para = rootObj.value("parameter").toString("");
        if (para == "Dauerkarten") {
            QJsonArray categories;

            QJsonArray blocked, reserved, free;
            foreach (StatsTickets* pStats, this->m_statsTickets) {
                QString name = pStats->m_name;
                if (name.size() > 8)
                    name.resize(8);
                categories.append(name);

                blocked.append(pStats->m_blocked);
                reserved.append(pStats->m_reserved);
                free.append(pStats->m_free);
            }

            QJsonObject barBlocked, barReserved, barFree;
            barBlocked.insert("title", "Geht selbst");
            barReserved.insert("title", "Reserviert");
            barFree.insert("title", "Frei");
            barBlocked.insert("values", blocked);
            barReserved.insert("values", reserved);
            barFree.insert("values", free);
            QJsonArray bars;
            bars.append(barBlocked);
            bars.append(barReserved);
            bars.append(barFree);

            rootObjAnswer.insert("categories", categories);
            rootObjAnswer.insert("bars", bars);
        } else
            return ERROR_CODE_NOT_POSSIBLE;
    } else
        return ERROR_CODE_NOT_POSSIBLE;

    rootObjAnswer.insert("type", "Statistic");
    rootObjAnswer.insert("cmd", cmd);

    answer.clear();
    answer.append(QJsonDocument(rootObjAnswer).toJson(QJsonDocument::Compact));

    return ERROR_CODE_SUCCESS;
}
