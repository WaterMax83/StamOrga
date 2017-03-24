#include "udpserver.h"
#include "../../Common/Network/messagecommand.h"

UdpServer::UdpServer(GlobalData *pData) : BackgroundWorker()
{
    this->SetWorkerName("TCPServer");

    this->m_pGlobalData = pData;
}


int UdpServer::DoBackgroundWork()
{
    this->m_pUdpMasterSocket = new QUdpSocket();
    if (!this->m_pUdpMasterSocket->bind(QHostAddress::Any, 55000))
    {
        qDebug() << QString("Error binding master socket %1\n").arg(this->m_pUdpMasterSocket->errorString());
        return -1;
    }
    connect(this->m_pUdpMasterSocket, &QUdpSocket::readyRead, this, &UdpServer::readyReadMasterPort);
    connect(this->m_pUdpMasterSocket, &QUdpSocket::readChannelFinished, this, &UdpServer::readChannelFinished);

    return 0;
}

void UdpServer::readyReadMasterPort()
{
    while(this->m_pUdpMasterSocket->hasPendingDatagrams()) {
        QHostAddress sender;
        quint16 port;
        QByteArray datagram;
        datagram.resize(this->m_pUdpMasterSocket->pendingDatagramSize());    

        if (this->m_pUdpMasterSocket->readDatagram(datagram.data(), datagram.size(), &sender, &port)) {
            /* Check if user Connection already exists */
            UserConnection *usrCon = this->getUserConnection(sender, port);
            if (usrCon == NULL) {
                UserConnection con;
                con.sender = sender;
                con.port = port;
                this->m_lUserCons.append(con);
                usrCon = &this->m_lUserCons[this->m_lUserCons.size() - 1];
            }
            usrCon->msgBuffer.StoreNewData(datagram);
        }
    }

    this->checkNewOncomingData();
}

void UdpServer::checkNewOncomingData()
{
    MessageProtocol *msg;

    for (int i=0; i<this->m_lUserCons.size(); i++) {

        while((msg = this->m_lUserCons[i].msgBuffer.GetNextMessage()) != NULL) {

            if (msg->getIndex() == OP_CODE_CMD_REQ::REQ_CONNECT_USER) {

                QString userName(QByteArray(msg->getPointerToData(), msg->getDataLength()));
                MessageProtocol *ack;
                if (this->m_pGlobalData->m_UserList.userExists(userName))
                    ack = new MessageProtocol(OP_CODE_CMD_RES::ACK_CONNECT_USER, 0x00123456);
                else
                    ack = new MessageProtocol(OP_CODE_CMD_RES::ACK_CONNECT_USER, 0x0);

                const char *pData = ack->getNetworkProtocol();
                this->m_pUdpMasterSocket->writeDatagram(pData, ack->getNetworkSize(), this->m_lUserCons[i].sender, this->m_lUserCons[i].port);

                delete ack;
            }

            delete msg;
        }
    }
}

UserConnection *UdpServer::getUserConnection(QHostAddress addr, quint16 port)
{
    for (int i=0; i<this->m_lUserCons.size(); i++) {
        if (this->m_lUserCons[i].sender == addr && this->m_lUserCons[i].port == port)
            return &this->m_lUserCons[i];
    }
    return NULL;
}

void UdpServer::readChannelFinished()
{
    qDebug() << "Signal readChannelFinished was called";
}


UdpServer::~UdpServer()
{
    if (this->m_pUdpMasterSocket != NULL)
        delete this->m_pUdpMasterSocket;
}
