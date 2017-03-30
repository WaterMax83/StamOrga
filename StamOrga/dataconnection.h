#ifndef DATACONNECTION_H
#define DATACONNECTION_H

#include <QtCore/QTimer>
#include <QtCore/QList>
#include <QtNetwork/QUdpSocket>

#include "globaldata.h"
#include "datahandling.h"
#include "../Common/General/backgroundworker.h"
#include "../Common/Network/messagebuffer.h"

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

public slots:
    void startSendLoginRequest();
    void startSendVersionRequest();

private slots:
    void connectionTimeoutFired();
    void readyReadDataPort();

private:
    GlobalData      *m_pGlobalData;
    MessageBuffer   m_messageBuffer;
    DataHandling    m_dataHandle;

    QTimer          *m_pConTimeout;
    QUdpSocket      *m_pDataUdpSocket = NULL;
    QHostAddress    m_hDataReceiver;


    void checkNewOncomingData();
    qint32 sendMessageRequest(MessageProtocol *msg);
    void removeActualRequest(quint32 req);

    QList<quint32>         m_lActualRequest;
};

#endif // DATACONNECTION_H
