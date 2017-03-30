#ifndef CONNECTIONHANDLING_H
#define CONNECTIONHANDLING_H

#include <QObject>

#include "mainconnection.h"
#include "dataconnection.h"
#include "globaldata.h"
#include "../Common/General/backgroundcontroller.h"


class ConnectionHandling : public QObject
{
    Q_OBJECT
public:
    explicit ConnectionHandling(QObject *parent = 0);

    bool startMainConnection();
    bool startGettingInfo();

    void setGlobalData(GlobalData *pData)
    {
        if (pData != NULL)
            this->m_pGlobalData = pData;
    }

//    ConnectionInfo *GetConnectionInfo() { return &this->m_conInfo; }

signals:
    void sNotifyConnectionFinished(qint32 result);
    void sNotifyVersionRequest(qint32 result, QString msg);

    void sStartSendLoginRequest();
    void sStartSendVersionRequest();

public slots:

private slots:
    void slMainConReqFin(qint32 result, const QString &msg);
    void slDataConLoginFinished(qint32 result);
    void slDataConVersionFinished(qint32 result, QString msg);

private:
    BackgroundController m_ctrlMainCon;
    MainConnection *m_pMainCon = NULL;

    BackgroundController m_ctrlDataCon;
    DataConnection *m_pDataCon = NULL;

    GlobalData *m_pGlobalData = NULL;

    void sendLoginRequest();
    void sendVersionRequest();

    void startDataConnection();
    void stopDataConnection();
    bool isDataConnectionActive() { return this->m_ctrlDataCon.IsRunning(); }
};

#endif // CONNECTIONHANDLING_H
