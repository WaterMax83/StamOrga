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

#include "../../Common/General/globalfunctions.h"
#include "../Common/Network/messagecommand.h"
#include "../Connection/cconmanager.h"
#include "cdataconsolemanager.h"

cDataConsoleManager* g_DataConsoleManager;

cDataConsoleManager::cDataConsoleManager(QObject* parent)
    : cGenDisposer(parent)
{
}

qint32 cDataConsoleManager::initialize()
{
    this->m_consoleOutput = "";

    this->m_initialized = true;

    return ERROR_CODE_SUCCESS;
}

qint32 cDataConsoleManager::startSendConsoleCommand(const QString command)
{
    if (!this->m_initialized)
        return ERROR_CODE_NOT_INITIALIZED;

    QJsonObject rootObj;
    rootObj.insert("cmd", command);

    TcpDataConRequest* req = new TcpDataConRequest(OP_CODE_CMD_REQ::REQ_SEND_CONSOLE_CMD);
    req->m_lData           = QJsonDocument(rootObj).toJson(QJsonDocument::Compact);

    g_ConManager->sendNewRequest(req);
    return ERROR_CODE_SUCCESS;
}

qint32 cDataConsoleManager::handleConsoleCommandResponse(MessageProtocol* msg)
{
    if (!this->m_initialized)
        return ERROR_CODE_NOT_INITIALIZED;

    QByteArray  data(msg->getPointerToData());
    QJsonObject rootObj = QJsonDocument::fromJson(data).object();

    qint32  rValue = rootObj.value("ack").toInt(ERROR_CODE_NOT_FOUND);
    QString result = rootObj.value("result").toString();

    QByteArray output     = QByteArray::fromBase64(result.toUtf8());
    this->m_consoleOutput = qUncompress(output);

    return rValue;
}

QString cDataConsoleManager::getLastConsoleOutput()
{
    return this->m_consoleOutput;
}
