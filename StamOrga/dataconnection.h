#ifndef DATACONNECTION_H
#define DATACONNECTION_H

#include <QtCore/QTimer>
#include <QtCore/QList>
#include <QtNetwork/QUdpSocket>

#include "../Data/globaldata.h"
#include "datahandling.h"
#include "../Common/General/backgroundworker.h"
#include "../Common/Network/messagebuffer.h"

struct ActualRequest {
    quint32     request;
    QVariant    data;
};

class DataConnection : public BackgroundWorker
{
    Q_OBJECT
public:
    DataConnection(GlobalData *pData);
    ~DataConnection();

    int DoBackgroundWork() override;

    QString m_workerName = "DataConnection";


signals:
    void notifyLoginRequest(qint32 result);
    void notifyVersionRequest(qint32 result, QString msg);
    void notifyUserPropsRequest(qint32 result, quint32 value);
    void notifyUpdPassRequest(qint32 result, QString passw);
    void notifyGamesListRequest(qint32 result);

public slots:
    void startSendLoginRequest();
    void startSendVersionRequest();
    void startSendUserPropsRequest();
    void startSendUpdPassRequest(QString newPassWord);
    void startSendGamesListRequest();

private slots:
    void connectionTimeoutFired();
    void readyReadDataPort();

private:
    GlobalData      *m_pGlobalData;
    MessageBuffer   m_messageBuffer;
    DataHandling    *m_pDataHandle;

    QTimer          *m_pConTimeout;
    QUdpSocket      *m_pDataUdpSocket = NULL;
    QHostAddress    m_hDataReceiver;


    void checkNewOncomingData();
    qint32 sendMessageRequest(MessageProtocol *msg, QVariant *data = NULL);
    void removeActualRequest(quint32 req);
    QVariant getActualRequestData(quint32);

    bool    m_bRequestLoginAgain;
    void sendActualRequestsAgain();

    QList<ActualRequest>         m_lActualRequest;
};

#endif // DATACONNECTION_H
