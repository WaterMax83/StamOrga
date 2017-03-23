#ifndef MAINCONNECTION_H
#define MAINCONNECTION_H

#include <QObject>
#include <QTimer>

#include "connectioninfo.h"
#include "../Common/General/backgroundworker.h"

class MainConnection : public BackgroundWorker
{
    Q_OBJECT
public:
    MainConnection(ConnectionInfo *info);

    int DoBackgroundWork() override;

    QString m_workerName = "MainConnection";

signals:
    void ConnectionRequestFinished(bool result, const QString &msg);
    void ConnectionEstablished();

private slots:
    void ConnectionTimeoutFired();

private:
    ConnectionInfo *m_conInfo;

    QTimer *m_pConTimeout;

};

#endif // MAINCONNECTION_H
