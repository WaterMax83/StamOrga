#ifndef UDPSERVER_H
#define UDPSERVER_H

#include <QtCore/QObject>
#include <QtCore/QList>
#include <QtNetwork/QUdpSocket>

#include "udpdataserver.h"
#include "connectiondata.h"
#include "General/globaldata.h"
#include <../Common/General/backgroundworker.h>
#include <../Common/General/backgroundcontroller.h>
#include <../Common/Network/messagebuffer.h>


struct UserConnection {
    bool                    isConnected;
    MessageBuffer           msgBuffer;
    UserConData             userConData;
    UdpDataServer           *pDataServer;
    BackgroundController    *pctrlUdpDataServer;
};


class UdpServer : public BackgroundWorker
{
    Q_OBJECT
public:
    UdpServer(GlobalData *pData);
    ~UdpServer();

protected:
    int DoBackgroundWork() override;

    QString m_workerName = "UDPServer";

private slots:
    void readyReadMasterPort();
    void readChannelFinished();

    void onConnectionTimedOut(quint16 port);

private:
    QUdpSocket                  *m_pUdpMasterSocket = NULL;

    GlobalData                  *m_pGlobalData;

    QList<UserConnection>       m_lUserCons;

    UserConnection  *getUserConnection(QHostAddress addr, quint16 port);

    quint16 getFreeDataPort();

    void checkNewOncomingData();
};

#endif // UDPSERVER_H
