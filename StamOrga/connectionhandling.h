#ifndef CONNECTIONHANDLING_H
#define CONNECTIONHANDLING_H

#include <QtCore/QObject>
#include <QtCore/QTimer>

#include "mainconnection.h"
#include "dataconnection.h"
#include "globaldata.h"
#include "../Common/General/backgroundcontroller.h"


class ConnectionHandling : public QObject
{
    Q_OBJECT
public:
    explicit ConnectionHandling(QObject *parent = 0);
    ~ConnectionHandling();

    bool startMainConnection(QString name, QString passw);
    bool startGettingInfo();

    bool startUpdatePassword(QString newPassWord);

    void setGlobalData(GlobalData *pData)
    {
        if (pData != NULL)
            this->m_pGlobalData = pData;
    }

//    ConnectionInfo *GetConnectionInfo() { return &this->m_conInfo; }

signals:
    void sNotifyConnectionFinished(qint32 result);
    void sNotifyVersionRequest(qint32 result, QString msg);
    void sNotifyUserPropertiesRequest(qint32 result, quint32 value);
    void sNotifyUpdatePasswordRequest(qint32 result);

    void sStartSendLoginRequest();
    void sStartSendVersionRequest();
    void sStartSendUserPropertiesRequest();
    void sStartSendUpdatePasswordRequest(QString newPassWord);

public slots:

private slots:
    void slMainConReqFin(qint32 result, const QString &msg);
    void slDataConLoginFinished(qint32 result);
    void slDataConVersionFinished(qint32 result, QString msg);
    void slDataConUserPropsFinished(qint32 result, quint32 value);
    void slDataConUpdPassFinished(qint32 result);

    void slTimerConResetFired();
    void slTimerConLoginFired();

private:
    BackgroundController m_ctrlMainCon;
    MainConnection *m_pMainCon = NULL;

    BackgroundController m_ctrlDataCon;
    DataConnection *m_pDataCon = NULL;

    GlobalData *m_pGlobalData = NULL;

    QTimer      *m_pTimerConReset;
    QTimer      *m_pTimerLoginReset;

    void sendLoginRequest();
    void sendVersionRequest();
    void sendUserPropertiesRequest();
    void sendUpdatePasswordRequest(QString newPassWord);

    void startDataConnection();
    void stopDataConnection();
    bool isDataConnectionActive() { return this->m_ctrlDataCon.IsRunning(); }
};

#endif // CONNECTIONHANDLING_H
