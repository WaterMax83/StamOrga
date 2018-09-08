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

#include "../Common/Network/messagecommand.h"
#include "../General/globaldata.h"
#include "cstatisticmanager.h"

extern GlobalData* g_GlobalData;

cStatisticManager g_StatisticManager;

cStatisticManager::cStatisticManager(QObject* parent)
    : BackgroundWorker(parent)
{
}


int cStatisticManager::initialize()
{
    this->m_bckGrndCtrl = new BackgroundController();
    this->m_bckGrndCtrl->Start(this, false);

    return 1;
}

int cStatisticManager::DoBackgroundWork()
{
    this->m_cycleTimer = new QTimer();
    this->m_cycleTimer->setSingleShot(true);
    connect(this->m_cycleTimer, &QTimer::timeout, this, &cStatisticManager::slotCycleTimerFired);

    connect(this, &cStatisticManager::signalYearChanged, this, &cStatisticManager::slotYearChanged);

//    this->addYearToStatistic(g_GlobalData->m_currentSeason);
//    this->addYearToStatistic(2018);
#ifdef QT_DEBUG
    this->m_cycleTimer->start(2000);
#else
    this->m_cycleTimer->start(10000);
#endif

    return ERROR_CODE_SUCCESS;
}

qint32 cStatisticManager::addYearToStatistic(qint32 year)
{
    QMutexLocker lock1(&this->m_statsMutex);

    for (int i = 0; i < this->m_stats.size(); i++) {
        StatsPerYear* pStats = this->m_stats.at(i);
        if (pStats->m_year == year)
            return ERROR_CODE_NOT_UNIQUE;
    }
    StatsPerYear* pStats = new StatsPerYear();
    pStats->m_year       = year;
    this->m_stats.append(pStats);

    std::sort(this->m_stats.begin(), this->m_stats.end(), StatsPerYear::compareYearDescending);

    emit this->signalYearChanged();

    return ERROR_CODE_SUCCESS;
}

qint32 cStatisticManager::removeYearFromStatistic(qint32 year)
{
    QMutexLocker lock1(&this->m_statsMutex);

    for (int i = 0; i < this->m_stats.size(); i++) {
        StatsPerYear* pStats = this->m_stats.at(i);
        if (pStats->m_year == year) {

            foreach (StatsTickets* pTickets, pStats->m_statsTickets)
                delete pTickets;
            foreach (StatsReserved* pRes, pStats->m_reservedTicketNames)
                delete pRes;
            foreach (StatsMeeting* pMeet, pStats->m_meetingNames)
                delete pMeet;
            foreach (StatsMeeting* pMeet, pStats->m_awayTripNames)
                delete pMeet;
            delete pStats;
            this->m_stats.removeAt(i);

            emit this->signalYearChanged();

            return ERROR_CODE_SUCCESS;
        }
    }

    return ERROR_CODE_NOT_FOUND;
}

void cStatisticManager::slotYearChanged()
{
#ifdef QT_DEBUG
    this->m_cycleTimer->start(2000);
#else
    this->m_cycleTimer->start(10000);
#endif
}

void cStatisticManager::slotCycleTimerFired()
{
    QMutexLocker lock0(&g_GlobalData->m_globalDataMutex);
    QMutexLocker lock1(&this->m_statsMutex);

    /* Clear all info */
    for (int i = 0; i < this->m_stats.size(); i++) {
        StatsPerYear* pStats = this->m_stats.at(i);
        foreach (StatsTickets* pTickets, pStats->m_statsTickets)
            delete pTickets;
        pStats->m_statsTickets.clear();

        foreach (StatsReserved* pRes, pStats->m_reservedTicketNames)
            delete pRes;
        pStats->m_reservedTicketNames.clear();

        foreach (StatsMeeting* pMeet, pStats->m_meetingNames)
            delete pMeet;
        pStats->m_meetingNames.clear();
        foreach (StatsMeeting* pMeet, pStats->m_awayTripNames)
            delete pMeet;
        pStats->m_awayTripNames.clear();
    }

    for (int i = 0; i < this->m_stats.size(); i++) {
        StatsPerYear* pStats = this->m_stats.at(i);

        qint64 seasonStart = g_GlobalData->m_GamesList.getTimeStampofFirstTicketGame(pStats->m_year);
        /* First collect all tickets */
        for (int j = 0; j < g_GlobalData->m_SeasonTicket.getNumberOfInternalList(); j++) {
            TicketInfo* pTicket = (TicketInfo*)g_GlobalData->m_SeasonTicket.getRequestConfigItemFromListIndex(j);
            if (pTicket == NULL)
                continue;

            if (getSeasonFromTimeStamp(pTicket->m_creation) > pStats->m_year)
                continue;

            if (pTicket->isTicketRemoved() && pTicket->m_deleteTimeStamp < seasonStart)
                continue;

            StatsTickets* pTicks  = new StatsTickets();
            pTicks->m_ticketIndex = pTicket->m_index;
            pTicks->m_name        = pTicket->m_itemName;
            if (pTicks->m_name.endsWith("_DELETED"))
                pTicks->m_name.replace("_DELETED", "");
            pTicks->m_creation = pTicket->m_creation;

            pStats->m_statsTickets.append(pTicks);
        }
    }

    this->calculateStatsForAllYears();

    this->m_cycleTimer->start(6 * 60 * 60 * 1000);
}

void cStatisticManager::calculateStatsForAllYears()
{
    qint64 currentTimeStamp = QDateTime::currentMSecsSinceEpoch();

    /* get all games to get the number of tickets for blocked */
    for (int i = 0; i < g_GlobalData->m_GamesList.getNumberOfInternalList(); i++) {
        GamesPlay* pGame = (GamesPlay*)g_GlobalData->m_GamesList.getRequestConfigItemFromListIndex(i);
        if (pGame == NULL || pGame->m_timestamp > currentTimeStamp)
            continue;

        if (pGame->m_itemName != "KSC" || pGame->m_competition > LIGA_3 || pGame->m_competition < BUNDESLIGA_1)
            continue;

        for (int i = 0; i < this->m_stats.size(); i++) {
            StatsPerYear* pStats = this->m_stats.at(i);

            if (pGame->m_season != pStats->m_year)
                continue;

            foreach (StatsTickets* pTicks, pStats->m_statsTickets) {
                if (pTicks->m_creation > pGame->m_timestamp)
                    continue;
                pTicks->m_blocked++;
            }
        }
    }


    foreach (AvailableGameTickets* pAvTick, g_GlobalData->m_availableTickets) {
        GamesPlay* pGame = (GamesPlay*)g_GlobalData->m_GamesList.getItem(pAvTick->getGameIndex());

        if (pGame == NULL || pGame->m_timestamp > currentTimeStamp)
            continue;

        if (pGame->m_itemName != "KSC" || pGame->m_competition > LIGA_3 || pGame->m_competition < BUNDESLIGA_1)
            continue;

        for (int i = 0; i < this->m_stats.size(); i++) {
            StatsPerYear* pStats = this->m_stats.at(i);

            if (pGame->m_season != pStats->m_year)
                continue;

            for (int i = 0; i < pAvTick->getNumberOfInternalList(); i++) {
                AvailableTicketInfo* pTicket = (AvailableTicketInfo*)pAvTick->getRequestConfigItemFromListIndex(i);
                if (pTicket == NULL)
                    continue;

                /* add counter for free or reserved */
                foreach (StatsTickets* pTicks, pStats->m_statsTickets) {
                    if (pTicks->m_ticketIndex != pTicket->m_ticketID)
                        continue;

                    if (pTicket->m_state != TICKET_STATE_BLOCKED && pTicks->m_blocked > 0) {
                        pTicks->m_blocked--;
                        if (pTicket->m_state == TICKET_STATE_FREE)
                            pTicks->m_free++;
                        else if (pTicket->m_state == TICKET_STATE_RESERVED)
                            pTicks->m_reserved++;
                    }
                }

                if (pTicket->m_state == TICKET_STATE_RESERVED) {
                    bool bFoundItem = false;
                    foreach (StatsReserved* pRes, pStats->m_reservedTicketNames) {
                        if (this->cleanName(pRes->m_name) == this->cleanName(pTicket->m_itemName)) {
                            pRes->m_count++;
                            bFoundItem = true;
                            break;
                        }
                    }
                    if (!bFoundItem) {
                        StatsReserved* pRes = new StatsReserved();
                        pRes->m_name        = pTicket->m_itemName;
                        pRes->m_count       = 1;
                        pStats->m_reservedTicketNames.append(pRes);
                    }
                }
            }
        }
    }

    foreach (MeetingInfo* pMeetingInfo, g_GlobalData->m_meetingInfos) {
        GamesPlay* pGame = (GamesPlay*)g_GlobalData->m_GamesList.getItem(pMeetingInfo->getGameIndex());

        if (pGame == NULL || pGame->m_timestamp > currentTimeStamp)
            continue;

        for (int i = 0; i < this->m_stats.size(); i++) {
            StatsPerYear* pStats = this->m_stats.at(i);

            if (pGame->m_season != pStats->m_year)
                continue;

            for (int i = 0; i < pMeetingInfo->getNumberOfInternalList(); i++) {
                AcceptMeetingInfo* pInfo = (AcceptMeetingInfo*)pMeetingInfo->getRequestConfigItemFromListIndex(i);
                if (pInfo == NULL)
                    continue;

                bool bFoundItem = false;
                foreach (StatsMeeting* pMeet, pStats->m_meetingNames) {
                    if (this->cleanName(pMeet->m_name) == this->cleanName(pInfo->m_itemName)) {
                        bFoundItem = true;
                        if (pInfo->m_state == ACCEPT_STATE_ACCEPT)
                            pMeet->m_accepted++;
                        else if (pInfo->m_state == ACCEPT_STATE_MAYBE)
                            pMeet->m_interested++;
                        break;
                    }
                }
                if (!bFoundItem && pInfo->m_state != ACCEPT_STATE_DECLINE) {
                    StatsMeeting* pMeet = new StatsMeeting();
                    pMeet->m_name       = pInfo->m_itemName;
                    if (pInfo->m_state == ACCEPT_STATE_ACCEPT)
                        pMeet->m_accepted++;
                    else if (pInfo->m_state == ACCEPT_STATE_MAYBE)
                        pMeet->m_interested++;
                    pStats->m_meetingNames.append(pMeet);
                }
            }
        }
    }

    foreach (AwayTripInfo* pMeetingInfo, g_GlobalData->m_awayTripInfos) {
        GamesPlay* pGame = (GamesPlay*)g_GlobalData->m_GamesList.getItem(pMeetingInfo->getGameIndex());

        if (pGame == NULL || pGame->m_timestamp > currentTimeStamp)
            continue;

        for (int i = 0; i < this->m_stats.size(); i++) {
            StatsPerYear* pStats = this->m_stats.at(i);

            if (pGame->m_season != pStats->m_year)
                continue;

            for (int i = 0; i < pMeetingInfo->getNumberOfInternalList(); i++) {
                AcceptMeetingInfo* pInfo = (AcceptMeetingInfo*)pMeetingInfo->getRequestConfigItemFromListIndex(i);
                if (pInfo == NULL)
                    continue;

                bool bFoundItem = false;
                foreach (StatsMeeting* pMeet, pStats->m_awayTripNames) {
                    if (this->cleanName(pMeet->m_name) == this->cleanName(pInfo->m_itemName)) {
                        bFoundItem = true;
                        if (pInfo->m_state == ACCEPT_STATE_ACCEPT)
                            pMeet->m_accepted++;
                        else if (pInfo->m_state == ACCEPT_STATE_MAYBE)
                            pMeet->m_interested++;
                        break;
                    }
                }
                if (!bFoundItem && pInfo->m_state != ACCEPT_STATE_DECLINE) {
                    StatsMeeting* pMeet = new StatsMeeting();
                    pMeet->m_name       = pInfo->m_itemName;
                    if (pInfo->m_state == ACCEPT_STATE_ACCEPT)
                        pMeet->m_accepted++;
                    else if (pInfo->m_state == ACCEPT_STATE_MAYBE)
                        pMeet->m_interested++;
                    pStats->m_awayTripNames.append(pMeet);
                }
            }
        }
    }

    for (int i = 0; i < this->m_stats.size(); i++) {
        StatsPerYear* pStats = this->m_stats.at(i);

        std::sort(pStats->m_statsTickets.begin(), pStats->m_statsTickets.end(), StatsTickets::compareCountFunctionAscending);
        std::sort(pStats->m_reservedTicketNames.begin(), pStats->m_reservedTicketNames.end(), StatsReserved::compareCountFunctionAscending);
        std::sort(pStats->m_meetingNames.begin(), pStats->m_meetingNames.end(), StatsMeeting::compareCountFunctionAscending);
        std::sort(pStats->m_awayTripNames.begin(), pStats->m_awayTripNames.end(), StatsMeeting::compareCountFunctionAscending);
    }
}

QString cStatisticManager::cleanName(QString name)
{
    QString rValue = name.toLower();
    rValue.replace("'", "");

    return rValue.replace(".", "");
}


/*
 * Request
 * {
 *      "type": "Statistic",
 *      "version": "V1.0"       // not used at the moment
 *      "cmd":  "overview"  -> get groups of statistic
 * }
 *
 * Request
 * {
 *      "type": "Statistic",
 *      "version": "V1.0"       // not used at the moment
 *      "cmd":  "content"           -> get content of the parameter
 *      "parameter": "Dauerkarten"  -> get content of Dauerkarten
 * }
 *
 * Answer
 * {
 *      "type": "Statistic",
 *      "cmd": "overview"                                                       -> send groups of statistic
 *      "parameter": [{"value":"SeasonTickets"},{"value":"Reservierungen"}]     -> these are the groups
 * }
 */
MessageProtocol* cStatisticManager::handleStatisticCommand(UserConData* pUserCon, MessageProtocol* request)
{
    QMutexLocker lock(&this->m_statsMutex);

    qint32          rCode = ERROR_CODE_SUCCESS;
    QJsonParseError jerror;
    QByteArray      data    = QByteArray(request->getPointerToData());
    QJsonObject     rootObj = QJsonDocument::fromJson(data, &jerror).object();
    if (jerror.error != QJsonParseError::NoError) {
        qWarning().noquote() << QString("Could not answer statistic command, json parse errror: %1 - %2").arg(jerror.errorString()).arg(jerror.offset);
        rCode = ERROR_CODE_WRONG_PARAMETER;
    }

    QString cmd = rootObj.value("cmd").toString("");
    if (cmd.isEmpty()) {
        qWarning().noquote() << QString("No cmd found in JSON Statistics command");
        rCode = ERROR_CODE_MISSING_PARAMETER;
    }

    QJsonObject rootObjAnswer;
    if (rCode == ERROR_CODE_SUCCESS) {
        if (cmd == "overview") {
            QJsonArray parameter;
            parameter.append("Dauerkarten");
            parameter.append("Reservierungen");
            parameter.append("Treffen");
            parameter.append("Auswärtsfahrt");
            QJsonArray years;
            for (int i = 0; i < this->m_stats.size(); i++)
                years.append(this->m_stats.at(i)->m_year);

            rootObjAnswer.insert("parameter", parameter);
            rootObjAnswer.insert("years", years);

        } else if (cmd == "content") {

            StatsPerYear* pStats = NULL;
            qint32        year   = rootObj.value("year").toInt(g_GlobalData->m_currentSeason);
            QString       para   = rootObj.value("parameter").toString("");
            for (int i = 0; i < this->m_stats.size(); i++) {
                if (this->m_stats.at(i)->m_year == year) {
                    pStats = this->m_stats.at(i);
                    break;
                }
            }
            if (pStats == NULL)
                rCode = ERROR_CODE_NOT_POSSIBLE;
            else if (para == "Dauerkarten")
                this->handleSeasonTicketCommand(rootObjAnswer, pStats);
            else if (para == "Reservierungen")
                this->handleReservesCommand(rootObjAnswer, pStats);
            else if (para == "Treffen")
                this->handleMeetingCommand(rootObjAnswer, pStats);
            else if (para == "Auswärtsfahrt")
                this->handleAwayTripCommand(rootObjAnswer, pStats);
            else
                rCode = ERROR_CODE_NOT_POSSIBLE;
        } else
            rCode = ERROR_CODE_NOT_POSSIBLE;
    }

    rootObjAnswer.insert("type", "Statistic");
    rootObjAnswer.insert("cmd", cmd);
    rootObjAnswer.insert("ack", rCode);

    Q_UNUSED(pUserCon);
    //    qInfo().noquote() << QString("Handle statistics command from %1 with %2").arg(pUserCon->m_userName).arg(rCode);

    QByteArray answer = QJsonDocument(rootObjAnswer).toJson(QJsonDocument::Compact);

    return new MessageProtocol(OP_CODE_CMD_RES::ACK_CMD_STATISTIC, answer);
}

/* Answer
 * {
 *      "type": "Statistic",
 *      "cmd": "content"                    -> send content of chart
 *      "title": "Dauerkarten 17/18",       -> title of the chart
 *      "maxX": 12,                         -> max value of the X-Axis
 *      "categories": ["Name1", "Name2"]    -> names of the Y-Axis
 *      "bars": [{                          -> the bars to show
 *                  "title":"frei",
 *                  "color":"green",
 *                  "values": [1,2,3,4]
 *              }]
 * }
 */
qint32 cStatisticManager::handleSeasonTicketCommand(QJsonObject& rootObjAnswer, StatsPerYear* pStats)
{
    QJsonArray categories, blocked, reserved, free;
    qint32     maxX = 1;
    foreach (StatsTickets* pTicks, pStats->m_statsTickets) {
        QString name = pTicks->m_name;
        /* clean name to 8 characters and check if not already used */
        if (name.size() > 8)
            name.resize(8);
        qint32 index = 0;
        while (categories.contains(name)) {
            if (name.length() > 7)
                name.resize(7);
            name.append(QString::number(index++));
        }
        categories.append(name);

        blocked.append(pTicks->m_blocked);
        reserved.append(pTicks->m_reserved);
        free.append(pTicks->m_free);

        qint32 tmp = pTicks->m_blocked + pTicks->m_free + pTicks->m_reserved;
        if (tmp > maxX)
            maxX = tmp;
    }

    QJsonObject barBlocked, barReserved, barFree;
    barBlocked.insert("title", "Geht selbst");
    barReserved.insert("title", "Reserviert");
    barFree.insert("title", "Frei");
    barBlocked.insert("values", blocked);
    barReserved.insert("values", reserved);
    barFree.insert("values", free);
    barBlocked.insert("color", "red");
    barReserved.insert("color", "yellow");
    barFree.insert("color", "green");

    QJsonArray bars;
    bars.append(barBlocked);
    bars.append(barReserved);
    bars.append(barFree);

    qint32 currentSeason = pStats->m_year % 2000;
    rootObjAnswer.insert("categories", categories);
    rootObjAnswer.insert("bars", bars);
    rootObjAnswer.insert("title", QString("Dauerkarten %1/%2").arg(currentSeason).arg(currentSeason + 1));
    rootObjAnswer.insert("maxX", maxX);

    return ERROR_CODE_SUCCESS;
}

/* Answer
 * {
 *      "type": "Statistic",
 *      "cmd": "content"                    -> send content of chart
 *      "title": "Reservierungen 17/18",    -> title of the chart
 *      "maxX": 12,                         -> max value of the X-Axis
 *      "categories": ["Name1", "Name2"]    -> names of the Y-Axis
 *      "bars": [{                          -> the bars to show
 *                  "title":"reserviert",
 *                  "color":"orange",
 *                  "values": [1,2,3,4]
 *              }]
 * }
 */
qint32 cStatisticManager::handleReservesCommand(QJsonObject& rootObjAnswer, StatsPerYear* pStats)
{
    QJsonArray categories, reserved;
    qint32     maxX = 1;
    foreach (StatsReserved* pRes, pStats->m_reservedTicketNames) {
        categories.append(pRes->m_name);

        reserved.append(pRes->m_count);

        if (pRes->m_count > maxX)
            maxX = pRes->m_count;
    }

    QJsonObject barReserved;
    barReserved.insert("title", "reserviert von");
    barReserved.insert("values", reserved);
    barReserved.insert("color", "orange");

    QJsonArray bars;
    bars.append(barReserved);

    qint32 currentSeason = pStats->m_year % 2000;
    rootObjAnswer.insert("categories", categories);
    rootObjAnswer.insert("bars", bars);
    rootObjAnswer.insert("title", QString("Reservierungen %1/%2").arg(currentSeason).arg(currentSeason + 1));
    rootObjAnswer.insert("maxX", maxX);

    return ERROR_CODE_SUCCESS;
}

/* Answer
 * {
 *      "type": "Statistic",
 *      "cmd": "content"                    -> send content of chart
 *      "title": "Treffen 17/18",    -> title of the chart
 *      "maxX": 12,                         -> max value of the X-Axis
 *      "categories": ["Name1", "Name2"]    -> names of the Y-Axis
 *      "bars": [{                          -> the bars to show
 *                  "title":"Zugesagt",
 *                  "color":"green",
 *                  "values": [1,2,3,4]
 *              },
 *              {
 *                  "title":"Interesse",
 *                  "color":"yellow",
 *                  "values": [1,2,3,4]
 *              }]
 * }
 */
qint32 cStatisticManager::handleMeetingCommand(QJsonObject& rootObjAnswer, StatsPerYear* pStats)
{
    QJsonArray categories, accepted, interested;
    qint32     maxX = 1;
    foreach (StatsMeeting* pRes, pStats->m_meetingNames) {
        categories.append(pRes->m_name);

        accepted.append(pRes->m_accepted);
        interested.append(pRes->m_interested);

        if (pRes->m_accepted + pRes->m_interested > maxX)
            maxX = pRes->m_accepted + pRes->m_interested;
    }

    QJsonObject barReserved, barInterested;
    barReserved.insert("title", "Zugesagt");
    barReserved.insert("values", accepted);
    barReserved.insert("color", "green");
    barInterested.insert("title", "Interesse");
    barInterested.insert("values", interested);
    barInterested.insert("color", "yellow");

    QJsonArray bars;
    bars.append(barReserved);
    bars.append(barInterested);

    qint32 currentSeason = pStats->m_year % 2000;
    rootObjAnswer.insert("categories", categories);
    rootObjAnswer.insert("bars", bars);
    rootObjAnswer.insert("title", QString("Treffen %1/%2").arg(currentSeason).arg(currentSeason + 1));
    rootObjAnswer.insert("maxX", maxX);

    return ERROR_CODE_SUCCESS;
}

/* Answer
 * {
 *      "type": "Statistic",
 *      "cmd": "content"                    -> send content of chart
 *      "title": "Auswärtsfahrt 17/18",    -> title of the chart
 *      "maxX": 12,                         -> max value of the X-Axis
 *      "categories": ["Name1", "Name2"]    -> names of the Y-Axis
 *      "bars": [{                          -> the bars to show
 *                  "title":"Zugesagt",
 *                  "color":"green",
 *                  "values": [1,2,3,4]
 *              },
 *              {
 *                  "title":"Interesse",
 *                  "color":"yellow",
 *                  "values": [1,2,3,4]
 *              }]
 * }
 */
qint32 cStatisticManager::handleAwayTripCommand(QJsonObject& rootObjAnswer, StatsPerYear* pStats)
{
    QJsonArray categories, accepted, interested;
    qint32     maxX = 1;
    foreach (StatsMeeting* pRes, pStats->m_awayTripNames) {
        categories.append(pRes->m_name);

        accepted.append(pRes->m_accepted);
        interested.append(pRes->m_interested);

        if (pRes->m_accepted + pRes->m_interested > maxX)
            maxX = pRes->m_accepted + pRes->m_interested;
    }

    QJsonObject barReserved, barInterested;
    barReserved.insert("title", "Zugesagt");
    barReserved.insert("values", accepted);
    barReserved.insert("color", "green");
    barInterested.insert("title", "Interesse");
    barInterested.insert("values", interested);
    barInterested.insert("color", "yellow");

    QJsonArray bars;
    bars.append(barReserved);
    bars.append(barInterested);

    qint32 currentSeason = pStats->m_year % 2000;
    rootObjAnswer.insert("categories", categories);
    rootObjAnswer.insert("bars", bars);
    rootObjAnswer.insert("title", QString("Auswärtsfahrt %1/%2").arg(currentSeason).arg(currentSeason + 1));
    rootObjAnswer.insert("maxX", maxX);

    return ERROR_CODE_SUCCESS;
}
