#ifndef CONNECTIONHANDLING_H
#define CONNECTIONHANDLING_H

#include <QObject>

#include "mainconnection.h"
#include "connectioninfo.h"
#include "../Common/General/backgroundcontroller.h"


class ConnectionHandling : public QObject
{
    Q_OBJECT
public:
    explicit ConnectionHandling(QObject *parent = 0);

    bool StartMainConnection();

    ConnectionInfo *GetConnectionInfo() { return &this->m_conInfo; }

signals:
    void NotifyError(const QString &error);
    void NotifyInfo(const QString &info);

    void NotifyConnectionFinished();

public slots:

private slots:
    void MainConReqFin(bool result, const QString &msg);

private:
    BackgroundController m_ctrlMainCon;
    MainConnection *m_pMainCon = NULL;

    ConnectionInfo m_conInfo;
};

#endif // CONNECTIONHANDLING_H
