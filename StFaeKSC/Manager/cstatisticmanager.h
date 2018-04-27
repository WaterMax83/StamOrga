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

#ifndef CSTATISTICMANAGER_H
#define CSTATISTICMANAGER_H

#include <QList>
#include <QMutex>
#include <QObject>
#include <QTimer>

#include "../Common/General/backgroundcontroller.h"
#include "../Common/General/backgroundworker.h"
#include "../Common/Network/messageprotocol.h"
#include "../Network/connectiondata.h"


struct StatsTickets {
    StatsTickets()
    {
        this->m_ticketIndex = -1;
        this->m_free        = 0;
        this->m_reserved    = 0;
        this->m_blocked     = 0;
        this->m_timestamp   = 0;
    }
    qint32  m_ticketIndex;
    QString m_name;
    qint32  m_free;
    qint32  m_reserved;
    qint32  m_blocked;
    qint64  m_timestamp;

    static bool compareCountFunctionAscending(StatsTickets* p1, StatsTickets* p2)
    {
        qint32 cnt1 = p1->m_blocked + p1->m_free + p1->m_reserved;
        qint32 cnt2 = p2->m_blocked + p2->m_free + p2->m_reserved;
        if (cnt1 > cnt2)
            return false;
        else if (cnt1 == cnt2) {
            if (p1->m_blocked > p2->m_blocked)
                return false;
            else if (p1->m_blocked == p2->m_blocked) {
                if (p1->m_reserved > p2->m_reserved)
                    return false;
                else if (p1->m_reserved == p2->m_reserved) {
                    if (p1->m_free > p2->m_free)
                        return false;
                    else if (p1->m_free == p2->m_free) {
                        if (p1->m_name.compare(p2->m_name) > 0)
                            return false;
                    }
                }
            }
        }
        return true;
    }
};

struct StatsReserved {
    QString m_name  = "";
    qint32  m_count = 0;

    static bool compareCountFunctionAscending(StatsReserved* p1, StatsReserved* p2)
    {
        if (p1->m_count > p2->m_count)
            return false;
        else if (p1->m_count == p2->m_count) {
            if (p1->m_name.compare(p2->m_name) > 0)
                return false;
        }
        return true;
    }
};

struct StatsMeeting {
    QString m_name       = "";
    qint32  m_accepted   = 0;
    qint32  m_interested = 0;

    static bool compareCountFunctionAscending(StatsMeeting* p1, StatsMeeting* p2)
    {
        qint32 cnt1 = p1->m_accepted + p1->m_interested;
        qint32 cnt2 = p2->m_accepted + p2->m_interested;
        if (cnt1 > cnt2)
            return false;
        else if (cnt1 == cnt2) {
            if (p1->m_accepted > p2->m_accepted)
                return false;
            else if (p1->m_accepted == p2->m_accepted) {
                if (p1->m_interested > p2->m_interested)
                    return false;
                else if (p1->m_interested == p2->m_interested) {
                    if (p1->m_name.compare(p2->m_name) > 0)
                        return false;
                }
            }
        }
        return true;
    }
};

struct StatsPerYear {
    qint32                m_year;
    QList<StatsTickets*>  m_statsTickets;
    QList<StatsReserved*> m_reservedTicketNames;
    QList<StatsMeeting*>  m_meetingNames;
    QList<StatsMeeting*>  m_awayTripNames;
};

class cStatisticManager : public BackgroundWorker
{
    Q_OBJECT
public:
    explicit cStatisticManager(QObject* parent = nullptr);

    int initialize();

    MessageProtocol* handleStatisticCommand(UserConData* pUserCon, MessageProtocol* request);

    qint32 addYearToStatistic(qint32 year);

signals:
    void signalNewYearAdded();

private slots:
    void slotCycleTimerFired();
    void slotNewYearAdded();

protected:
    int DoBackgroundWork() override;

    BackgroundController* m_bckGrndCtrl;
    QTimer*               m_cycleTimer;
    QMutex                m_statsMutex;
    QList<StatsPerYear*>  m_stats;

    void calculateStatsForAllYears();

    QString cleanName(QString name);

    qint32 handleSeasonTicketCommand(QJsonObject& rootObjAnswer, StatsPerYear* pStats);
    qint32 handleReservesCommand(QJsonObject& rootObjAnswer, StatsPerYear* pStats);
    qint32 handleMeetingCommand(QJsonObject& rootObjAnswer, StatsPerYear* pStats);
    qint32 handleAwayTripCommand(QJsonObject& rootObjAnswer, StatsPerYear* pStats);
};

extern cStatisticManager g_StatisticManager;

#endif // CSTATISTICMANAGER_H
