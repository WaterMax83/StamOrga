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

#include <QtCore/QDateTime>
#include <QtCore/QTextStream>

#include <iostream>

#include "../../Common/General/globalfunctions.h"
#include "logging.h"

#define LOG_FILE_DATE_FORMAT "yyyy_MM_dd"

#ifdef Q_OS_ANDROID
#define MAX_DAYS_LOG_FILES 5
#else
#define MAX_DAYS_LOG_FILES 30
#endif

Logging* g_Logging = NULL;

void logMyMessageLogginOutPut(QtMsgType type, const QMessageLogContext& context, const QString& msg)
{
    Q_UNUSED(context);

    if (g_Logging != NULL)
        g_Logging->addNewEntry(type, msg);
}


Logging::Logging(QObject* parent)
    : BackgroundWorker(parent)
{
    this->m_logFile   = NULL;
    this->m_hourTimer = NULL;
}

void Logging::initialize()
{
    // Set the global Message Pattern
    SetMessagePattern();

    g_Logging = this;

    qInstallMessageHandler(logMyMessageLogginOutPut);
}

int Logging::DoBackgroundWork()
{

    this->m_hourTimer = new QTimer();
    this->m_hourTimer->setSingleShot(true);
    connect(this->m_hourTimer, &QTimer::timeout, this, &Logging::slotEveryHourTimeout);

    this->slotEveryHourTimeout();

    QString loggingPath = this->createLoggingFilePath();

    if (loggingPath.size() > 0) {

        /* Read all old log files */
        QStringList nameFilter;
        nameFilter << "*.log";
        QFileInfo   fileInfo(loggingPath);
        QDir        configDir(fileInfo.absoluteDir());
        QStringList infoConfigList = configDir.entryList(nameFilter, QDir::Files | QDir::Readable);
        foreach (QString file, infoConfigList) {
            QDate fileDate = QDate::fromString(file.replace(".log", ""), LOG_FILE_DATE_FORMAT);
            if (fileDate.isValid())
                this->m_lastLogFiles.append(configDir.path() + "/" + file + ".log");
        }
    }

    connect(this, &Logging::signalNewLogEntries, this, &Logging::slotNewLogEntries);

    return 0;
}

void Logging::addNewEntry(QtMsgType type, /*const QMessageLogContext context, */ const QString& msg)
{
    LogEntry* entry = new LogEntry();
    entry->m_type   = type;
    //    entry->m_context = context;
    entry->m_message = msg;
    {
        QMutexLocker lock(&this->m_entryListMutex);
        this->m_logEntries.append(entry);
    }

    emit this->signalNewLogEntries();
}

int Logging::showLoggingInfo(quint16 numbOfLines)
{
    QMutexLocker lock(&this->m_internalMutex);

    QList<QString> aList = this->m_currentLogging.split("\n");
    if (aList.last() == "")
        aList.removeLast();

    int cnt = 0;
    if (aList.size() > numbOfLines)
        cnt = aList.size() - numbOfLines;

    for (; cnt < aList.size(); cnt++) {
        std::cout << aList[cnt].toStdString() << std::endl;
    }

    return numbOfLines;
}

QStringList Logging::getLogFileDates()
{
    QMutexLocker lock(&this->m_internalMutex);

    QStringList rValue;
    foreach (QString file, this->m_lastLogFiles) {
        QFileInfo info(file);
        rValue.append(info.fileName().replace(".log", ""));
    }
    return rValue;
}


QString Logging::createLoggingFilePath()
{
    QString loggingPath = "";
#ifdef Q_OS_ANDROID
    loggingPath = getUserAppDataLocation() + "/Log/";
#else
#ifdef Q_OS_WIN
#ifndef QT_DEBUG
    loggingPath = getUserHomeConfigPath() + "/Log/";
#endif //#ifdef QT_DEBUG
#else  // Q_OS_WIN
    loggingPath = getUserHomeConfigPath() + "/Log/";
#endif // Q_OS_WIN
#endif // Q_OS_ANDROID

    if (loggingPath.size() > 0) {
        loggingPath.append(QDate::currentDate().toString(LOG_FILE_DATE_FORMAT));
        loggingPath.append(".log");
    }

    return loggingPath;
}

void Logging::slotNewLogEntries()
{
    while (true) {

        this->m_entryListMutex.lock();
        if (this->m_logEntries.isEmpty()) {
            this->m_entryListMutex.unlock();
            break;
        }

        LogEntry* entry = this->m_logEntries.first();
        this->m_logEntries.removeFirst();
        this->m_entryListMutex.unlock();

        QMessageLogContext context;
        QString            msg = qFormatLogMessage(entry->m_type, context, entry->m_message);


        this->m_internalMutex.lock();
        this->m_currentLogging.append(msg + "\n");
        if (this->m_logFile != NULL && this->m_logFile->isOpen()) {
            QTextStream out(this->m_logFile);
            out << msg << "\n";
            if (entry->m_type == QtCriticalMsg)
                std::cout << msg.toStdString() << std::endl;
            //} else if (entry->m_type != QtDebugMsg && entry->m_type != QtInfoMsg) {
        }
#ifdef QT_DEBUG
#ifdef Q_OS_UNIX
        else
#endif
            std::cout << msg.toStdString() << std::endl;
#endif
        delete entry;
        this->m_internalMutex.unlock();
    }
}

void Logging::slotEveryHourTimeout()
{
    QMutexLocker lock(&this->m_internalMutex);

    /* Check if new path is needed */
    QString loggingPath = this->createLoggingFilePath();
    if (loggingPath.size() > 0) {
        if (this->m_logFile == NULL || this->m_logFile->fileName() != loggingPath) {

            if (this->m_logFile != NULL) {
                this->m_logFile->close();
                delete this->m_logFile;
            }
            if (!checkFilePathExistAndCreate(loggingPath)) {
                CONSOLE_CRITICAL(QString("Could not create file for Logging"));
                return;
            }
            this->m_logFile = new QFile(loggingPath);
            if (!this->m_logFile->open(QFile::ReadWrite | QFile::Text | QFile::Append)) {
                CONSOLE_CRITICAL(QString("Could not open file for Logging"));
                return;
            }
#ifdef Q_OS_UNIX
            QFileInfo info(loggingPath);
            QFile link(info.absolutePath() + "/CurrentLog.log");
            if (link.exists())
                link.remove();
            if (this->m_logFile->link(info.absolutePath() + "/CurrentLog.log"))
                qInfo().noquote() << QString("Create symbolic link for new actual log file %1").arg(loggingPath);
            else
                qWarning().noquote() << QString("Could not create symbolic link for current log file %1").arg(loggingPath);
#endif
        }
    }

    /* Remove all old log files */
    for (int i = this->m_lastLogFiles.size() - 1; i >= 0; i--) {
        QFileInfo file(this->m_lastLogFiles[i]);
        QDate     fileDate = QDate::fromString(file.fileName().replace(".log", ""), LOG_FILE_DATE_FORMAT);
        if (fileDate.isValid() && fileDate.addDays(MAX_DAYS_LOG_FILES) < QDate::currentDate()) {
            if (!QFile::remove(file.absoluteFilePath()))
                qWarning().noquote() << QString("Could not remove file: %1.log").arg(file.absoluteFilePath());
            else
                this->m_lastLogFiles.removeAt(i);
        }
    }

    /* Restart Timer to check again in next hour */
    this->m_hourTimer->start((60 - QTime::currentTime().minute()) * 60 * 1000);
}

void Logging::terminate()
{
    disconnect(this, &Logging::signalNewLogEntries, this, &Logging::slotNewLogEntries);
    this->slotNewLogEntries();
    if (this->m_logFile != NULL) {
        this->m_logFile->close();
        delete this->m_logFile;
        this->m_logFile = NULL;
    }

    if (this->m_hourTimer == NULL)
        delete this->m_hourTimer;
    this->m_hourTimer = NULL;
}
