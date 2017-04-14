#ifndef CONNECTIONHANDLING_H
#define CONNECTIONHANDLING_H

#include <QtCore/QObject>
#include <QtCore/QTimer>

#include "mainconnection.h"
#include "dataconnection.h"
#include "../Data/globaldata.h"
#include "../Common/General/backgroundcontroller.h"


class ConnectionHandling : public QObject
{
    Q_OBJECT
public:
    explicit ConnectionHandling(QObject *parent = 0);
    ~ConnectionHandling();

    qint32 startMainConnection(QString name, QString passw);
    qint32 startGettingVersionInfo();
    qint32 startGettingUserProps();
    bool startUpdatePassword(QString newPassWord);
    qint32 startUpdateReadableName(QString name);
    qint32 startGettingGamesList();
    qint32 startSeasonTicketRemove(QString name);
    qint32 startSeasonTicketAdd(QString name, quint32 discount);
    qint32 startGettingSeasonTicketList();




    void setGlobalData(GlobalData *pData)
    {
        if (pData != NULL)
            this->m_pGlobalData = pData;
    }

    GlobalData* getGlobalData()
    {
        return this->m_pGlobalData;
    }

//    ConnectionInfo *GetConnectionInfo() { return &this->m_conInfo; }

signals:
    void sNotifyConnectionFinished(qint32 result);
    void sNotifyVersionRequest(qint32 result, QString msg);
    void sNotifyUserPropertiesRequest(qint32 result, quint32 value);
    void sNotifyUpdatePasswordRequest(qint32 result, QString newPassWord);
    void sNotifyUpdateReadableNameRequest(qint32 result);
    void sNotifyGamesListRequest(qint32 result);
    void sNotifySeasonTicketRemoveRequest(quint32 result);
    void sNotifySeasonTicketAddRequest(quint32 result);
    void sNotifySeasonTicketListRequest(quint32 result);

    void sStartSendLoginRequest();

    void sStartSendNewRequest(DataConRequest request);

public slots:

private slots:
    void slMainConReqFin(qint32 result, const QString &msg);

    void slDataConLastRequestFinished(DataConRequest request);

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

    QList<DataConRequest> m_lErrorMainCon;

    void sendLoginRequest();
    void sendNewRequest(DataConRequest request);

    void checkTimeoutResult(qint32 result);

    void startDataConnection();
    void stopDataConnection();
    bool isDataConnectionActive() { return this->m_ctrlDataCon.IsRunning(); }
    bool isMainConnectionActive() { return this->m_ctrlMainCon.IsRunning(); }
};

#endif // CONNECTIONHANDLING_H
