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
    this->m_initialized = false;
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

    QJsonArray gamesArr;
    foreach (QString game, this->m_onlineGames) {
        QStringList gameInfos = game.split(";");
        QJsonObject gameObj;
        gameObj.insert("comp", gameInfos.at(0));
        gameObj.insert("season", gameInfos.at(1).toInt());
        gameObj.insert("index", gameInfos.at(2).toInt());
        gamesArr.append(gameObj);
    }

    QJsonArray addrArr;
    foreach (QString adr, this->m_smtpAddresses)
        addrArr.append(adr);
    QJsonObject smtpObj;
    smtpObj.insert("login", this->m_smtpLogin);
    smtpObj.insert("password", this->m_smtpPassword);
    smtpObj.insert("addresses", addrArr);

    rootObj.insert("stats", statsArr);
    rootObj.insert("readOnlineGame", gamesArr);
    rootObj.insert("smtp", smtpObj);

    TcpDataConRequest* req = new TcpDataConRequest(OP_CODE_CMD_REQ::REQ_CMD_CONTROL);
    req->m_lData           = QJsonDocument(rootObj).toJson(QJsonDocument::Compact);

    g_ConManager->sendNewRequest(req);
    return ERROR_CODE_SUCCESS;
}


qint32 cPCControlManager::handleControlCommand(MessageProtocol* msg)
{
    QMutexLocker lock(&this->m_mutex);

    qInfo() << "Handle control " << this->m_initialized;

    if (!this->m_initialized)
        return ERROR_CODE_NOT_INITIALIZED;

    QByteArray  data(msg->getPointerToData());
    QJsonObject rootObj = QJsonDocument::fromJson(data).object();

    qInfo() << rootObj;

    qint32  result = rootObj.value("ack").toInt(ERROR_CODE_NOT_FOUND);
    QString cmd    = rootObj.value("cmd").toString();
    if (cmd == "save") {
        this->m_mutex.unlock();
        this->refreshControlList();
        this->m_mutex.lock();
    } else if (cmd == "refresh") {
        this->m_statistic.clear();
        this->m_onlineGames.clear();
        this->m_smtpAddresses.clear();

        QJsonArray statsArr = rootObj.value("stats").toArray();
        for (int i = 0; i < statsArr.size(); i++)
            this->m_statistic.append(QString::number(statsArr.at(i).toInt()));

        QJsonArray gamesArr = rootObj.value("readOnlineGame").toArray();
        for (int i = 0; i < gamesArr.size(); i++) {
            QJsonObject gameObj  = gamesArr.at(i).toObject();
            QString     comp     = gameObj.value("comp").toString();
            qint32      maxIndex = gameObj.value("index").toInt(-1);
            qint32      season   = gameObj.value("season").toInt(-1);

            if (comp.isEmpty() || maxIndex <= 0 || season < MIN_GAME_YEAR || season > MAX_GAME_YEAR)
                continue;
            this->m_onlineGames.append(QString("%1;%2;%3").arg(comp).arg(season).arg(maxIndex));
        }

        QJsonObject smtpObj = rootObj.value("smtp").toObject();
        if (!smtpObj.isEmpty()) {
            this->m_smtpLogin    = smtpObj.value("login").toString();
            this->m_smtpPassword = smtpObj.value("password").toString();
            QJsonArray addrArr   = smtpObj.value("addresses").toArray();
            for (int i = 0; i < addrArr.size(); i++)
                this->m_smtpAddresses.append(addrArr.at(i).toString());
        }
    }

    return result;
}

qint32 cPCControlManager::setStatistic(QString stats)
{
    if (!this->m_initialized)
        return ERROR_CODE_NOT_INITIALIZED;

    QMutexLocker lock(&this->m_mutex);

    QStringList statsList = stats.split("\n");
    for (int i = statsList.size() - 1; i >= 0; i--) {
        QString stat = statsList.at(i);
        bool    ok   = false;
        qint32  tmp  = stat.toInt(&ok);
        if (!ok || tmp < MIN_GAME_YEAR || tmp > MAX_GAME_YEAR)
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

qint32 cPCControlManager::setOnlineGames(QString games)
{
    if (!this->m_initialized)
        return ERROR_CODE_NOT_INITIALIZED;

    QMutexLocker lock(&this->m_mutex);

    QStringList gamesList = games.split("\n");
    for (int i = gamesList.size() - 1; i >= 0; i--) {
        QStringList gameInfos = gamesList.at(i).split(";");
        if (gameInfos.size() != 3) {
            gamesList.removeAt(i);
            continue;
        }
        bool   ok1 = false, ok2 = false;
        qint32 tmp1 = gameInfos.at(1).toInt(&ok1);
        qint32 tmp2 = gameInfos.at(2).toInt(&ok2);
        if (!ok1 || !ok2 || tmp1 < MIN_GAME_YEAR || tmp1 > MAX_GAME_YEAR || tmp2 <= 0 || tmp2 > 64)
            gamesList.removeAt(i);
    }

    this->m_onlineGames = gamesList;

    return ERROR_CODE_SUCCESS;
}

QString cPCControlManager::getOnlineGames()
{
    QMutexLocker lock(&this->m_mutex);

    QString rValue;
    for (int i = 0; i < this->m_onlineGames.size(); i++)
        rValue.append(this->m_onlineGames.at(i) + "\n");

    return rValue;
}

qint32 cPCControlManager::setSmtpData(QString login, QString password, QString addresses)
{
    if (!this->m_initialized)
        return ERROR_CODE_NOT_INITIALIZED;

    QMutexLocker lock(&this->m_mutex);

    this->m_smtpLogin    = login;
    this->m_smtpPassword = password;
    QStringList addrList = addresses.split("\n");
    for (int i = addrList.size() - 1; i >= 0; i--) {
        QString addr = addrList.at(i);
        if (!addr.contains("@"))
            addrList.removeAt(i);
    }

    this->m_smtpAddresses = addrList;

    return ERROR_CODE_SUCCESS;
}

qint32 cPCControlManager::getStmpData(QString& login, QString& password, QString& addresses)
{
    if (!this->m_initialized)
        return ERROR_CODE_NOT_INITIALIZED;

    QMutexLocker lock(&this->m_mutex);

    login    = this->m_smtpLogin;
    password = this->m_smtpPassword;
    addresses.clear();
    for (int i = 0; i < this->m_smtpAddresses.size(); i++)
        addresses.append(this->m_smtpAddresses.at(i) + "\n");

    return ERROR_CODE_SUCCESS;
}
