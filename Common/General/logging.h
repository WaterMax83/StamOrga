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
#include <QtCore/QTimer>

#ifdef STAMORGA_APP
#include "../../Common/General/backgroundworker.h"
#else
#include "../Common/General/backgroundworker.h"
#endif

//void logMyMessageLogginOutPut(QtMsgType type, const QMessageLogContext& context, const QString& msg);

struct LogEntry {
    QtMsgType m_type;
    //    QMessageLogContext m_context;
    QString m_message;
};

class Logging : public BackgroundWorker
{
    Q_OBJECT
public:
    explicit Logging(QObject* parent = 0);

    void initialize();

    void addNewEntry(QtMsgType type, /*const QMessageLogContext context,*/ const QString& msg);

    int showLoggingInfo(quint16 numbOfLines);

    void clearCurrentLoggingList(int index);

    QString getCurrentLoggingList(int index);

    QStringList getLogFileDates();

    void terminate();


signals:
    void signalNewLogEntries();

private slots:
    void slotNewLogEntries();
    void slotEveryHourTimeout();

protected:
    int DoBackgroundWork();

private:
    QFile*           m_logFile;
    QList<LogEntry*> m_logEntries;
    QMutex           m_internalMutex;
    QMutex           m_entryListMutex;
    QString          m_currentLogging;
    QTimer*          m_hourTimer;

    QList<QString> m_lastLogFiles;

    QString createLoggingFilePath();
};


#endif // LOGGING_H
