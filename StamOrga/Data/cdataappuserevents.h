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

#ifndef APPUSEREVENTS_H
#define APPUSEREVENTS_H

#include <QObject>
#include <QtCore/QJsonArray>
#include <QtCore/QJsonObject>

#include "../Common/General/cgendisposer.h"
//#include "globaldata.h"
#include "userinterface.h"

struct EventInfo {
    QString m_type;
    QString m_info;
    qint64  m_eventID;
};

class cDataAppUserEvents : public cGenDisposer
{
    Q_OBJECT
public:
    explicit cDataAppUserEvents(QObject* parent = nullptr);

    qint32 initialize();

    qint32 addNewUserEvents(QJsonArray& jsArr);

    void resetCurrentEvents();

    Q_INVOKABLE qint32 getCurrentMainEventCounter();
    Q_INVOKABLE qint32 getCurrentUpdateEventCounter();
    Q_INVOKABLE qint32 getCurrentFanclubEventCounter();

    Q_INVOKABLE qint32 clearUserEventFanclub(qint32 newsIndex);
    Q_INVOKABLE qint32 clearUserEventGamPlay(qint32 gameIndex);
    Q_INVOKABLE qint32 clearUserEventUpdate();

signals:

public slots:

private:
    bool   m_eventNewAppVersion;
    qint32 m_eventNewFanclubNews;

    //    GlobalData*       m_pGlobalData;
    QList<EventInfo*> m_lEvents;
};

extern cDataAppUserEvents g_DataAppUserEvents;

#endif // APPUSEREVENTS_H
