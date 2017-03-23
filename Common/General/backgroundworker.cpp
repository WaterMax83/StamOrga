//**************************************************************************************************
/// \file       backgroundworker.cpp
/// \brief      Contains the abstract class for background tasks
/// \remark
/// \author     Markus Schneider (msc)
/// \project    XTE SDS SM1281 simulator
/// \version    V 01.00.EN, 2017-02-23, msc, Initial version
/// \date       2017-02-23
//**************************************************************************************************

#include <QtCore/QThread>

#include "backgroundworker.h"


BackgroundWorker::BackgroundWorker(QObject *parent) : QObject(parent)
{
    this->SetWorkerName("BackgroundWorker");
}

BackgroundWorker::BackgroundWorker(QString &name)
{
    this->SetWorkerName(name);
}

BackgroundWorker::~BackgroundWorker()
{

}

void BackgroundWorker::Info(const QString &info)
{
    emit notifyThreadInfo(info);
}

void BackgroundWorker::Error(const QString &error,  const bool &stop)
{
    emit notifyThreadError(error, stop);
}

void BackgroundWorker::startBackgroundWork()
{
    int result = this->DoBackgroundWork();

    emit notifyBackgroundWorkerFinished(result);
}

bool BackgroundWorker::IsStopRequested()
{
    return QThread::currentThread()->isInterruptionRequested();
}

