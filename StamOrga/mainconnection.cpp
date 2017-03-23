#include <QByteArray>
#include <QUdpSocket>

#include "mainconnection.h"
#include "../Common/Network/messageprotocol.h"
#include "../Common/Network/messagecommand.h"

MainConnection::MainConnection(ConnectionInfo *info) : BackgroundWorker()
{
    this->SetWorkerName("MainConnection");
    this->m_conInfo = info;
}


int MainConnection::DoBackgroundWork()
{
    QByteArray aData;
    aData.append(this->m_conInfo->GetUserName());

    MessageProtocol msg(OP_CODE_CMD_REQ::REQ_CONNECT_USER, aData);

    QUdpSocket socket(this);

    if (!socket.bind())
    {
        emit this->ConnectionRequestFinished(false, socket.errorString());
        return -1;
    }

    QHostAddress sender(this->m_conInfo->GetHostAddress());
    const char *pData = msg.getNetworkProtocol();
    socket.writeDatagram(pData, msg.getNetworkSize(), sender, this->m_conInfo->GetPort());

    this->m_pConTimeout = new QTimer();
    this->m_pConTimeout->setSingleShot(true);
    connect(this->m_pConTimeout, &QTimer::timeout, this, &MainConnection::ConnectionTimeoutFired);
    this->m_pConTimeout->start(3000);

    return 0;
}

void MainConnection::ConnectionTimeoutFired()
{
    emit this->ConnectionRequestFinished(false, "Timeout");
}
