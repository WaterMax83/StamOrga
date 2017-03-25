#ifndef UDPDATASERVER_H
#define UDPDATASERVER_H

#include <QtCore/QString>
#include <QtCore/QTimer>
#include <QtNetwork/QUdpSocket>

#include "../General/globaldata.h"
#include "../Common/General/backgroundworker.h"
#include "../Common/Network/messagebuffer.h"

class UdpDataServer : public BackgroundWorker
{
    Q_OBJECT
public:
    UdpDataServer(quint32 port, QHostAddress addr, GlobalData *pData);
    ~UdpDataServer();

protected:
    int DoBackgroundWork() override;

    QString m_workerName = "UDPServer";

signals:
    void notifyConnectionTimedOut(quint16 port);

private slots:
    void readyReadSocketPort();
    void onConnectionResetTimeout();

private:
    GlobalData      *m_pGlobalData;
    quint32         m_dataPort;
    QHostAddress    m_senderAddr;

    QUdpSocket      *m_pUdpSocket = NULL;
    MessageBuffer   m_msgBuffer;

    QTimer          *m_pConResetTimer = NULL;

    void checkNewOncomingData();
};

#endif // UDPDATASERVER_H
