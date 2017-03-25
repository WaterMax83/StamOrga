#include "udpdataserver.h"

UdpDataServer::UdpDataServer(quint32 port, QHostAddress addr, GlobalData *pData) : BackgroundWorker()
{
    this->SetWorkerName(QString("UDP Data Server %1").arg(port));

    this->m_dataPort = port;
    this->m_senderAddr = addr;
    this->m_pGlobalData = pData;
}


int UdpDataServer::DoBackgroundWork()
{
    this->m_pConResetTimer = new QTimer();
    this->m_pConResetTimer->setSingleShot(true);
    this->m_pConResetTimer->setInterval(2000);
    connect(this->m_pConResetTimer, &QTimer::timeout, this, &UdpDataServer::onConnectionResetTimeout);
    this->m_pConResetTimer->start();

    return 0;
}

void UdpDataServer::onConnectionResetTimeout()
{
    emit this->notifyConnectionTimedOut(this->m_dataPort);
}


UdpDataServer::~UdpDataServer()
{
    if (this->m_pConResetTimer != NULL)
        delete this->m_pConResetTimer;
}

