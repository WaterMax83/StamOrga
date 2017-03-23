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
    explicit BackgroundController(QObject *parent = 0);
    ~BackgroundController();

    bool IsRunning() { return m_thread.isRunning(); }

//    void SetCleanupAfterWorkerFinished(bool value) { this->m_bCleanupAfterWorkerFinished = value; }
    bool GetCleanupAfterWorkerFinished() { return this->m_bCleanupAfterWorkerFinished; }

    bool GetBackGroundWorkerFinished() { return this->m_bBackgroundWorkerFinished; }

    void Start(BackgroundWorker *worker, bool CleanupAfterWorkerFinished = true);

    void Stop();

signals:
    void notifyBackgroundWorkerFinished(const int &result);
    void notifyThreadFinished();

public slots:

private slots:
    void finishedThread();
    void finishedBackgroundWorker(const int &result);

private:
    QThread m_thread;

    BackgroundWorker *m_worker = NULL;

    bool m_bCleanupAfterWorkerFinished;

    bool m_bBackgroundWorkerFinished;

    void CleanupBackgroundWorker();
};

#endif // BACKGROUNDCONTROLLER_H
