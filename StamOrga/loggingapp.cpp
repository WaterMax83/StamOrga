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

#include <QtCore/QStandardPaths>
#include <QtCore/QtGlobal>

#include "../../Common/General/globalfunctions.h"
#include "loggingapp.h"


//LoggingApp* gLoggingApp;

//static void stamOrgaMessageOutput(QtMsgType type, const QMessageLogContext& context, const QString& msg)
//{
//    QString* newMessage = new QString();
//    newMessage->append(qFormatLogMessage(type, context, msg));

//    {
//        QMutexLocker lock(&gLoggingApp->m_mutex);
//        gLoggingApp->m_logEntries.append(newMessage);
//    }

//    emit gLoggingApp->signalNewLogEntries();
//}


LoggingApp::LoggingApp(QObject* parent)
    : BackgroundWorker(parent)
{
}

void LoggingApp::initialize(GlobalData* pData)
{
    this->m_globalData = pData;

    // Set the global Message Pattern
    SetMessagePattern();
#ifdef QT_DEBUG
#ifdef Q_OS_ANDROID
//    qInstallMessageHandler(stamOrgaMessageOutput);
#endif
#endif
}


int LoggingApp::DoBackgroundWork()
{
#ifdef QT_DEBUG
#ifdef Q_OS_ANDROID
    connect(this, &LoggingApp::signalNewLogEntries, this, &LoggingApp::slotNewLogEntries);
#endif
#endif
    qInfo() << QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    return 0;
}

void LoggingApp::slotNewLogEntries()
{
    while (true) {

        this->m_mutex.lock();
        if (this->m_logEntries.isEmpty()) {
            this->m_mutex.unlock();
            break;
        }

        QString* entry = this->m_logEntries.first();
        this->m_logEntries.removeFirst();
        this->m_mutex.unlock();

        //        this->m_logFile->write(this->createLogLine(entry));
        this->m_globalData->addNewLoggingMessage(entry);
        delete entry;
    }

    this->m_logFile->flush();
}

void LoggingApp::terminate()
{
#ifdef QT_DEBUG
#ifdef Q_OS_ANDROID
    disconnect(this, &LoggingApp::signalNewLogEntries, this, &LoggingApp::slotNewLogEntries);
#endif
#endif
}
