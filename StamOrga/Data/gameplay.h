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

#ifndef GAMEPLAY_H
#define GAMEPLAY_H

#include <QtCore/QDateTime>
#include <QtCore/QObject>

class GamePlay : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString home READ home WRITE setHome NOTIFY homeChanged)
    Q_PROPERTY(QString away READ away WRITE setAway NOTIFY awayChanged)
    Q_PROPERTY(QString score READ score WRITE setScore NOTIFY scoreChanged)
    Q_PROPERTY(QString timestamp READ timestamp NOTIFY timestampChanged)
    Q_PROPERTY(QString competition READ competition NOTIFY competitionChanged)
    Q_PROPERTY(quint32 index READ index WRITE setIndex NOTIFY indexChanged)
    Q_PROPERTY(quint8 seasonIndex READ seasonIndex WRITE setSeasonIndex NOTIFY seasonIndexChanged)
public:
    explicit GamePlay(QObject* parent = 0);

    QString home() { return this->m_home; }
    void setHome(const QString& home)
    {
        if (this->m_home != home) {
            this->m_home = home;
            emit homeChanged();
        }
    }

    QString away() { return this->m_away; }
    void setAway(const QString& away)
    {
        if (this->m_away != away) {
            this->m_away = away;
            emit awayChanged();
        }
    }

    QString score() { return this->m_score; }
    void setScore(const QString& score)
    {
        if (this->m_score != score) {
            this->m_score = score;
            emit scoreChanged();
        }
    }

    QString timestamp()
    {
        return QDateTime::fromMSecsSinceEpoch(this->m_timestamp).toString("ddd, dd.MM.yy hh:mm");
    }
    qint64 timestamp64Bit()
    {
        return this->m_timestamp;
    }
    void setTimeStamp(qint64 ts)
    {
        if (this->m_timestamp != ts) {
            this->m_timestamp = ts;
            emit this->timestampChanged();
        }
    }

    QString competition()
    {
        switch (this->m_comp) {
        case 1:
            return "1 Bundesliga";
        case 2:
            return "2 Bundesliga";
        default:
            return "Unknown";
        }
    }
    quint8 compValue()
    {
        return this->m_comp;
    }

    void setCompetition(quint8 co)
    {
        if (this->m_comp != co) {
            this->m_comp = co;
            emit this->competitionChanged();
        }
    }

    quint8 index() { return this->m_index; }
    void setIndex(const quint8 index)
    {
        if (this->m_index != index) {
            this->m_index = index;
            emit this->indexChanged();
        }
    }

    quint8 seasonIndex() { return this->m_seasonIndex; }
    void setSeasonIndex(const quint8 seasonIndex)
    {
        if (this->m_seasonIndex != seasonIndex) {
            this->m_seasonIndex = seasonIndex;
            emit this->seasonIndexChanged();
        }
    }

    Q_INVOKABLE QString getCompetitionIndex()
    {
        if (this->m_comp == 1 || this->m_comp == 2) {
            return QString("%1. Spieltag").arg(this->m_seasonIndex);
        }
        return "not implemented";
    }

    Q_INVOKABLE quint16 getFreeTickets() { return this->m_freeTickets; }
    void setFreeTickets(quint16 number)
    {
        this->m_freeTickets = number;
    }

    void setBlockedTickets(quint16 number)
    {
        this->m_blockedTickets = number;
    }

    void setReservedTickets(quint16 number)
    {
        this->m_reservedTickets = number;
    }

    Q_INVOKABLE bool isGameInPast();
    Q_INVOKABLE bool isGameRunning();

    Q_INVOKABLE bool isGameAHomeGame();
    Q_INVOKABLE bool isGameASeasonTicketGame();


signals:
    void homeChanged();
    void awayChanged();
    void scoreChanged();
    void timestampChanged();
    void competitionChanged();
    void indexChanged();
    void seasonIndexChanged();

public slots:

private:
    QString m_home;
    QString m_away;
    QString m_score;
    quint8  m_comp;
    quint32 m_index;
    quint8  m_seasonIndex;
    qint64  m_timestamp;
    quint16 m_freeTickets;
    quint16 m_blockedTickets;
    quint16 m_reservedTickets;
};

#endif // GAMEPLAY_H
