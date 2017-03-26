#include "udpdataserver.h"
#include "../Common/Network/messageprotocol.h"

UdpDataServer::UdpDataServer(UserConData *pUsrConData, GlobalData *pGlobalData) : BackgroundWorker()
{
    this->SetWorkerName(QString("UDP Data Server %1").arg(pUsrConData->dataPort));

    this->m_pUsrConData = pUsrConData;
    this->m_pGlobalData = pGlobalData;
}


int UdpDataServer::DoBackgroundWork()
{
    this->m_pUdpSocket = new QUdpSocket();
    if (!this->m_pUdpSocket->bind(QHostAddress::Any, this->m_pUsrConData->dataPort))
    {
        qDebug() << QString("Error binding socket  for port %1: %2\n").arg(this->m_pUsrConData->dataPort).arg(this->m_pUdpSocket->errorString());
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
            if (port == this->m_pUsrConData->dataPort
                && sender.toIPv4Address() == this->m_pUsrConData->sender.toIPv4Address())
                this->m_msgBuffer.StoreNewData(datagram);
        }
    }

    this->checkNewOncomingData();
}

void UdpDataServer::onConnectionResetTimeout()
{
    emit this->notifyConnectionTimedOut(this->m_pUsrConData->dataPort);
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

