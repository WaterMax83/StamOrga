#include <QtCore/QByteArray>
#include <QtNetwork/QUdpSocket>
#include <QtCore/QtEndian>

#include "mainconnection.h"
#include "../Common/Network/messageprotocol.h"
#include "../Common/Network/messagecommand.h"

MainConnection::MainConnection(GlobalData *pData) : BackgroundWorker()
{
    this->SetWorkerName("MainConnection");
    this->m_pGlobalData = pData;
}


int MainConnection::DoBackgroundWork()
{
    QByteArray aData;
    aData.append(this->m_pGlobalData->userName());

    MessageProtocol msg(OP_CODE_CMD_REQ::REQ_CONNECT_USER, aData);

    this->m_pMasterUdpSocket = new QUdpSocket();

    if (!this->m_pMasterUdpSocket->bind())
    {
        emit this->connectionRequestFinished(0, this->m_pMasterUdpSocket->errorString());
        return -1;
    }

    connect(this->m_pMasterUdpSocket, &QUdpSocket::readyRead, this, &MainConnection::readyReadMasterPort);

    this->m_hMasterReceiver = QHostAddress(this->m_pGlobalData->ipAddr());
    const char *pData = msg.getNetworkProtocol();
    this->m_pMasterUdpSocket->writeDatagram(pData, msg.getNetworkSize(), this->m_hMasterReceiver, this->m_pGlobalData->conPort());

    this->m_pConTimeout = new QTimer();
    this->m_pConTimeout->setSingleShot(true);
    connect(this->m_pConTimeout, &QTimer::timeout, this, &MainConnection::connectionTimeoutFired);
    this->m_pConTimeout->start(3000);

    return 0;
}

void MainConnection::connectionTimeoutFired()
{
    emit this->connectionRequestFinished(0, "Timeout");
}

void MainConnection::readyReadMasterPort()
{
    while(this->m_pMasterUdpSocket->hasPendingDatagrams()) {
        QHostAddress sender;
        quint16 port;
        QByteArray datagram;
        datagram.resize(this->m_pMasterUdpSocket->pendingDatagramSize());

        if (this->m_pMasterUdpSocket->readDatagram(datagram.data(), datagram.size(), &sender, &port)) {
            if (this->m_pGlobalData->conPort() == port
                && this->m_hMasterReceiver.toIPv4Address() == sender.toIPv4Address()) {

                this->m_messageBuffer.StoreNewData(datagram);
            }
        }
    }

    this->checkNewOncomingData();
}

void MainConnection::checkNewOncomingData()
{
    MessageProtocol *msg;
    while((msg = this->m_messageBuffer.GetNextMessage()) != NULL) {

        if (msg->getIndex() == OP_CODE_CMD_RES::ACK_CONNECT_USER) {
            this->m_pConTimeout->stop();
            quint32 rValue = msg->getIntData();
            qDebug() << QString("return value = %1").arg(rValue);
            if (msg->getDataLength() == 4 && rValue > 0) {
                emit this->connectionRequestFinished(rValue, "");
            }
            else
                emit this->connectionRequestFinished(0, QString("Wrong user %1").arg(rValue));
        }
        delete msg;
    }
}


MainConnection::~MainConnection()
{
    if (this->m_pMasterUdpSocket != NULL)
        delete this->m_pMasterUdpSocket;
}
