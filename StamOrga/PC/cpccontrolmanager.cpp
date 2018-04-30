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

#include <QtCore/QJsonArray>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>

#include "../Common/General/globalfunctions.h"
#include "../Common/Network/messagecommand.h"
#include "Connection/cconmanager.h"
#include "cpccontrolmanager.h"

cPCControlManager* g_PCControlManager;

cPCControlManager::cPCControlManager(QObject* parent)
    : cGenDisposer(parent)
{
}

qint32 cPCControlManager::initialize()
{
    if (this->m_initialized)
        return ERROR_CODE_SUCCESS;

    this->m_initialized = true;

    return ERROR_CODE_SUCCESS;
}

qint32 cPCControlManager::terminate()
{
    QMutexLocker lock(&this->m_mutex);

    if (!this->m_initialized)
        return ERROR_CODE_NOT_INITIALIZED;

    this->m_initialized = false;

    return ERROR_CODE_SUCCESS;
}

qint32 cPCControlManager::refreshControlList()
{
    QMutexLocker lock(&this->m_mutex);

    QJsonObject rootObj;
    rootObj.insert("cmd", "refresh");

    TcpDataConRequest* req = new TcpDataConRequest(OP_CODE_CMD_REQ::REQ_CMD_CONTROL);
    req->m_lData           = QJsonDocument(rootObj).toJson(QJsonDocument::Compact);

    g_ConManager->sendNewRequest(req);
    return ERROR_CODE_SUCCESS;
}

qint32 cPCControlManager::saveControlList()
{
    QMutexLocker lock(&this->m_mutex);

    QJsonObject rootObj;
    rootObj.insert("cmd", "save");
    QJsonArray statsArr;
    foreach (QString stat, this->m_statistic)
        statsArr.append(stat.toInt());

    rootObj.insert("stats", statsArr);

    TcpDataConRequest* req = new TcpDataConRequest(OP_CODE_CMD_REQ::REQ_CMD_CONTROL);
    req->m_lData           = QJsonDocument(rootObj).toJson(QJsonDocument::Compact);

    g_ConManager->sendNewRequest(req);
    return ERROR_CODE_SUCCESS;
}


qint32 cPCControlManager::handleControlCommand(MessageProtocol* msg)
{
    QMutexLocker lock(&this->m_mutex);

    if (!this->m_initialized)
        return ERROR_CODE_NOT_INITIALIZED;

    QByteArray  data(msg->getPointerToData());
    QJsonObject rootObj = QJsonDocument::fromJson(data).object();

    qint32  result = rootObj.value("ack").toInt(ERROR_CODE_NOT_FOUND);
    QString cmd    = rootObj.value("cmd").toString();
    if (cmd == "save") {
        this->m_mutex.unlock();
        this->refreshControlList();
        this->m_mutex.lock();
    } else if (cmd == "refresh") {
        this->m_statistic.clear();
        QJsonArray statsArr = rootObj.value("stats").toArray();
        for (int i = 0; i < statsArr.size(); i++) {
            this->m_statistic.append(QString::number(statsArr.at(i).toInt()));
        }
    }

    return result;
}

qint32 cPCControlManager::setStatistic(QString stats)
{
    QMutexLocker lock(&this->m_mutex);
    QStringList  statsList = stats.split("\n");
    for (int i = statsList.size() - 1; i >= 0; i--) {
        QString stat = statsList.at(i);
        bool    ok   = false;
        qint32  tmp  = stat.toInt(&ok);
        if (!ok || tmp < 2015 || tmp > 2025)
            statsList.removeAt(i);
    }

    this->m_statistic = statsList;

    return ERROR_CODE_SUCCESS;
}

QString cPCControlManager::getStastistic()
{
    QMutexLocker lock(&this->m_mutex);

    QString rValue;
    for (int i = 0; i < this->m_statistic.size(); i++)
        rValue.append(this->m_statistic.at(i) + "\n");

    return rValue;
}
