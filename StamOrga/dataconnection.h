#ifndef DATACONNECTION_H
#define DATACONNECTION_H

#include <QtCore/QTimer>
#include <QtNetwork/QUdpSocket>

#include "globaldata.h"
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

private slots:
    void connectionTimeoutFired();
    void readyReadDataPort();

private:
    GlobalData      *m_pGlobalData;
    MessageBuffer   m_messageBuffer;

    QTimer          *m_pConTimeout;
    QUdpSocket      *m_pDataUdpSocket = NULL;
    QHostAddress    m_hDataReceiver;

    void checkNewOncomingData();
};

#endif // DATACONNECTION_H
