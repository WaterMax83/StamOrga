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
/// \file       backgroundcontroller.cpp
/// \brief      Contains the function to start and stop a background task
/// \remark
/// \author     Markus Schneider (msc)
/// \project    XTE SDS SM1281 simulator
/// \version    V 01.00.EN, 2017-02-23, msc, Initial version
/// \date       2017-02-23
//**************************************************************************************************


#include <QDebug>

#include "backgroundcontroller.h"
#include "backgroundworker.h"

BackgroundController::BackgroundController(QObject* parent)
    : QObject(parent)
{
    this->m_bBackgroundWorkerFinished = false;
}


void BackgroundController::Start(BackgroundWorker* worker, bool CleanupAfterWorkerFinished)
{
    if (this->IsRunning())
        return;

    this->m_bCleanupAfterWorkerFinished = CleanupAfterWorkerFinished;
    this->m_bBackgroundWorkerFinished   = false;

    this->m_worker = worker;
    worker->moveToThread(&this->m_thread);
    connect(&this->m_thread, &QThread::started, worker, &BackgroundWorker::startBackgroundWork);
    connect(worker, &BackgroundWorker::notifyBackgroundWorkerFinished, this, &BackgroundController::finishedBackgroundWorker);
    connect(&this->m_thread, &QThread::finished, this, &BackgroundController::finishedThread);
    connect(&this->m_thread, &QThread::finished, worker, &BackgroundWorker::deleteLater);

    this->m_thread.setObjectName(worker->GetWorkerName());
    this->m_thread.start();
}

void BackgroundController::Stop(bool wait)
{
    if (!this->IsRunning())
        return;

    this->m_bCleanupAfterWorkerFinished = true; /* Always cleanup when stopped */
    this->m_thread.requestInterruption();

    if (this->m_bBackgroundWorkerFinished) {
        this->m_thread.quit();
        if (wait)
            this->m_thread.wait();
    }
}

void BackgroundController::finishedThread()
{
    this->CleanupBackgroundWorker();

    emit notifyThreadFinished();
}

void BackgroundController::finishedBackgroundWorker(const int& result)
{
    Q_UNUSED(result);
    //    disconnect(this->m_worker, &BackgroundWorker::notifyBackgroundWorkerFinished, this, &BackgroundController::finishedBackgroundWorker);

    if (this->m_bCleanupAfterWorkerFinished)
        this->CleanupBackgroundWorker();

    this->m_bBackgroundWorkerFinished = true;

    emit notifyBackgroundWorkerFinished(result);
}

void BackgroundController::CleanupBackgroundWorker()
{
    if (this->m_worker == nullptr)
        return;

    if (this->IsRunning()) {
        this->m_thread.quit();
        if (!this->m_thread.wait(2000)) {
            this->m_thread.terminate();
            this->m_thread.wait();
        }
    }

    disconnect(&this->m_thread, &QThread::started, this->m_worker, &BackgroundWorker::startBackgroundWork);
    disconnect(&this->m_thread, &QThread::finished, this, &BackgroundController::finishedThread);

    this->m_worker = nullptr;
}


BackgroundController::~BackgroundController()
{
    this->CleanupBackgroundWorker();
}
