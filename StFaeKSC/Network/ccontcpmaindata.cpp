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

#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>

#include "../Common/General/globalfunctions.h"
#include "../Common/Network/messagecommand.h"
#include "ccontcpmaindata.h"

cConTcpMainData g_ConTcpMainData;

cConTcpMainData::cConTcpMainData()
{
}

qint32 cConTcpMainData::initialize(ListedUser* pListedUser)
{
    this->m_pListedUser = pListedUser;
    this->m_initialized = true;

    return ERROR_CODE_SUCCESS;
}

MessageProtocol* cConTcpMainData::getNewUserAcknowledge(const QString userName, const QHostAddress addr)
{
    QJsonObject rootObj;
    qint32      userIndex = this->m_pListedUser->getItemIndex(userName);
    if (userIndex > 0) {

        //                    if (this->m_lUserCons[i].userConData.m_dstDataPort == 0) { // when there is not already a port, create a new
        //                        this->m_lUserCons[i].userConData.m_dstDataPort = this->getFreeDataPort();
        qInfo().noquote() << QString("Connected user %1").arg(userName);
        //                                                         .arg(this->m_lUserCons[i].userConData.m_dstDataPort)
        //                                                         .arg(this->m_lUserCons[i].userConData.m_sender.toString());
        //                    }

        quint16 port = this->getFreeDataPort();
        rootObj.insert("port", port);

        rootObj.insert("salt", this->m_pListedUser->getSalt(userName));

        QString random = createRandomString(10);
        rootObj.insert("random", random);

        rootObj.insert("ack", ERROR_CODE_SUCCESS);


        TcpUserConnection* userCon           = new TcpUserConnection();
        userCon->m_userConData.m_dstDataPort = port;
        userCon->m_userConData.m_userName    = userName;
        userCon->m_userConData.m_userID      = userIndex;
        userCon->m_userConData.m_sender      = addr;
        userCon->m_pDataServer               = new cConTcpDataServer();
        userCon->m_pDataServer->initialize(&userCon->m_userConData);
        this->connect(userCon->m_pDataServer, &cConTcpDataServer::signalServerClosed, this, &cConTcpMainData::slotServerClosed);

        userCon->m_pctrlTcpDataServer = new BackgroundController();
        userCon->m_pctrlTcpDataServer->Start(userCon->m_pDataServer, false);
        this->m_lTcpUserCons.append(userCon);

        //                    /* Create new thread if it is not running and you got a port */
        //                    if (this->m_lUserCons[i].userConData.m_dstDataPort && this->m_lUserCons[i].pctrlUdpDataServer == NULL) {
        //                        this->m_lUserCons[i].userConData.m_userName = userName;
        //                        this->m_lUserCons[i].userConData.m_userID   = userIndex;
        //                        this->m_lUserCons[i].pDataServer            = new UdpDataServer(&this->m_lUserCons[i].userConData,
        //                                                                             this->m_pGlobalData);
        //                        connect(this->m_lUserCons[i].pDataServer, &UdpDataServer::notifyConnectionTimedOut, this, &cConTcpMainSocket::onConnectionTimedOut);
        //                        this->m_lUserCons[i].pctrlUdpDataServer = new BackgroundController();
        //                        this->m_lUserCons[i].pctrlUdpDataServer->Start(this->m_lUserCons[i].pDataServer, false);
        //                    }
    } else {
        rootObj.insert("ack", ERROR_CODE_NO_USER);
        qInfo().noquote() << QString("Wrong user tried to connect: \"%1\"").arg(userName);
    }


    QByteArray data = QJsonDocument(rootObj).toJson(QJsonDocument::Compact);

    return new MessageProtocol(OP_CODE_CMD_RES::ACK_CONNECT_USER, data);
}

void cConTcpMainData::slotServerClosed(quint16 destPort)
{
    for (int i = 0; i < this->m_lTcpUserCons.count(); i++) {
        TcpUserConnection* pCon = this->m_lTcpUserCons.at(i);
        if (pCon->m_userConData.m_dstDataPort == destPort) {
            pCon->m_pDataServer->terminate();
            pCon->m_pctrlTcpDataServer->Stop();
            this->m_lTcpUserCons.removeAt(i);
            break;
        }
    }
}

quint16 cConTcpMainData::getFreeDataPort()
{
    quint16 retPort = TCP_PORT + 1;
    do {
        bool bAlreadyUsed = false;
        foreach (TcpUserConnection* pUsrCon, this->m_lTcpUserCons) {
            if (pUsrCon->m_userConData.m_dstDataPort == retPort) {
                bAlreadyUsed = true;
                break;
            }
        }
        if (!bAlreadyUsed)
            return retPort;
        retPort++;
    } while (retPort < TCP_PORT + 1000);

    return 0;
}
