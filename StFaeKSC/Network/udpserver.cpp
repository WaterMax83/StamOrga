#include "udpserver.h"
#include "../../Common/Network/messagecommand.h"


#define UDP_PORT    55000

UdpServer::UdpServer(GlobalData *pData) : BackgroundWorker()
{
    this->SetWorkerName("UDPMasterServer");

    this->m_pGlobalData = pData;
}


int UdpServer::DoBackgroundWork()
{
    this->m_pUdpMasterSocket = new QUdpSocket();
    if (!this->m_pUdpMasterSocket->bind(QHostAddress::Any, UDP_PORT))
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
                con.srcPort = port;
                con.dataPort = 0;
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

                /* Get userName from packet */
                QString userName(QByteArray(msg->getPointerToData(), msg->getDataLength()));
                MessageProtocol *ack;
                if (this->m_pGlobalData->m_UserList.userExists(userName)) {
                    if (this->m_lUserCons[i].dataPort == 0)                         // when there is not already a port, create a new
                        this->m_lUserCons[i].dataPort = this->getFreeDataPort();

                    ack = new MessageProtocol(OP_CODE_CMD_RES::ACK_CONNECT_USER, (quint32)this->m_lUserCons[i].dataPort);

                    /* Create new thread if it is not running and you got a port */
                    if (this->m_lUserCons[i].dataPort && !this->m_lUserCons[i].pctrlUdpDataServer->IsRunning()) {
                        this->m_lUserCons[i].pDataServer = new UdpDataServer(this->m_lUserCons[i].dataPort,
                                                                             this->m_lUserCons[i].sender,
                                                                             this->m_pGlobalData);
                        connect(this->m_lUserCons[i].pDataServer, &UdpDataServer::notifyConnectionTimedOut, this, &UdpServer::onConnectionTimedOut);
                        this->m_lUserCons[i].pctrlUdpDataServer = new BackgroundController();
                        this->m_lUserCons[i].pctrlUdpDataServer->Start(this->m_lUserCons[i].pDataServer, false);
                    }
                }
                else
                    ack = new MessageProtocol(OP_CODE_CMD_RES::ACK_CONNECT_USER, 0x0);

                /* send answer */
                const char *pData = ack->getNetworkProtocol();
                this->m_pUdpMasterSocket->writeDatagram(pData, ack->getNetworkSize(), this->m_lUserCons[i].sender, this->m_lUserCons[i].srcPort);

                delete ack;
            }

            delete msg;
        }
    }
}

void UdpServer::onConnectionTimedOut(quint16 port)
{
    for (int i=0; i<this->m_lUserCons.size(); i++)
    {
        if (this->m_lUserCons[i].dataPort == port)
        {
            this->m_lUserCons[i].pctrlUdpDataServer->Stop();
            delete this->m_lUserCons[i].pctrlUdpDataServer;
            this->m_lUserCons.removeAt(i);
            return;
        }
    }
}

UserConnection *UdpServer::getUserConnection(QHostAddress addr, quint16 port)
{
    for (int i=0; i<this->m_lUserCons.size(); i++) {
        if (this->m_lUserCons[i].sender == addr && this->m_lUserCons[i].srcPort == port)
            return &this->m_lUserCons[i];
    }
    return NULL;
}

quint16 UdpServer::getFreeDataPort()
{
    quint16 retPort = UDP_PORT + 1;
    do {
        bool bAlreadyUsed = false;
        foreach(UserConnection usrCon, this->m_lUserCons) {
            if (usrCon.dataPort == retPort) {
                bAlreadyUsed = true;
                break;
            }
        }
        if (!bAlreadyUsed)
            return retPort;
        retPort++;
    } while(retPort < UDP_PORT + 100);

    return 0;
}

void UdpServer::readChannelFinished()
{
//    qDebug() << "Signal readChannelFinished was called";
}


UdpServer::~UdpServer()
{
    if (this->m_pUdpMasterSocket != NULL)
        delete this->m_pUdpMasterSocket;
}
