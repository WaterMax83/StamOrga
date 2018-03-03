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

#include <QtCore/QByteArray>
#include <QtCore/QtEndian>

#include "General/globalfunctions.h"
#include "General/globaltiming.h"
#include "Network/messagecommand.h"
#include "Network/messageprotocol.h"
#include "ccontcpmain.h"

cConTcpMain::cConTcpMain()
    : BackgroundWorker()
{
    this->SetWorkerName("cConTcpMain");
    this->m_initialized = false;
}

qint32 cConTcpMain::initialize(QString host)
{
    this->m_initialized = true;

    this->m_hMasterReceiver = QHostAddress(host);
    if (m_hMasterReceiver.isNull())
        return ERROR_CODE_WRONG_PARAMETER;

    this->m_initialized;

    return ERROR_CODE_SUCCESS;
}


int cConTcpMain::DoBackgroundWork()
{
    this->m_pMasterTcpSocket = new QTcpSocket();

    if (!this->m_pMasterTcpSocket->bind()) {
        emit this->connectionRequestFinished(0, this->m_pMasterTcpSocket->errorString(), "", "");
        return -1;
    }

    //    /* Wait till IP address was set */
    //    qint32 sleepCounter = 0;
    //    while (!this->m_pGlobalData->isIPLookUpDone()) {
    //        QThread::msleep(100);
    //        sleepCounter++;
    //        if (sleepCounter >= 10)
    //            break;
    //    }

    connect(this->m_pMasterTcpSocket, &QTcpSocket::connected, this, &cConTcpMain::slotMasterSocketConnected);
    typedef void (QAbstractSocket::*QAbstractSocketErrorSignal)(QAbstractSocket::SocketError);
    connect(this->m_pMasterTcpSocket, static_cast<QAbstractSocketErrorSignal>(&QAbstractSocket::error), this, &cConTcpMain::slotMainSocketError);
    connect(this->m_pMasterTcpSocket, &QTcpSocket::readyRead, this, &cConTcpMain::slotReadyReadMasterSocket);

    //

    this->m_pConTimeout = new QTimer();
    this->m_pConTimeout->setSingleShot(true);
    connect(this->m_pConTimeout, &QTimer::timeout, this, &cConTcpMain::slotConnectionTimeoutFired);

    return 0;
}

void cConTcpMain::slotMasterSocketConnected()
{
}

//void cConTcpMain::slotNewBindingPortRequest()
//{
//    if (this->m_pMasterUdpSocket->state() != QAbstractSocket::UnconnectedState)
//        this->m_pMasterUdpSocket->disconnectFromHost();
//    this->m_pMasterUdpSocket->bind();
//    this->m_hMasterReceiver = QHostAddress(this->m_pGlobalData->ipAddr());
//    qDebug() << "Master Setting New Binding Request";
//}

//void cConTcpMain::slotSendNewMainConRequest(QString username)
//{
//    QByteArray aData;
//    aData.append(username);
//    this->m_userName = username;

//    MessageProtocol msg(OP_CODE_CMD_REQ::REQ_CONNECT_USER, aData);

//    const char* pData = msg.getNetworkProtocol();
//    this->m_pMasterUdpSocket->writeDatagram(pData, msg.getNetworkSize(), this->m_hMasterReceiver, this->m_pGlobalData->conMasterPort());

//    this->m_pConTimeout->start(SOCKET_TIMEOUT_MS);
//}


void cConTcpMain::slotMainSocketError(QAbstractSocket::SocketError socketError)
{
    qCritical().noquote() << QString("Socket Error %1 - %2 ").arg(socketError).arg(this->m_pMasterTcpSocket->errorString());
}

void cConTcpMain::slotConnectionTimeoutFired()
{
    //    emit this->connectionRequestFinished(ERROR_CODE_TIMEOUT, "Timeout", "", "");
    //    qInfo().noquote() << QString("Main Con Timeout %1").arg(this->m_pMasterUdpSocket->errorString());
}

void cConTcpMain::slotReadyReadMasterSocket()
{
    //    while (this->m_pMasterUdpSocket->hasPendingDatagrams()) {
    //        QHostAddress sender;
    //        quint16      port;
    //        QByteArray   datagram;
    //        datagram.resize(this->m_pMasterUdpSocket->pendingDatagramSize());

    //        if (this->m_pMasterUdpSocket->readDatagram(datagram.data(), datagram.size(), &sender, &port)) {
    //            if (this->m_pGlobalData->conMasterPort() == port
    //                && this->m_hMasterReceiver.toIPv4Address() == sender.toIPv4Address()) {

    //                this->m_messageBuffer.StoreNewData(datagram);
    //            }
    //        }
    //    }
    //    this->checkNewOncomingData();
}

//void cConTcpMain::checkNewOncomingData()
//{
//    MessageProtocol* msg;
//    while ((msg = this->m_messageBuffer.GetNextMessage()) != NULL) {

//        if (msg->getIndex() == OP_CODE_CMD_RES::ACK_CONNECT_USER) {
//            this->m_pConTimeout->stop();
//            if (msg->getDataLength() < 4)
//                emit this->connectionRequestFinished(ERROR_CODE_WRONG_SIZE, QString("Datalength %1 is wrong, expected >4").arg(msg->getDataLength()), "", "");
//            else {
//                const char* pData = msg->getPointerToData();
//                qint32      rValue;
//                memcpy(&rValue, pData, sizeof(qint32));
//                if (qToLittleEndian(rValue) > ERROR_CODE_NO_ERROR) {
//                    if (msg->getDataLength() < 4 + 8 + 1)
//                        emit this->connectionRequestFinished(ERROR_CODE_WRONG_SIZE, QString("Datalength %1 is wrong, expected >13").arg(msg->getDataLength()), "", "");
//                    else {
//                        QString salt(pData + sizeof(qint32));
//                        QString random(pData + sizeof(qint32) + 1 + salt.toUtf8().size());

//                        if (this->m_pGlobalData->userName() != this->m_userName) {
//                            this->m_pGlobalData->setUserName(this->m_userName);
//                            this->m_pGlobalData->saveGlobalUserSettings();
//                        }
//                        emit this->connectionRequestFinished(qToLittleEndian(rValue), "", salt, random);
//                    }
//                } else if (qToLittleEndian(rValue) == ERROR_CODE_NO_USER)
//                    emit this->connectionRequestFinished(qToLittleEndian(rValue), QString("Wrong user %1").arg(this->m_userName), "", "");
//                else
//                    emit this->connectionRequestFinished(qToLittleEndian(rValue), QString("unkown error %1").arg(qToLittleEndian(rValue)), "", "");
//            }
//        }
//        delete msg;
//    }
//}


cConTcpMain::~cConTcpMain()
{
    if (this->m_pMasterTcpSocket != NULL)
        delete this->m_pMasterTcpSocket;
    this->m_pMasterTcpSocket = NULL;
}
