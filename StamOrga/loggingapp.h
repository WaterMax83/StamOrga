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

#ifndef LOGGING_H
#define LOGGING_H

#include <QtCore/QFile>
#include <QtCore/QList>
#include <QtCore/QMutex>
#include <QtCore/QObject>

#include "../Common/General/backgroundcontroller.h"
#include "../Common/General/backgroundworker.h"
#include "Data/globaldata.h"

class LoggingApp : public BackgroundWorker
{
    Q_OBJECT
public:
    explicit LoggingApp(QObject* parent = 0);

    void initialize(GlobalData* pData);

    void terminate();

signals:
    void signalNewLogEntries();

public slots:
    void slotNewLogEntries();

protected:
    int DoBackgroundWork();

    void stamOrgaMessageOutput(QtMsgType type, const QMessageLogContext& context, const QString& msg);

private:
    QFile*          m_logFile;
    GlobalData*     m_globalData;
    QList<QString*> m_logEntries;
    QMutex          m_mutex;
};

#endif // LOGGING_H
