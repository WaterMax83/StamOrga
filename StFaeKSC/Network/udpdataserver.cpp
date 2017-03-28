#include "udpdataserver.h"
#include "../Common/Network/messageprotocol.h"
#include "../Common/Network/messagecommand.h"
#include "../Common/General/globalfunctions.h"

UdpDataServer::UdpDataServer(UserConData *pUsrConData, GlobalData *pGlobalData) : BackgroundWorker()
{
    this->SetWorkerName(QString("UDP Data Server %1").arg(pUsrConData->dstDataPort));

    this->m_pUsrConData = pUsrConData;
    this->m_pGlobalData = pGlobalData;
}


int UdpDataServer::DoBackgroundWork()
{
    this->m_pUdpSocket = new QUdpSocket();
    if (!this->m_pUdpSocket->bind(QHostAddress::Any, this->m_pUsrConData->dstDataPort))
    {
        qDebug() << QString("Error binding socket  for port %1: %2\n").arg(this->m_pUsrConData->dstDataPort).arg(this->m_pUdpSocket->errorString());
        return -1;
    }
    connect(this->m_pUdpSocket, &QUdpSocket::readyRead, this, &UdpDataServer::readyReadSocketPort);

    this->m_pConResetTimer = new QTimer();
    this->m_pConResetTimer->setSingleShot(true);
    this->m_pConResetTimer->setInterval(10000);
    connect(this->m_pConResetTimer, &QTimer::timeout, this, &UdpDataServer::onConnectionResetTimeout);
    this->m_pConResetTimer->start();

    this->m_pDataConnection = new DataConnection(this->m_pGlobalData);

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
            if (sender.toIPv4Address() == this->m_pUsrConData->sender.toIPv4Address()) {
                this->m_msgBuffer.StoreNewData(datagram);
                this->m_pUsrConData->srcDataPort = port;
            }
        }
    }

    this->checkNewOncomingData();
}

void UdpDataServer::onConnectionResetTimeout()
{
    emit this->notifyConnectionTimedOut(this->m_pUsrConData->dstDataPort);
}

void UdpDataServer::checkNewOncomingData()
{
    MessageProtocol *msg;
    while((msg = this->m_msgBuffer.GetNextMessage()) != NULL) {

        MessageProtocol *ack = checkNewMessage(msg);

        if (ack != NULL) {

            const char *pData = ack->getNetworkProtocol();
            this->m_pUdpSocket->writeDatagram(pData, ack->getNetworkSize(),
                                                    this->m_pUsrConData->sender,
                                                    this->m_pUsrConData->srcDataPort);
            delete ack;
        }
        delete msg;
    }
}

MessageProtocol *UdpDataServer::checkNewMessage(MessageProtocol *msg)
{
    if (this->m_bIsLoggedIn) {
        switch(msg->getIndex()) {
        case OP_CODE_CMD_REQ::REQ_LOGIN_USER:
            return this->m_pDataConnection->requestCheckUserLogin(msg, this->m_pUsrConData);
        default:
            qInfo().noquote() << QString("Unkown command %1").arg(msg->getIndex());
            return NULL;
        }
    }
    else if (msg->getIndex() == OP_CODE_CMD_REQ::REQ_LOGIN_USER)
        return this->m_pDataConnection->requestCheckUserLogin(msg, this->m_pUsrConData);
    else
        return new MessageProtocol(OP_CODE_CMD_RES::ACK_NOT_LOGGED_IN);

    return NULL;
}

UdpDataServer::~UdpDataServer()
{
    if (this->m_pDataConnection != NULL)
        delete this->m_pDataConnection;

    if (this->m_pConResetTimer != NULL)
        delete this->m_pConResetTimer;

    if (this->m_pUdpSocket != NULL)
        delete this->m_pUdpSocket;
}

