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
/// \file       backgroundcontroller.h
/// \brief      Interface description of BackgroundController class
/// \remark     Contains the function to start and stop a background task
/// \author     Markus Schneider (msc)
/// \project    XTE SDS SM1281 simulator
/// \version    V 01.00.EN, 2017-02-23, msc, Initial version
/// \date       2017-02-23
//**************************************************************************************************


#ifndef BACKGROUNDCONTROLLER_H
#define BACKGROUNDCONTROLLER_H

#include <QObject>
#include <QThread>

#include "backgroundworker.h"

class BackgroundController : public QObject
{
    Q_OBJECT
public:
    explicit BackgroundController(QObject* parent = 0);
    virtual ~BackgroundController();

    bool IsRunning() { return m_thread.isRunning(); }

    //    void SetCleanupAfterWorkerFinished(bool value) { this->m_bCleanupAfterWorkerFinished = value; }
    bool GetCleanupAfterWorkerFinished() { return this->m_bCleanupAfterWorkerFinished; }

    bool GetBackGroundWorkerFinished() { return this->m_bBackgroundWorkerFinished; }

    void Start(BackgroundWorker* worker, bool CleanupAfterWorkerFinished = true);

    void Stop(bool wait = true);

signals:
    void notifyBackgroundWorkerFinished(const int& result);
    void notifyThreadFinished();

public slots:

private slots:
    void finishedThread();
    void finishedBackgroundWorker(const int& result);

private:
    QThread m_thread;

    BackgroundWorker* m_worker = NULL;

    bool m_bCleanupAfterWorkerFinished;

    bool m_bBackgroundWorkerFinished;

    void CleanupBackgroundWorker();
};

#endif // BACKGROUNDCONTROLLER_H
