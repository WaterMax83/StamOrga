#ifndef UDPDATASERVER_H
#define UDPDATASERVER_H

#include <QtCore/QString>
#include <QtCore/QTimer>
#include <QtNetwork/QUdpSocket>

#include "connectiondata.h"
#include "../General/globaldata.h"
#include "../General/dataconnection.h"
#include "../Common/General/backgroundworker.h"
#include "../Common/Network/messagebuffer.h"

class UdpDataServer : public BackgroundWorker
{
    Q_OBJECT
public:
    UdpDataServer(UserConData *pUsrConData, GlobalData *pGlobalData);
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
    UserConData     *m_pUsrConData;
    DataConnection  *m_pDataConnection = NULL;

    QUdpSocket      *m_pUdpSocket = NULL;
    MessageBuffer   m_msgBuffer;

    QTimer          *m_pConResetTimer = NULL;

    void checkNewOncomingData();

    MessageProtocol *checkNewMessage(MessageProtocol *msg);
};

#endif // UDPDATASERVER_H
