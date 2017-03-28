#ifndef MAINCONNECTION_H
#define MAINCONNECTION_H

#include <QtCore/QObject>
#include <QtCore/QTimer>
#include <QtNetwork/QUdpSocket>

#include "globaldata.h"
#include "../Common/General/backgroundworker.h"
#include "../Common/Network/messagebuffer.h"

class MainConnection : public BackgroundWorker
{
    Q_OBJECT
public:
    MainConnection(GlobalData *pData);
    ~MainConnection();

    int DoBackgroundWork() override;

    QString m_workerName = "MainConnection";

signals:
    void connectionRequestFinished(qint32 result, const QString &msg);

private slots:
    void connectionTimeoutFired();
    void readyReadMasterPort();

private:
    GlobalData      *m_pGlobalData;
    MessageBuffer   m_messageBuffer;

    QTimer          *m_pConTimeout;
    QUdpSocket      *m_pMasterUdpSocket = NULL;
    QHostAddress    m_hMasterReceiver;

    void checkNewOncomingData();

};

#endif // MAINCONNECTION_H
