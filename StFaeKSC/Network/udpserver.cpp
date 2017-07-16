/*
*	This file is part of StamOrga
*   Copyright (C) 2017 Markus Schneider
*
*	This program is free software; you can redistribute it and/or modify
*   it under the terms of the GNU General Public License as published by
*   the Free Software Foundation; either version 3 of the License, or
*   (at your option) any later version.
*
*	StamOrga is distributed in the hope that it will be useful,
*    but WITHOUT ANY WARRANTY; without even the implied warranty of
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*    GNU General Public License for more details.

*    You should have received a copy of the GNU General Public License
*    along with StamOrga.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "udpserver.h"
#include "../../Common/General/globalfunctions.h"
#include "../../Common/Network/messagecommand.h"


#define UDP_PORT 55000

UdpServer::UdpServer(GlobalData* pData)
    : BackgroundWorker()
{
    this->SetWorkerName("UDPMasterServer");

    this->m_pGlobalData = pData;
}


int UdpServer::DoBackgroundWork()
{
    this->m_pUdpMasterSocket = new QUdpSocket();
    if (!this->m_pUdpMasterSocket->bind(QHostAddress::Any, UDP_PORT)) {
        qCritical() << QString("Error binding master socket %1\n").arg(this->m_pUdpMasterSocket->errorString());
        return -1;
    }
    connect(this->m_pUdpMasterSocket, &QUdpSocket::readyRead, this, &UdpServer::readyReadMasterPort);
    connect(this->m_pUdpMasterSocket, &QUdpSocket::readChannelFinished, this, &UdpServer::readChannelFinished);

    qDebug() << "Started Master UDP Server";

    return 0;
}

void UdpServer::readyReadMasterPort()
{
    while (this->m_pUdpMasterSocket->hasPendingDatagrams()) {
        QHostAddress sender;
        quint16      port;
        QByteArray   datagram;
        datagram.resize(this->m_pUdpMasterSocket->pendingDatagramSize());

        if (this->m_pUdpMasterSocket->readDatagram(datagram.data(), datagram.size(), &sender, &port)) {
            /* Check if user Connection already exists */
            UserConnection* usrCon = this->getUserMasterConnection(sender, port);
            if (usrCon == NULL) {
                UserConnection con;
                con.userConData.m_sender        = sender;
                con.userConData.m_srcMasterPort = port;
                con.userConData.m_dstDataPort   = 0;
                con.userConData.m_srcDataPort   = 0;
                con.pctrlUdpDataServer          = NULL;
                con.pDataServer                 = NULL;
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
    MessageProtocol* msg;

    for (int i = 0; i < this->m_lUserCons.size(); i++) {

        while ((msg = this->m_lUserCons[i].msgBuffer.GetNextMessage()) != NULL) {

            if (msg->getIndex() == OP_CODE_CMD_REQ::REQ_CONNECT_USER) {

                /* Get userName from packet */
                QString          userName(QByteArray(msg->getPointerToData(), msg->getDataLength()));
                MessageProtocol* ack;

                qint32 userIndex = this->m_pGlobalData->m_UserList.getItemIndex(userName);
                if (userIndex > 0) {

                    if (this->m_lUserCons[i].userConData.m_dstDataPort == 0) { // when there is not already a port, create a new
                        this->m_lUserCons[i].userConData.m_dstDataPort = this->getFreeDataPort();
                        qInfo().noquote() << QString("Connected user %1 with data port %2 for %3")
                                                 .arg(userName)
                                                 .arg(this->m_lUserCons[i].userConData.m_dstDataPort)
                                                 .arg(this->m_lUserCons[i].userConData.m_sender.toString());
                    }

                    if (msg->getVersion() == MSG_HEADER_VERSION_START)
                        ack = new MessageProtocol(OP_CODE_CMD_RES::ACK_CONNECT_USER, (qint32) this->m_lUserCons[i].userConData.m_dstDataPort);
                    else {
                        quint8  buffer[50];
                        quint32 offset = 0;
                        memset(&buffer, 0x0, 50);
                        qint32 dataPort = qToLittleEndian(this->m_lUserCons[i].userConData.m_dstDataPort);
                        memcpy(&buffer[offset], &dataPort, sizeof(qint32));
                        offset += sizeof(qint32);
                        QString salt = this->m_pGlobalData->m_UserList.getSalt(userName);
                        memcpy(&buffer[offset], salt.toUtf8().constData(), salt.toUtf8().size());
                        offset += salt.toUtf8().size() + 1;
                        QString random = createRandomString(10);
                        memcpy(&buffer[offset], random.toUtf8().constData(), random.toUtf8().size());
                        offset += random.toUtf8().size() + 1;
                        this->m_lUserCons[i].userConData.m_randomLogin = random;

                        ack = new MessageProtocol(OP_CODE_CMD_RES::ACK_CONNECT_USER, (char*)&buffer, offset);
                    }

                    /* Create new thread if it is not running and you got a port */
                    if (this->m_lUserCons[i].userConData.m_dstDataPort && this->m_lUserCons[i].pctrlUdpDataServer == NULL) {
                        this->m_lUserCons[i].userConData.m_userName = userName;
                        this->m_lUserCons[i].pDataServer            = new UdpDataServer(&this->m_lUserCons[i].userConData,
                                                                             this->m_pGlobalData);
                        connect(this->m_lUserCons[i].pDataServer, &UdpDataServer::notifyConnectionTimedOut, this, &UdpServer::onConnectionTimedOut);
                        this->m_lUserCons[i].pctrlUdpDataServer = new BackgroundController();
                        this->m_lUserCons[i].pctrlUdpDataServer->Start(this->m_lUserCons[i].pDataServer, false);
                    }
                } else {
                    ack = new MessageProtocol(OP_CODE_CMD_RES::ACK_CONNECT_USER, ERROR_CODE_NO_USER);
                    qInfo().noquote() << QString("Wrong user tried to connect: \"%1\"").arg(userName);
                }

                /* send answer */
                const char* pData = ack->getNetworkProtocol();
                this->m_pUdpMasterSocket->writeDatagram(pData, ack->getNetworkSize(),
                                                        this->m_lUserCons[i].userConData.m_sender,
                                                        this->m_lUserCons[i].userConData.m_srcMasterPort);

                delete ack;
            }

            delete msg;
        }
    }
}

void UdpServer::onConnectionTimedOut(quint16 port)
{
    for (int i = 0; i < this->m_lUserCons.size(); i++) {
        if (this->m_lUserCons[i].userConData.m_dstDataPort == port) {
            qInfo().noquote() << QString("Connection timeout for user %1 with port %2").arg(this->m_lUserCons[i].userConData.m_userName).arg(port);
            this->m_lUserCons[i].pctrlUdpDataServer->Stop();
            delete this->m_lUserCons[i].pctrlUdpDataServer;
            this->m_lUserCons.removeAt(i);
            return;
        }
    }
}

UserConnection* UdpServer::getUserMasterConnection(QHostAddress addr, quint16 port)
{
    for (int i = 0; i < this->m_lUserCons.size(); i++) {
        if (this->m_lUserCons[i].userConData.m_sender == addr && this->m_lUserCons[i].userConData.m_srcMasterPort == port)
            return &this->m_lUserCons[i];
    }
    return NULL;
}

quint16 UdpServer::getFreeDataPort()
{
    quint16 retPort = UDP_PORT + 1;
    do {
        bool bAlreadyUsed = false;
        foreach (UserConnection usrCon, this->m_lUserCons) {
            if (usrCon.userConData.m_dstDataPort == retPort) {
                bAlreadyUsed = true;
                break;
            }
        }
        if (!bAlreadyUsed)
            return retPort;
        retPort++;
    } while (retPort < UDP_PORT + 1000);

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
