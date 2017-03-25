#include "udpdataserver.h"
#include "../Common/Network/messageprotocol.h"

UdpDataServer::UdpDataServer(quint32 port, QHostAddress addr, GlobalData *pData) : BackgroundWorker()
{
    this->SetWorkerName(QString("UDP Data Server %1").arg(port));

    this->m_dataPort = port;
    this->m_senderAddr = addr;
    this->m_pGlobalData = pData;
}


int UdpDataServer::DoBackgroundWork()
{
    this->m_pUdpSocket = new QUdpSocket();
    if (!this->m_pUdpSocket->bind(QHostAddress::Any, this->m_dataPort))
    {
        qDebug() << QString("Error binding socket  for port %1: %2\n").arg(this->m_dataPort).arg(this->m_pUdpSocket->errorString());
        return -1;
    }
    connect(this->m_pUdpSocket, &QUdpSocket::readyRead, this, &UdpDataServer::readyReadSocketPort);

    this->m_pConResetTimer = new QTimer();
    this->m_pConResetTimer->setSingleShot(true);
    this->m_pConResetTimer->setInterval(10000);
    connect(this->m_pConResetTimer, &QTimer::timeout, this, &UdpDataServer::onConnectionResetTimeout);
    this->m_pConResetTimer->start();

    return 0;
}

void UdpDataServer::readyReadSocketPort()
{
    this->m_pConResetTimer->start();            // something was coming, reset timer

    while(this->m_pUdpSocket->hasPendingDatagrams()) {
        QHostAddress sender;
        quint16 port;
        QByteArray datagram;
        datagram.resize(this->m_pUdpSocket->pendingDatagramSize());

        if (this->m_pUdpSocket->readDatagram(datagram.data(), datagram.size(), &sender, &port)) {
            if (port == this->m_dataPort && sender.toIPv4Address() == this->m_senderAddr.toIPv4Address())
                this->m_msgBuffer.StoreNewData(datagram);
        }
    }

    this->checkNewOncomingData();
}

void UdpDataServer::onConnectionResetTimeout()
{
    emit this->notifyConnectionTimedOut(this->m_dataPort);
}

void UdpDataServer::checkNewOncomingData()
{
    MessageProtocol *msg;
    while((msg = this->m_msgBuffer.GetNextMessage()) != NULL) {

    }
}

UdpDataServer::~UdpDataServer()
{
    if (this->m_pConResetTimer != NULL)
        delete this->m_pConResetTimer;

    if (this->m_pUdpSocket != NULL)
        delete this->m_pUdpSocket;
}

