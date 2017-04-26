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

//**************************************************************************************************
/// \file       backgroundworker.h
/// \brief      Interface description of BackgroundWorker class
/// \remark     Contains the abstract class for background tasks
/// \author     Markus Schneider (msc)
/// \project    XTE SDS SM1281 simulator
/// \version    V 01.00.EN, 2017-02-23, msc, Initial version
/// \date       2017-02-23
//**************************************************************************************************


#ifndef BACKGROUNDWORKER_H
#define BACKGROUNDWORKER_H

#include <QObject>

class BackgroundWorker : public QObject
{
    Q_OBJECT

public:
    explicit BackgroundWorker(QObject *parent = 0);
    explicit BackgroundWorker(QString &name);
    ~BackgroundWorker();

    void SetWorkerName(const QString &value) { this->m_workerName = value; }
    QString& GetWorkerName() { return this->m_workerName; }

signals:
    void notifyThreadInfo(const QString &info);
    void notifyThreadError(const QString &error, const bool &stop);
    void notifyBackgroundWorkerFinished(const int &result);

public slots:
    void startBackgroundWork();

protected:
    virtual int DoBackgroundWork() = 0;

    void Info(const QString &info);
    void Error(const QString &error, const bool &stop = false);


    bool IsStopRequested();

private:
    QString m_workerName;

};

#endif // BACKGROUNDWORKER_H
