#ifndef UDPSERVER_H
#define UDPSERVER_H

#include <QtCore/QObject>
#include <QtCore/QList>
#include <QtNetwork/QUdpSocket>

#include <../Common/General/backgroundworker.h>
#include <../Common/Network/messagebuffer.h>
#include "General/globaldata.h"


struct UserConnection {
    QHostAddress    sender;
    quint16         port;
    MessageBuffer   msgBuffer;
    bool            isConnected;
};

class UdpServer : public BackgroundWorker
{

public:
    UdpServer(GlobalData *pData);
    ~UdpServer();

protected:
    int DoBackgroundWork() override;

    QString m_workerName = "UDPServer";

private slots:
    void readyReadMasterPort();
    void readChannelFinished();

private:
    QUdpSocket                  *m_pUdpMasterSocket = NULL;

    GlobalData                  *m_pGlobalData;

    QList<UserConnection>       m_lUserCons;

    UserConnection  *getUserConnection(QHostAddress addr, quint16 port);

    void checkNewOncomingData();
};

#endif // UDPSERVER_H
