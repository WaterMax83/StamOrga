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

#include "ccontcpmainserver.h"
#include "../../Common/General/globalfunctions.h"
#include "../../Common/Network/messagecommand.h"


#define TCP_PORT 55000

cConTcpMain::cConTcpMain()
    : BackgroundWorker()
{
    this->SetWorkerName("TCPMasterServer");

//    this->m_pGlobalData = pData;
}

qint32 cConTcpMain::initialize()
{
    this->m_initialized = true;

    return ERROR_CODE_SUCCESS;
}


int cConTcpMain::DoBackgroundWork()
{
    this->m_pTcpMasterServer = new QTcpServer();
    if (!this->m_pTcpMasterServer->listen(QHostAddress::Any, TCP_PORT)) {
        qCritical() << QString("Error listening master server %1\n").arg(this->m_pTcpMasterServer->errorString());
        return -1;
    }

    connect(this->m_pTcpMasterServer, &QTcpServer::newConnection, this, &cConTcpMain::slotSocketConnected);
//    connect(this->m_pTcpMasterSocket, &QUdpSocket::readChannelFinished, this, &cConTcpMain::readChannelFinished);

    qInfo().noquote() << "Started Master TCP Server";

    return ERROR_CODE_SUCCESS;
}

void cConTcpMain::slotSocketConnected()
{
    while(this->m_pTcpMasterServer->hasPendingConnections()) {
        QTcpSocket* socket = this->m_pTcpMasterServer->nextPendingConnection();
    }
}

void cConTcpMain::readyReadMasterPort()
{
//    m_pTcpMasterServer->
//    while (this->m_pUdpMasterSocket->hasPendingDatagrams()) {
//        QHostAddress sender;
//        quint16      port;
//        QByteArray   datagram;
//        datagram.resize(this->m_pUdpMasterSocket->pendingDatagramSize());

//        if (this->m_pUdpMasterSocket->readDatagram(datagram.data(), datagram.size(), &sender, &port)) {
//            /* Check if user Connection already exists */
//            UserConnection* usrCon = this->getUserMasterConnection(sender, port);
//            if (usrCon == NULL) {
//                UserConnection con;
//                con.userConData.m_sender        = sender;
//                con.userConData.m_srcMasterPort = port;
//                con.userConData.m_dstDataPort   = 0;
//                con.userConData.m_srcDataPort   = 0;
//                con.pctrlUdpDataServer          = NULL;
//                con.pDataServer                 = NULL;
//                this->m_lUserCons.append(con);
//                usrCon = &this->m_lUserCons[this->m_lUserCons.size() - 1];
//            }
//            usrCon->msgBuffer.StoreNewData(datagram);
//        }
//    }

//    this->checkNewOncomingData();
}

//void cConTcpMain::checkNewOncomingData()
//{
//    MessageProtocol* msg;

//    for (int i = 0; i < this->m_lUserCons.size(); i++) {

//        while ((msg = this->m_lUserCons[i].msgBuffer.GetNextMessage()) != NULL) {

//            if (msg->getIndex() == OP_CODE_CMD_REQ::REQ_CONNECT_USER) {

//                /* Get userName from packet */
//                QString          userName(QByteArray(msg->getPointerToData(), msg->getDataLength()));
//                MessageProtocol* ack;

//                qint32 userIndex = this->m_pGlobalData->m_UserList.getItemIndex(userName);
//                if (userIndex > 0) {

//                    if (this->m_lUserCons[i].userConData.m_dstDataPort == 0) { // when there is not already a port, create a new
//                        this->m_lUserCons[i].userConData.m_dstDataPort = this->getFreeDataPort();
//                        qInfo().noquote() << QString("Connected user %1 with data port %2 for %3")
//                                                 .arg(userName)
//                                                 .arg(this->m_lUserCons[i].userConData.m_dstDataPort)
//                                                 .arg(this->m_lUserCons[i].userConData.m_sender.toString());
//                    }

//                    if (msg->getVersion() == MSG_HEADER_VERSION_START)
//                        ack = new MessageProtocol(OP_CODE_CMD_RES::ACK_CONNECT_USER, (qint32) this->m_lUserCons[i].userConData.m_dstDataPort);
//                    else {
//                        quint8  buffer[50];
//                        quint32 offset = 0;
//                        memset(&buffer, 0x0, 50);
//                        qint32 dataPort = qToLittleEndian(this->m_lUserCons[i].userConData.m_dstDataPort);
//                        memcpy(&buffer[offset], &dataPort, sizeof(qint32));
//                        offset += sizeof(qint32);
//                        QString salt = this->m_pGlobalData->m_UserList.getSalt(userName);
//                        memcpy(&buffer[offset], salt.toUtf8().constData(), salt.toUtf8().size());
//                        offset += salt.toUtf8().size() + 1;
//                        QString random = createRandomString(10);
//                        memcpy(&buffer[offset], random.toUtf8().constData(), random.toUtf8().size());
//                        offset += random.toUtf8().size() + 1;
//                        this->m_lUserCons[i].userConData.m_randomLogin = random;

//                        ack = new MessageProtocol(OP_CODE_CMD_RES::ACK_CONNECT_USER, (char*)&buffer, offset);
//                    }

//                    /* Create new thread if it is not running and you got a port */
//                    if (this->m_lUserCons[i].userConData.m_dstDataPort && this->m_lUserCons[i].pctrlUdpDataServer == NULL) {
//                        this->m_lUserCons[i].userConData.m_userName = userName;
//                        this->m_lUserCons[i].userConData.m_userID   = userIndex;
//                        this->m_lUserCons[i].pDataServer            = new UdpDataServer(&this->m_lUserCons[i].userConData,
//                                                                             this->m_pGlobalData);
//                        connect(this->m_lUserCons[i].pDataServer, &UdpDataServer::notifyConnectionTimedOut, this, &cConTcpMain::onConnectionTimedOut);
//                        this->m_lUserCons[i].pctrlUdpDataServer = new BackgroundController();
//                        this->m_lUserCons[i].pctrlUdpDataServer->Start(this->m_lUserCons[i].pDataServer, false);
//                    }
//                } else {
//                    ack = new MessageProtocol(OP_CODE_CMD_RES::ACK_CONNECT_USER, ERROR_CODE_NO_USER);
//                    qInfo().noquote() << QString("Wrong user tried to connect: \"%1\"").arg(userName);
//                }

//                /* send answer */
//                const char* pData = ack->getNetworkProtocol();
//                this->m_pUdpMasterSocket->writeDatagram(pData, ack->getNetworkSize(),
//                                                        this->m_lUserCons[i].userConData.m_sender,
//                                                        this->m_lUserCons[i].userConData.m_srcMasterPort);

//                delete ack;
//            }

//            delete msg;
//        }
//    }
//}

//void cConTcpMain::onConnectionTimedOut(quint16 port)
//{
//    for (int i = 0; i < this->m_lUserCons.size(); i++) {
//        if (this->m_lUserCons[i].userConData.m_dstDataPort == port) {
//            qInfo().noquote() << QString("Connection timeout for user %1 with port %2").arg(this->m_lUserCons[i].userConData.m_userName).arg(port);
//            this->m_lUserCons[i].pctrlUdpDataServer->Stop();
//            delete this->m_lUserCons[i].pctrlUdpDataServer;
//            this->m_lUserCons.removeAt(i);
//            return;
//        }
//    }
//}

//UserConnection* cConTcpMain::getUserMasterConnection(QHostAddress addr, quint16 port)
//{
//    for (int i = 0; i < this->m_lUserCons.size(); i++) {
//        if (this->m_lUserCons[i].userConData.m_sender == addr && this->m_lUserCons[i].userConData.m_srcMasterPort == port)
//            return &this->m_lUserCons[i];
//    }
//    return NULL;
//}

//quint16 cConTcpMain::getFreeDataPort()
//{
//    quint16 retPort = UDP_PORT + 1;
//    do {
//        bool bAlreadyUsed = false;
//        foreach (UserConnection usrCon, this->m_lUserCons) {
//            if (usrCon.userConData.m_dstDataPort == retPort) {
//                bAlreadyUsed = true;
//                break;
//            }
//        }
//        if (!bAlreadyUsed)
//            return retPort;
//        retPort++;
//    } while (retPort < UDP_PORT + 1000);

//    return 0;
//}

//void cConTcpMain::readChannelFinished()
//{
//    //    qDebug() << "Signal readChannelFinished was called";
//}


cConTcpMain::~cConTcpMain()
{
    if (this->m_pTcpMasterServer != NULL)
        delete this->m_pTcpMasterServer;
}
