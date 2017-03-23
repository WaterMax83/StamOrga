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

BackgroundController::BackgroundController(QObject *parent) : QObject(parent)
{
    this->m_bBackgroundWorkerFinished = false;
}


void BackgroundController::Start(BackgroundWorker *worker, bool CleanupAfterWorkerFinished)
{
    if (this->IsRunning())
        return;

    this->m_bCleanupAfterWorkerFinished = CleanupAfterWorkerFinished;
    this->m_bBackgroundWorkerFinished = false;

    this->m_worker = worker;
    worker->moveToThread(&this->m_thread);
    connect(&this->m_thread, &QThread::started, worker, &BackgroundWorker::startBackgroundWork);
    connect(worker, &BackgroundWorker::notifyBackgroundWorkerFinished, this, &BackgroundController::finishedBackgroundWorker);
    connect(&this->m_thread, &QThread::finished, this, &BackgroundController::finishedThread);
    connect(&this->m_thread, &QThread::finished, worker, &BackgroundWorker::deleteLater);

    this->m_thread.setObjectName(worker->GetWorkerName());
    this->m_thread.start();
}

void BackgroundController::Stop()
{
    if (!this->IsRunning())
        return;

    this->m_bCleanupAfterWorkerFinished = true;    /* Always cleanup when stopped */
    this->m_thread.requestInterruption();

    if (this->m_bBackgroundWorkerFinished)
    {
        this->m_thread.quit();
        this->m_thread.wait();
    }
}

void BackgroundController::finishedThread()
{
    this->CleanupBackgroundWorker();

    emit notifyThreadFinished();
}

void BackgroundController::finishedBackgroundWorker(const int &result)
{
	disconnect(this->m_worker, &BackgroundWorker::notifyBackgroundWorkerFinished, this, &BackgroundController::finishedBackgroundWorker);

    if (this->m_bCleanupAfterWorkerFinished)
        this->CleanupBackgroundWorker();

    this->m_bBackgroundWorkerFinished = true;

    emit notifyBackgroundWorkerFinished(result);
}

void BackgroundController::CleanupBackgroundWorker()
{
    if (this->m_worker == NULL)
        return;

    if (this->IsRunning())
    {
        this->m_thread.quit();
        if (!this->m_thread.wait(2000))
        {
            this->m_thread.terminate();
            this->m_thread.wait();
        }
    }

    disconnect(&this->m_thread, &QThread::started, this->m_worker, &BackgroundWorker::startBackgroundWork);
	disconnect(&this->m_thread, &QThread::finished, this, &BackgroundController::finishedThread);

    this->m_worker = NULL;
}



BackgroundController::~BackgroundController()
{
    this->CleanupBackgroundWorker();
}
