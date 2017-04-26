/*
*	This file is part of StamOrga
*   Copyright (C) 2017 Markus Schneider
*
*	This program is free software; you can redistribute it and/or modify
*   it under the terms of the GNU General Public License as published by
*   the Free Software Foundation; either version 3 of the License, or
*   (at your option) any later version.
*
*	Foobar is distributed in the hope that it will be useful,
*    but WITHOUT ANY WARRANTY; without even the implied warranty of
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*    GNU General Public License for more details.

*    You should have received a copy of the GNU General Public License
*    along with Foobar.  If not, see <http://www.gnu.org/licenses/>.
*/

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

