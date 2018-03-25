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
#include "cstatisticmanager.h"
#include "../Common/Network/messagecommand.h"

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
#ifdef QT_DEBUG
    this->m_cycleTimer->start(1000);
#else
    this->m_cycleTimer->start(10000);
#endif

    return ERROR_CODE_SUCCESS;
}

void cStatisticManager::slotCycleTimerFired()
{
    QMutexLocker lock0(&g_GlobalData->m_globalDataMutex);
    QMutexLocker lock1(&this->m_statsMutex);

    /* Clear all info */
    foreach (StatsTickets* pStats, this->m_statsTickets)
        delete pStats;
    this->m_statsTickets.clear();

    foreach (StatsReserved* pRes, this->m_reservedTicketNames)
        delete pRes;
    this->m_reservedTicketNames.clear();

    foreach (StatsMeeting* pMeet, this->m_meetingNames)
        delete pMeet;
    this->m_meetingNames.clear();
    foreach (StatsMeeting* pMeet, this->m_awayTripNames)
        delete pMeet;
    this->m_awayTripNames.clear();


    /* First collect all tickets */
    for (int i = 0; i < g_GlobalData->m_SeasonTicket.getNumberOfInternalList(); i++) {
        TicketInfo* pTicket = (TicketInfo*)g_GlobalData->m_SeasonTicket.getRequestConfigItemFromListIndex(i);
        if (pTicket == NULL)
            continue;

        StatsTickets* pStats  = new StatsTickets();
        pStats->m_ticketIndex = pTicket->m_index;
        pStats->m_name        = pTicket->m_itemName;
        pStats->m_timestamp   = pTicket->m_creation;

        this->m_statsTickets.append(pStats);
    }

    qint64 currentTimeStamp = QDateTime::currentMSecsSinceEpoch();

    /* get all games to get the number of tickets for blocked */
    for (int i = 0; i < g_GlobalData->m_GamesList.getNumberOfInternalList(); i++) {
        GamesPlay* pGame = (GamesPlay*)g_GlobalData->m_GamesList.getRequestConfigItemFromListIndex(i);
        if (pGame == NULL || pGame->m_saison != g_GlobalData->m_currentSeason || pGame->m_timestamp > currentTimeStamp)
            continue;

        if (pGame->m_itemName != "KSC" || pGame->m_competition > LIGA_3 || pGame->m_competition < BUNDESLIGA_1)
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

        if (pGame->m_itemName != "KSC" || pGame->m_competition > LIGA_3 || pGame->m_competition < BUNDESLIGA_1)
            continue;

        for (int i = 0; i < pAvTick->getNumberOfInternalList(); i++) {
            AvailableTicketInfo* pTicket = (AvailableTicketInfo*)pAvTick->getRequestConfigItemFromListIndex(i);
            if (pTicket == NULL)
                continue;

            /* add counter for free or reserved */
            foreach (StatsTickets* pStats, this->m_statsTickets) {
                if (pStats->m_ticketIndex != pTicket->m_ticketID)
                    continue;

                if (pTicket->m_state != TICKET_STATE_BLOCKED && pStats->m_blocked > 0) {
                    pStats->m_blocked--;
                    if (pTicket->m_state == TICKET_STATE_FREE)
                        pStats->m_free++;
                    else if (pTicket->m_state == TICKET_STATE_RESERVED)
                        pStats->m_reserved++;
                }
            }

            if (pTicket->m_state == TICKET_STATE_RESERVED) {
                bool bFoundItem = false;
                foreach (StatsReserved* pRes, this->m_reservedTicketNames) {
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
                    this->m_reservedTicketNames.append(pRes);
                }
            }
        }
    }

    foreach (MeetingInfo* pMeetingInfo, g_GlobalData->m_meetingInfos) {
        GamesPlay* pGame = (GamesPlay*)g_GlobalData->m_GamesList.getItem(pMeetingInfo->getGameIndex());

        if (pGame == NULL || pGame->m_saison != g_GlobalData->m_currentSeason || pGame->m_timestamp > currentTimeStamp)
            continue;

        for (int i = 0; i < pMeetingInfo->getNumberOfInternalList(); i++) {
            AcceptMeetingInfo* pInfo = (AcceptMeetingInfo*)pMeetingInfo->getRequestConfigItemFromListIndex(i);
            if (pInfo == NULL)
                continue;

            bool bFoundItem = false;
            foreach (StatsMeeting* pMeet, this->m_meetingNames) {
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
                this->m_meetingNames.append(pMeet);
            }
        }
    }

    foreach (AwayTripInfo* pMeetingInfo, g_GlobalData->m_awayTripInfos) {
        GamesPlay* pGame = (GamesPlay*)g_GlobalData->m_GamesList.getItem(pMeetingInfo->getGameIndex());

        if (pGame == NULL || pGame->m_saison != g_GlobalData->m_currentSeason || pGame->m_timestamp > currentTimeStamp)
            continue;

        for (int i = 0; i < pMeetingInfo->getNumberOfInternalList(); i++) {
            AcceptMeetingInfo* pInfo = (AcceptMeetingInfo*)pMeetingInfo->getRequestConfigItemFromListIndex(i);
            if (pInfo == NULL)
                continue;

            bool bFoundItem = false;
            foreach (StatsMeeting* pMeet, this->m_awayTripNames) {
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
                this->m_awayTripNames.append(pMeet);
            }
        }
    }


    std::sort(this->m_statsTickets.begin(), this->m_statsTickets.end(), StatsTickets::compareCountFunctionAscending);
    std::sort(this->m_reservedTicketNames.begin(), this->m_reservedTicketNames.end(), StatsReserved::compareCountFunctionAscending);
    std::sort(this->m_meetingNames.begin(), this->m_meetingNames.end(), StatsMeeting::compareCountFunctionAscending);
    std::sort(this->m_awayTripNames.begin(), this->m_awayTripNames.end(), StatsMeeting::compareCountFunctionAscending);

    this->m_cycleTimer->start(6 * 60 * 60 * 1000);
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

    qint32 rCode = ERROR_CODE_SUCCESS;
    QJsonParseError jerror;
    QByteArray  data    = QByteArray(request->getPointerToData());
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

            rootObjAnswer.insert("parameter", parameter);

        } else if (cmd == "content") {

            QString para = rootObj.value("parameter").toString("");
            if (para == "Dauerkarten")
                this->handleSeasonTicketCommand(rootObjAnswer);
            else if (para == "Reservierungen")
                this->handleReservesCommand(rootObjAnswer);
            else if (para == "Treffen")
                this->handleMeetingCommand(rootObjAnswer);
            else if (para == "Auswärtsfahrt")
                this->handleAwayTripCommand(rootObjAnswer);
            else
                rCode = ERROR_CODE_NOT_POSSIBLE;
        } else
            rCode = ERROR_CODE_NOT_POSSIBLE;
    }

    rootObjAnswer.insert("type", "Statistic");
    rootObjAnswer.insert("cmd", cmd);
    rootObjAnswer.insert("ack", rCode);

    qInfo().noquote() << QString("Handle statistics command from %1 with %2").arg(pUserCon->m_userName).arg(rCode);

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
qint32 cStatisticManager::handleSeasonTicketCommand(QJsonObject& rootObjAnswer)
{
    QJsonArray categories, blocked, reserved, free;
    qint32     maxX = 0;
    foreach (StatsTickets* pStats, this->m_statsTickets) {
        QString name = pStats->m_name;
        /* clean name to 8 characters and check if not already used */
        if (name.size() > 8)
            name.resize(8);
        qint32 index = 0;
        while (categories.contains(name)) {
            name.resize(7);
            name.append(QString::number(index++));
        }
        categories.append(name);

        blocked.append(pStats->m_blocked);
        reserved.append(pStats->m_reserved);
        free.append(pStats->m_free);

        qint32 tmp = pStats->m_blocked + pStats->m_free + pStats->m_reserved;
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

    qint32 currentSeason = g_GlobalData->m_currentSeason % 2000;
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
qint32 cStatisticManager::handleReservesCommand(QJsonObject& rootObjAnswer)
{
    QJsonArray categories, reserved;
    qint32     maxX = 0;
    foreach (StatsReserved* pRes, this->m_reservedTicketNames) {
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

    qint32 currentSeason = g_GlobalData->m_currentSeason % 2000;
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
qint32 cStatisticManager::handleMeetingCommand(QJsonObject& rootObjAnswer)
{
    QJsonArray categories, accepted, interested;
    qint32     maxX = 0;
    foreach (StatsMeeting* pRes, this->m_meetingNames) {
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

    qint32 currentSeason = g_GlobalData->m_currentSeason % 2000;
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
qint32 cStatisticManager::handleAwayTripCommand(QJsonObject& rootObjAnswer)
{
    QJsonArray categories, accepted, interested;
    qint32     maxX = 0;
    foreach (StatsMeeting* pRes, this->m_awayTripNames) {
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

    qint32 currentSeason = g_GlobalData->m_currentSeason % 2000;
    rootObjAnswer.insert("categories", categories);
    rootObjAnswer.insert("bars", bars);
    rootObjAnswer.insert("title", QString("Auswärtsfahrt %1/%2").arg(currentSeason).arg(currentSeason + 1));
    rootObjAnswer.insert("maxX", maxX);

    return ERROR_CODE_SUCCESS;
}
