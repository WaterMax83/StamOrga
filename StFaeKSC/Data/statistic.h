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
#include <QObject>
#include <QTimer>

#include "../Common/General/backgroundcontroller.h"
#include "../Common/General/backgroundworker.h"


struct StatsTickets {
    StatsTickets()
    {
        this->m_index     = -1;
        this->m_free      = 0;
        this->m_reserved  = 0;
        this->m_used      = 0;
        this->m_timestamp = 0;
    }
    qint32  m_index;
    QString m_name;
    qint32  m_free;
    qint32  m_reserved;
    qint32  m_used;
    qint64  m_timestamp;
};

class Statistic : public BackgroundWorker
{
    Q_OBJECT
public:
    explicit Statistic(QObject* parent = nullptr);

    int initialize();

signals:

private slots:
    void slotCycleTimerFired();

protected:
    int DoBackgroundWork() override;

    BackgroundController* m_bckGrndCtrl;
    QTimer*               m_cycleTimer;
    QList<StatsTickets*>  m_statsTickets;
};

#endif // CYCLECHECK_H
