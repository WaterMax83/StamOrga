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

#ifndef NEWSDATAITEM_H
#define NEWSDATAITEM_H

#include <QtCore/QDateTime>
#include <QtCore/QObject>

class NewsDataItem : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString user READ user WRITE setUser NOTIFY userChanged)
    Q_PROPERTY(QString header READ header WRITE setHeader NOTIFY headerChanged)
    Q_PROPERTY(quint32 index READ index WRITE setIndex NOTIFY indexChanged)
    Q_PROPERTY(QString info READ info WRITE setInfo NOTIFY infoChanged)
    Q_PROPERTY(bool event READ isEvent NOTIFY eventChanged)
public:
    explicit NewsDataItem(QObject* parent = 0);

    QString user() { return this->m_user; }
    void setUser(const QString& user)
    {
        if (this->m_user != user) {
            this->m_user = user;
            emit userChanged();
        }
    }

    QString header() { return this->m_header; }
    void setHeader(const QString& header)
    {
        if (this->m_header != header) {
            this->m_header = header;
            emit headerChanged();
        }
    }

    QString info() { return this->m_info; }
    void setInfo(const QString& info)
    {
        if (this->m_info != info) {
            this->m_info = info;
            emit infoChanged();
        }
    }

    quint32 index() { return this->m_index; }
    void setIndex(const quint32 index)
    {
        if (this->m_index != index) {
            this->m_index = index;
            emit this->indexChanged();
        }
    }

    bool isEvent() { return this->m_bIsEvent; }
    void setIsEvent(bool event)
    {
        if (this->m_bIsEvent != event) {
            this->m_bIsEvent = event;
            emit this->eventChanged();
        }
    }

    void setTimeStamp(qint64 timestamp)
    {
        this->m_timestamp = timestamp;
    }
    Q_INVOKABLE QString timestampReadableLine()
    {
        return QDateTime::fromMSecsSinceEpoch(this->m_timestamp).toString("ddd, dd.MM.yy hh:mm");
    }

    static bool compareTimeStampFunctionDescending(NewsDataItem* p1, NewsDataItem* p2);

signals:
    void userChanged();
    void headerChanged();
    void infoChanged();
    void indexChanged();
    void eventChanged();

public slots:

private:
    QString m_user;
    QString m_header;
    QString m_info;
    quint32 m_index;
    qint64  m_timestamp;
    bool    m_bIsEvent;
};

#endif // NEWSDATAITEM_H
