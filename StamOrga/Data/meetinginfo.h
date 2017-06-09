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


#ifndef MEETINGINFO_H
#define MEETINGINFO_H

#include <QtCore/QList>
#include <QtCore/QMutex>
#include <QtCore/QObject>

#include "acceptmeetinginfo.h"

class MeetingInfo : public QObject
{
    Q_OBJECT
public:
    explicit MeetingInfo(QObject* parent = 0);

    Q_INVOKABLE QString when() { return this->m_when; }
    Q_INVOKABLE QString where() { return this->m_where; }
    Q_INVOKABLE QString info() { return this->m_info; }

    void setWhen(QString when) { this->m_when = when; }
    void setWhere(QString where) { this->m_where = where; }
    void setInfo(QString info) { this->m_info = info; }

    Q_INVOKABLE AcceptMeetingInfo* getAcceptInfoFromIndex(quint32 index)
    {
        QMutexLocker lock(&this->m_listMutex);

        if (qint32(index) >= this->m_acceptInfo.size())
            return NULL;

        return this->m_acceptInfo[index];
    }

    qint32 addNewAcceptInfo(AcceptMeetingInfo* info)
    {
        QMutexLocker lock(&this->m_listMutex);

        for (int i = 0; i < this->m_acceptInfo.size(); i++) {
            if (this->m_acceptInfo[i]->index() == info->index())
                return -1;
        }

        this->m_acceptInfo.append(info);
        return 1;
    }

    void clearAcceptInfoList()
    {
        QMutexLocker lock(&this->m_listMutex);

        this->m_acceptInfo.clear();
    }


private:
    QString                   m_when;
    QString                   m_where;
    QString                   m_info;
    QList<AcceptMeetingInfo*> m_acceptInfo;
    QMutex                    m_listMutex;
};

#endif // MEETINGINFO_H
