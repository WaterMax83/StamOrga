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

#ifndef ACCEPTMEETINGINFO_H
#define ACCEPTMEETINGINFO_H

#include <QObject>

class AcceptMeetingInfo : public QObject
{
    Q_OBJECT
public:
    explicit AcceptMeetingInfo(QObject* parent = 0)
        : QObject(parent)
    {
    }

    Q_INVOKABLE quint32 index() { return this->m_index; }
    void setIndex(const quint32 index)
    {
        if (this->m_index != index)
            this->m_index = index;
    }

    Q_INVOKABLE quint32 value() { return this->m_value; }
    void setValue(const quint32 value)
    {
        if (this->m_value != value)
            this->m_value = value;
    }

    Q_INVOKABLE QString name() { return this->m_name; }
    void setName(const QString& name)
    {
        if (this->m_name != name)
            this->m_name = name;
    }


signals:

public slots:

private:
    quint32 m_index;
    quint32 m_value;
    QString m_name;
};

#endif // ACCEPTMEETINGINFO_H
