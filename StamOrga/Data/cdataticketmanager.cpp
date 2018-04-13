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
#include <QtQml/QQmlEngine>

#include "cdataticketmanager.h"

#include "../Common/General/globalfunctions.h"
#include "../Common/General/globaltiming.h"
#include "../Common/Network/messagecommand.h"
#include "../Connection/cconmanager.h"
#include "../Connection/cconusersettings.h"
#include "../cstaglobalsettings.h"
#include "../cstasettingsmanager.h"
#include "cdatagamesmanager.h"


// clang-format off

#define ITEM_INDEX          "index"

#define SEASONTICKET_GROUP  "SEASONTICKET_LIST"
#define TICKET_NAME         "name"
#define TICKET_PLACE        "place"
#define TICKET_DISCOUNT     "discount"
#define TICKET_USER_INDEX   "userIndex"

#define LOCAL_TICKET_UDPATE "LocalTicketsUpdateTime"
#define SERVER_TICKET_UDPATE "ServerTicketsUpdateTime"

// clang-format on

cDataTicketManager g_DataTicketManager;

cDataTicketManager::cDataTicketManager(QObject* parent)
    : cGenDisposer(parent)
{
}

qint32 cDataTicketManager::initialize()
{
    qRegisterMetaType<SeasonTicketItem*>("SeasonTicketItem*");

    if (!g_StaGlobalSettings.getSaveInfosOnApp())
        g_StaSettingsManager.removeGroup(SEASONTICKET_GROUP);

    qint64 iValue;
    g_StaSettingsManager.getInt64Value(SEASONTICKET_GROUP, LOCAL_TICKET_UDPATE, iValue);
    this->m_stLastLocalUpdateTimeStamp = iValue;
    g_StaSettingsManager.getInt64Value(SEASONTICKET_GROUP, SERVER_TICKET_UDPATE, iValue);
    this->m_stLastServerUpdateTimeStamp = iValue;

    this->m_initialized = true;

    QString value;
    qint32  index = 0;
    while (g_StaSettingsManager.getValue(SEASONTICKET_GROUP, TICKET_NAME, index, value) == ERROR_CODE_SUCCESS) {
        SeasonTicketItem* pTicket = new SeasonTicketItem();
        pTicket->setName(value);
        g_StaSettingsManager.getValue(SEASONTICKET_GROUP, TICKET_PLACE, index, value);
        pTicket->setPlace(value);
        g_StaSettingsManager.getInt64Value(SEASONTICKET_GROUP, TICKET_DISCOUNT, index, iValue);
        pTicket->setDiscount(iValue);
        g_StaSettingsManager.getInt64Value(SEASONTICKET_GROUP, ITEM_INDEX, index, iValue);
        pTicket->setIndex(iValue);
        g_StaSettingsManager.getInt64Value(SEASONTICKET_GROUP, TICKET_USER_INDEX, index, iValue);
        pTicket->setUserIndex(iValue);

        QQmlEngine::setObjectOwnership(pTicket, QQmlEngine::CppOwnership);
        this->addNewSeasonTicket(pTicket);
        index++;
    }

    return ERROR_CODE_SUCCESS;
}


void cDataTicketManager::addNewSeasonTicket(SeasonTicketItem* sTicket, const quint16 updateIndex)
{
    if (!this->m_initialized)
        return;

    SeasonTicketItem* pTicket = this->getSeasonTicket(sTicket->index());
    if (pTicket == NULL) {
        QMutexLocker lock(&this->m_mutex);
        this->m_lTickets.append(sTicket);
        return;
    } else if (updateIndex == UpdateIndex::UpdateDiff) {
        if (pTicket->name() != sTicket->name()) {
            pTicket->setName(sTicket->name());
        }
        if (pTicket->place() != sTicket->place()) {
            pTicket->setPlace(sTicket->place());
        }
        if (pTicket->discount() != sTicket->discount()) {
            pTicket->setDiscount(sTicket->discount());
        }

        delete sTicket;
    }
}

SeasonTicketItem* cDataTicketManager::getSeasonTicket(qint32 ticketIndex)
{
    if (!this->m_initialized)
        return NULL;

    QMutexLocker lock(&this->m_mutex);

    for (int i = 0; i < this->m_lTickets.size(); i++) {
        if (this->m_lTickets[i]->index() == ticketIndex)
            return this->m_lTickets[i];
    }
    return NULL;
}

qint32 cDataTicketManager::getSeasonTicketLength()
{
    QMutexLocker lock(&this->m_mutex);
    return this->m_lTickets.size();
}

SeasonTicketItem* cDataTicketManager::getSeasonTicketFromArrayIndex(int index)
{
    QMutexLocker lock(&this->m_mutex);

    if (index < this->m_lTickets.size()) {
        return this->m_lTickets.at(index);
    }
    return NULL;
}

QString cDataTicketManager::getSeasonTicketLastLocalUpdateString()
{
    if (!this->m_initialized)
        return "";

    QMutexLocker lock(&this->m_mutex);
    return QDateTime::fromMSecsSinceEpoch(this->m_stLastLocalUpdateTimeStamp).toString("dd.MM.yy hh:mm:ss");
}

qint32 cDataTicketManager::startListSeasonTickets()
{
    if (!this->m_initialized)
        return ERROR_CODE_NOT_INITIALIZED;

    QMutexLocker lock(&this->m_mutex);

    QJsonObject rootObj;
    if (this->m_stLastLocalUpdateTimeStamp + TIMEOUT_UPDATE_TICKETS > QDateTime::currentMSecsSinceEpoch() && this->m_lTickets.count() > 0)
        rootObj.insert("index", UpdateIndex::UpdateDiff);
    else
        rootObj.insert("index", UpdateIndex::UpdateAll);

    rootObj.insert("timestamp", this->m_stLastServerUpdateTimeStamp);

    TcpDataConRequest* req = new TcpDataConRequest(OP_CODE_CMD_REQ::REQ_GET_TICKETS_LIST);
    req->m_lData           = QJsonDocument(rootObj).toJson(QJsonDocument::Compact);

    g_ConManager.sendNewRequest(req);
    return ERROR_CODE_SUCCESS;
}

qint32 cDataTicketManager::handleListSeasonTicketsResponse(MessageProtocol* msg)
{
    if (!this->m_initialized)
        return ERROR_CODE_NOT_INITIALIZED;

    QByteArray  data(msg->getPointerToData());
    QJsonObject rootObj = QJsonDocument::fromJson(data).object();

    qint32     result      = rootObj.value("ack").toInt(ERROR_CODE_NOT_FOUND);
    qint32     updateIndex = rootObj.value("index").toInt(UpdateAll);
    qint64     timestamp   = (qint64)rootObj.value("timestamp").toDouble(0);
    QJsonArray arrTickets  = rootObj.value("tickets").toArray();

    this->m_mutex.lock();

    if (updateIndex == UpdateIndex::UpdateAll) {
        for (int i = 0; i < this->m_lTickets.size(); i++)
            delete this->m_lTickets[i];
        this->m_lTickets.clear();
    }
    this->m_stLastLocalUpdateTimeStamp = QDateTime::currentMSecsSinceEpoch();

    this->m_mutex.unlock();

    for (int i = 0; i < arrTickets.count(); i++) {
        QJsonObject       ticketObj = arrTickets.at(i).toObject();
        SeasonTicketItem* pTicket   = new SeasonTicketItem();

        pTicket->setDiscount(ticketObj.value("discount").toInt());
        pTicket->setIndex(ticketObj.value("index").toInt());
        pTicket->setUserIndex(ticketObj.value("userIndex").toInt());
        pTicket->setName(ticketObj.value("name").toString());
        pTicket->setPlace(ticketObj.value("place").toString());
        pTicket->checkTicketOwn(g_ConUserSettings.getUserIndex());

        QQmlEngine::setObjectOwnership(pTicket, QQmlEngine::CppOwnership);
        this->addNewSeasonTicket(pTicket, updateIndex);
    }

    QMutexLocker lock(&this->m_mutex);

    if (timestamp == this->m_stLastServerUpdateTimeStamp && arrTickets.count() == 0 && updateIndex != UpdateAll) {
        if (!g_StaGlobalSettings.getSaveInfosOnApp())
            return result;

        g_StaSettingsManager.setInt64Value(SEASONTICKET_GROUP, LOCAL_TICKET_UDPATE, this->m_stLastLocalUpdateTimeStamp);
        return result;
    }

    this->m_stLastServerUpdateTimeStamp = timestamp;

    if (!g_StaGlobalSettings.getSaveInfosOnApp())
        return result;

    g_StaSettingsManager.setInt64Value(SEASONTICKET_GROUP, LOCAL_TICKET_UDPATE, this->m_stLastLocalUpdateTimeStamp);
    g_StaSettingsManager.setInt64Value(SEASONTICKET_GROUP, SERVER_TICKET_UDPATE, this->m_stLastServerUpdateTimeStamp);

    for (int i = 0; i < this->m_lTickets.size(); i++) {
        g_StaSettingsManager.setValue(SEASONTICKET_GROUP, TICKET_NAME, i, this->m_lTickets[i]->name());
        g_StaSettingsManager.setValue(SEASONTICKET_GROUP, TICKET_PLACE, i, this->m_lTickets[i]->place());
        g_StaSettingsManager.setInt64Value(SEASONTICKET_GROUP, TICKET_DISCOUNT, i, this->m_lTickets[i]->discount());
        g_StaSettingsManager.setInt64Value(SEASONTICKET_GROUP, TICKET_USER_INDEX, i, this->m_lTickets[i]->userIndex());
        g_StaSettingsManager.setInt64Value(SEASONTICKET_GROUP, ITEM_INDEX, i, this->m_lTickets[i]->index());
    }

    return result;
}


qint32 cDataTicketManager::startAddSeasonTicket(const qint32 index, const QString name, const QString place, const qint32 discount)
{
    if (!this->m_initialized)
        return ERROR_CODE_NOT_INITIALIZED;

    QJsonObject rootObj;
    rootObj.insert("index", index);
    rootObj.insert("name", name);
    rootObj.insert("place", place);
    rootObj.insert("discount", discount);

    TcpDataConRequest* req = new TcpDataConRequest(OP_CODE_CMD_REQ::REQ_ADD_TICKET);
    req->m_lData           = QJsonDocument(rootObj).toJson(QJsonDocument::Compact);

    g_ConManager.sendNewRequest(req);
    return ERROR_CODE_SUCCESS;
}

qint32 cDataTicketManager::handleAddSeasonTicketResponse(MessageProtocol* msg)
{
    if (!this->m_initialized)
        return ERROR_CODE_NOT_INITIALIZED;

    return msg->getIntData();
}

qint32 cDataTicketManager::startRemoveSeasonTicket(const qint32 index)
{
    if (!this->m_initialized)
        return ERROR_CODE_NOT_INITIALIZED;

    QJsonObject rootObj;
    rootObj.insert("index", index);

    TcpDataConRequest* req = new TcpDataConRequest(OP_CODE_CMD_REQ::REQ_REMOVE_TICKET);
    req->m_lData           = QJsonDocument(rootObj).toJson(QJsonDocument::Compact);

    g_ConManager.sendNewRequest(req);
    return ERROR_CODE_SUCCESS;
}

qint32 cDataTicketManager::handleRemoveSeasonTicketResponse(MessageProtocol* msg)
{
    if (!this->m_initialized)
        return ERROR_CODE_NOT_INITIALIZED;

    QMutexLocker lock(&this->m_mutex);

    this->m_stLastServerUpdateTimeStamp = 0;

    return msg->getIntData();
}

qint32 cDataTicketManager::startListAvailableTickets(const qint32 gameIndex)
{
    if (!this->m_initialized)
        return ERROR_CODE_NOT_INITIALIZED;

    if (this->m_stLastServerUpdateTimeStamp == 0)
        return this->startListSeasonTickets();

    QMutexLocker lock(&this->m_mutex);

    QJsonObject rootObj;
    rootObj.insert("index", gameIndex);
    rootObj.insert("timestamp", this->m_stLastServerUpdateTimeStamp);

    TcpDataConRequest* req = new TcpDataConRequest(OP_CODE_CMD_REQ::REQ_GET_AVAILABLE_TICKETS);
    req->m_lData           = QJsonDocument(rootObj).toJson(QJsonDocument::Compact);

    g_ConManager.sendNewRequest(req);
    return ERROR_CODE_SUCCESS;
}

qint32 cDataTicketManager::handleListAvailableSeasonTicketResponse(MessageProtocol* msg)
{
    if (!this->m_initialized)
        return ERROR_CODE_NOT_INITIALIZED;

    QByteArray  data(msg->getPointerToData());
    QJsonObject rootObj = QJsonDocument::fromJson(data).object();

    qint32 result = rootObj.value("ack").toInt(ERROR_CODE_NOT_FOUND);
    if (result != ERROR_CODE_SUCCESS)
        return result;

    for (int i = 0; i < this->getSeasonTicketLength(); i++) {
        SeasonTicketItem* pItem = this->getSeasonTicketFromArrayIndex(i);
        if (pItem != NULL)
            pItem->setTicketState(TICKET_STATE_BLOCKED);
    }

    QJsonArray freeArr   = rootObj.value("free").toArray();
    QJsonArray resArr    = rootObj.value("reserved").toArray();
    qint32     gameIndex = rootObj.value("game").toInt();

    for (int i = 0; i < freeArr.count(); i++) {
        qint32            ticketIndex = freeArr.at(i).toInt();
        SeasonTicketItem* pItem       = this->getSeasonTicket(ticketIndex);
        if (pItem != NULL)
            pItem->setTicketState(TICKET_STATE_FREE);
        else {
            qWarning().noquote() << QString("Ticket with number %1 is missing for availableTicket Free").arg(ticketIndex);
            result = ERROR_CODE_MISSING_TICKET;
        }
    }
    for (int i = 0; i < resArr.count(); i++) {
        qint32            ticketIndex = resArr.at(i).toObject().value("index").toInt(0);
        SeasonTicketItem* pItem       = this->getSeasonTicket(ticketIndex);
        if (pItem != NULL)
            pItem->setTicketState(TICKET_STATE_RESERVED);
        else {
            qWarning().noquote() << QString("Ticket with number %1 is missing for availableTicket Reserved").arg(ticketIndex);
            result = ERROR_CODE_MISSING_TICKET;
            continue;
        }
        QString name = resArr.at(i).toObject().value("name").toString();
        pItem->setReserveName(name);
    }

    GamePlay* pGame = g_DataGamesManager.getGamePlay(gameIndex);
    if (pGame != NULL) {
        pGame->setFreeTickets(freeArr.count());
        pGame->setReservedTickets(resArr.count());
        pGame->setBlockedTickets(this->getSeasonTicketLength() - freeArr.count() - resArr.count());
    }


    return result;
}

qint32 cDataTicketManager::startChangeAvailableTicketState(const qint32 ticketIndex, const qint32 gameIndex,
                                                           const qint32 state, QString name)
{
    if (!this->m_initialized)
        return ERROR_CODE_NOT_INITIALIZED;

    QMutexLocker lock(&this->m_mutex);

    QJsonObject rootObj;
    rootObj.insert("ticketIndex", ticketIndex);
    rootObj.insert("gameIndex", gameIndex);
    rootObj.insert("state", state);
    rootObj.insert("name", name);

    TcpDataConRequest* req = new TcpDataConRequest(OP_CODE_CMD_REQ::REQ_STATE_CHANGE_SEASON_TICKET);
    req->m_lData           = QJsonDocument(rootObj).toJson(QJsonDocument::Compact);

    g_ConManager.sendNewRequest(req);
    return ERROR_CODE_SUCCESS;
}

qint32 cDataTicketManager::handleChangeAvailableTicketResponse(MessageProtocol* msg)
{
    if (!this->m_initialized)
        return ERROR_CODE_NOT_INITIALIZED;

    QByteArray  data(msg->getPointerToData());
    QJsonObject rootObj = QJsonDocument::fromJson(data).object();

    qint32 result = rootObj.value("ack").toInt(ERROR_CODE_NOT_FOUND);

    return result;
}
