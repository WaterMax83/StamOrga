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

#ifndef SEASONTICKET_H
#define SEASONTICKET_H

#include <QtCore/QDateTime>
#include <QtCore/QObject>

#include "../../Common/General/globalfunctions.h"

class SeasonTicketItem : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
    Q_PROPERTY(QString place READ place WRITE setPlace NOTIFY placeChanged)
    Q_PROPERTY(quint8 discount READ discount WRITE setDiscount NOTIFY discountChanged)
    Q_PROPERTY(qint32 index READ index NOTIFY indexChanged)
    Q_PROPERTY(qint32 userIndex READ userIndex NOTIFY userIndexChanged)
public:
    explicit SeasonTicketItem(QObject* parent = 0);

    QString name() { return this->m_name; }
    void setName(const QString& name)
    {
        if (this->m_name != name) {
            this->m_name = name;
            emit nameChanged();
        }
    }

    QString place() { return this->m_place; }
    void setPlace(const QString& place)
    {
        if (this->m_place != place) {
            this->m_place = place;
            emit placeChanged();
        }
    }

    quint8 discount() { return this->m_discount; }
    void setDiscount(const quint8 discount)
    {
        if (this->m_discount != discount) {
            this->m_discount = discount;
            emit this->discountChanged();
        }
    }

    qint32 index() { return this->m_index; }
    void setIndex(const qint32 index)
    {
        if (this->m_index != index) {
            this->m_index = index;
            emit this->indexChanged();
        }
    }

    qint32 userIndex() { return this->m_userIndex; }
    void setUserIndex(const qint32 userIndex)
    {
        if (this->m_userIndex != userIndex) {
            this->m_userIndex = userIndex;
            emit this->userIndexChanged();
        }
    }

    Q_INVOKABLE bool isTicketYourOwn() { return this->m_ownTicket; }
    void checkTicketOwn(qint32 userIndex)
    {
        if (this->m_userIndex == userIndex)
            this->m_ownTicket = true;
        else
            this->m_ownTicket = false;
    }

    Q_INVOKABLE quint32 getTicketState()
    {
        return this->m_ticketState;
    }
    void setTicketState(quint32 state)
    {
        this->m_ticketState = state;
        if (state != 3)
            this->m_ticketReserveName = "";
    }

    Q_INVOKABLE QString getTicketReserveName()
    {
        return this->m_ticketReserveName;
    }
    void setReserveName(QString name)
    {
        this->m_ticketReserveName = name;
    }


signals:
    void nameChanged();
    void placeChanged();
    void discountChanged();
    void indexChanged();
    void userIndexChanged();

public slots:

private:
    QString m_name;
    QString m_place;
    quint8  m_discount;
    qint32  m_index;
    qint32  m_userIndex;
    bool    m_ownTicket;

    quint32 m_ticketState;
    QString m_ticketReserveName;
};

#endif // SEASONTICKET_H
