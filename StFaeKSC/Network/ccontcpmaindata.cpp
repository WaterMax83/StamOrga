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

#include "../Common/General/config.h"
#include "../Common/General/globalfunctions.h"
#include "../Common/Network/messagecommand.h"
#include "../General/globaldata.h"
#include "General/pushnotification.h"
#include "ccontcpmaindata.h"

extern GlobalData* g_GlobalData;

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

MessageProtocol* cConTcpMainData::getNewUserAcknowledge(const QString& userName, const QHostAddress& addr, const cConSslUsage& sslUsage)
{
    QJsonObject rootObj;
    qint32      userIndex = this->m_pListedUser->getItemIndex(userName);
    if (userIndex > 0) {

        qInfo().noquote() << QString("Connected user %1").arg(userName);

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
        userCon->m_userConData.m_randomLogin = random;
        userCon->m_pDataServer               = new cConTcpDataServer();
        userCon->m_pDataServer->initialize(&userCon->m_userConData, sslUsage);
        this->connect(userCon->m_pDataServer, &cConTcpDataServer::signalServerClosed, this, &cConTcpMainData::slotServerClosed);

        userCon->m_pctrlTcpDataServer = new BackgroundController();
        userCon->m_pctrlTcpDataServer->Start(userCon->m_pDataServer, false);

        QMutexLocker lock(&this->m_mutex);

        this->m_lTcpUserCons.append(userCon);

    } else {
        rootObj.insert("ack", ERROR_CODE_NO_USER);
        qInfo().noquote() << QString("Wrong user tried to connect: \"%1\"").arg(userName);
    }

    QByteArray data = QJsonDocument(rootObj).toJson(QJsonDocument::Compact);

    return new MessageProtocol(OP_CODE_CMD_RES::ACK_CONNECT_USER, data);
}

void cConTcpMainData::slotServerClosed(quint16 destPort)
{
    QMutexLocker lock(&this->m_mutex);

    for (int i = 0; i < this->m_lTcpUserCons.count(); i++) {
        TcpUserConnection* pCon = this->m_lTcpUserCons.at(i);
        if (pCon->m_userConData.m_dstDataPort == destPort) {
            pCon->m_pDataServer->terminate();
            pCon->m_pctrlTcpDataServer->Stop(true);
            this->m_lTcpUserCons.removeAt(i);
            break;
        }
    }
}

quint16 cConTcpMainData::getFreeDataPort()
{
    QMutexLocker lock(&this->m_mutex);

    quint16 retPort = TCP_PORT + 5;
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
