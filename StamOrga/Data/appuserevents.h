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
#include <QtCore/QJsonObject>

#include "globaldata.h"
#include "userinterface.h"

struct EventInfo {
    QString m_type;
    QString m_info;
    qint64  m_eventID;
};

class AppUserEvents : public QObject
{
    Q_OBJECT
public:
    explicit AppUserEvents(QObject* parent = nullptr);

    void initialize(GlobalData* pGlobalData);

    void addNewUserEvents(QJsonObject& jsObj);

    void resetCurrentEvents();

    Q_INVOKABLE qint32 getCurrentMainEventCounter();
    Q_INVOKABLE qint32 getCurrentUpdateEventCounter();
    Q_INVOKABLE qint32 getCurrentFanclubEventCounter();

    Q_INVOKABLE qint32 clearUserEventFanclub(UserInterface* pInt, qint32 newsIndex);
    Q_INVOKABLE qint32 clearUserEventGamPlay(UserInterface* pInt, qint32 gameIndex);
    Q_INVOKABLE qint32 clearUserEventUpdate(UserInterface* pInt);

signals:

public slots:

private:
    bool   m_eventNewAppVersion;
    qint32 m_eventNewFanclubNews;

    GlobalData*       m_pGlobalData;
    QList<EventInfo*> m_lEvents;
};

#endif // APPUSEREVENTS_H
