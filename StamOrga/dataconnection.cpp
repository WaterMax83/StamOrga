#include "dataconnection.h"
#include "../Common/Network/messageprotocol.h"
#include "../Common/Network/messagecommand.h"

DataConnection::DataConnection(GlobalData *pData) : BackgroundWorker()
{
    this->SetWorkerName("MainConnection");
    this->m_pGlobalData = pData;
}


int DataConnection::DoBackgroundWork()
{
    /* Have to do it here or it will not be in the correct thread */
    this->m_pConTimeout = new QTimer();
    this->m_pConTimeout->setSingleShot(true);
    connect(this->m_pConTimeout, &QTimer::timeout, this, &DataConnection::connectionTimeoutFired);

    QByteArray aPassw;
    aPassw.append(this->m_pGlobalData->userName());

    MessageProtocol msg(OP_CODE_CMD_REQ::REQ_LOGIN_USER, aPassw);

    this->m_pDataUdpSocket = new QUdpSocket();

    if (!this->m_pDataUdpSocket->bind())
    {
//        emit this->connectionRequestFinished(0, this->m_pDataUdpSocket->errorString());
        return -1;
    }

    connect(this->m_pDataUdpSocket, &QUdpSocket::readyRead, this, &DataConnection::readyReadDataPort);

    this->m_hDataReceiver = QHostAddress(this->m_pGlobalData->ipAddr());
    const char *pData = msg.getNetworkProtocol();
    this->m_pDataUdpSocket->writeDatagram(pData, msg.getNetworkSize(), this->m_hDataReceiver, this->m_pGlobalData->conDataPort());

    this->m_pConTimeout->start(3000);

    return 0;
}


void DataConnection::readyReadDataPort()
{
    while(this->m_pDataUdpSocket->hasPendingDatagrams()) {
        QHostAddress sender;
        quint16 port;
        QByteArray datagram;
        datagram.resize(this->m_pDataUdpSocket->pendingDatagramSize());

        if (this->m_pDataUdpSocket->readDatagram(datagram.data(), datagram.size(), &sender, &port)) {
            if (this->m_pGlobalData->conDataPort() == port
                && this->m_hDataReceiver.toIPv4Address() == sender.toIPv4Address()) {

                this->m_messageBuffer.StoreNewData(datagram);
            }
        }
    }

    this->checkNewOncomingData();
}

void DataConnection::checkNewOncomingData()
{
    MessageProtocol *msg;
    while((msg = this->m_messageBuffer.GetNextMessage()) != NULL) {

        if (msg->getIndex() == OP_CODE_CMD_RES::ACK_LOGIN_USER) {
            this->m_pConTimeout->stop();
            qint32 rValue = msg->getIntData();
            if (msg->getDataLength() == 4 && rValue > 0) {
                emit this->notifyLoginRequest(rValue);
            }
            else
                emit this->notifyLoginRequest(0);
        }
        delete msg;
    }
}

void DataConnection::connectionTimeoutFired()
{
//    emit this->connectionRequestFinished(0, "Timeout");
    qDebug() << "Timeout from Data UdpServer";
}


DataConnection::~DataConnection()
{
    if (this->m_pDataUdpSocket != NULL)
        delete this->m_pDataUdpSocket;
}
