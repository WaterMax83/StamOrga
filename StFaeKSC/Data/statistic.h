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

#ifndef CYCLECHECK_H
#define CYCLECHECK_H

#include <QList>
#include <QMutex>
#include <QObject>
#include <QTimer>

#include "../Common/General/backgroundcontroller.h"
#include "../Common/General/backgroundworker.h"


struct StatsTickets {
    StatsTickets()
    {
        this->m_ticketIndex = -1;
        this->m_free        = 0;
        this->m_reserved    = 0;
        this->m_blocked     = 0;
        this->m_timestamp   = 0;
    }
    quint32 m_ticketIndex;
    QString m_name;
    qint32  m_free;
    qint32  m_reserved;
    qint32  m_blocked;
    qint64  m_timestamp;
};

struct StatsReserved {
    QString m_name  = "";
    qint32  m_count = 0;

    static bool compareCountFunctionAscending(StatsReserved* p1, StatsReserved* p2)
    {
        if (p1->m_count > p2->m_count)
            return false;
        return true;
    }
};

class Statistic : public BackgroundWorker
{
    Q_OBJECT
public:
    explicit Statistic(QObject* parent = nullptr);

    int initialize();

    qint32 handleStatisticCommand(QByteArray& cmd, QByteArray& answer);

signals:

private slots:
    void slotCycleTimerFired();

protected:
    int DoBackgroundWork() override;

    BackgroundController* m_bckGrndCtrl;
    QTimer*               m_cycleTimer;
    QList<StatsTickets*>  m_statsTickets;
    QMutex                m_statsMutex;
    QList<StatsReserved*> m_reservedTicketNames;

    qint32 handleSeasonTicketCommand(QJsonObject& rootObjAnswer);
    qint32 handleReservesCommand(QJsonObject& rootObjAnswer);
};

#endif // CYCLECHECK_H
