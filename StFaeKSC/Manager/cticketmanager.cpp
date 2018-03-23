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
#include "../General/globaldata.h"
#include "cticketmanager.h"

extern GlobalData* g_GlobalData;

cTicketManager g_TicketManager;

cTicketManager::cTicketManager(QObject* parent)
    : cGenDisposer(parent)
{
}


qint32 cTicketManager::initialize()
{
    this->m_initialized = true;

    return ERROR_CODE_SUCCESS;
}

MessageProtocol* cTicketManager::getSeasonTicketList(UserConData* pUserCon, MessageProtocol* request)
{
    if (!this->m_initialized)
        return NULL;

    QByteArray  data    = QByteArray(request->getPointerToData());
    QJsonObject rootObj = QJsonDocument::fromJson(data).object();

    qint32 updateIndex              = rootObj.value("index").toInt(UpdateIndex::UpdateAll);
    qint64 lastUpdateTicketsFromApp = (qint64)rootObj.value("timestamp").toDouble(0);

    if (lastUpdateTicketsFromApp == 0)
        updateIndex = UpdateIndex::UpdateAll;

    QJsonObject rootAns;
    rootAns.insert("ack", ERROR_CODE_SUCCESS);
    rootAns.insert("index", updateIndex);

    quint16 numbOfTickets = g_GlobalData->m_SeasonTicket.getNumberOfInternalList();

    QJsonArray arrTickets;
    for (quint32 i = 0; i < numbOfTickets; i++) {
        TicketInfo* pTicket = (TicketInfo*)(g_GlobalData->m_SeasonTicket.getRequestConfigItemFromListIndex(i));
        if (pTicket == NULL)
            continue;
        QJsonObject ticketObj;

        if (updateIndex == UpdateIndex::UpdateDiff && pTicket->m_timestamp <= lastUpdateTicketsFromApp)
            continue; // Skip ticket because user already has all info

        ticketObj.insert("name", pTicket->m_itemName);
        ticketObj.insert("place", pTicket->m_place);
        ticketObj.insert("discount", pTicket->m_discount);
        ticketObj.insert("index", pTicket->m_index);
        ticketObj.insert("userIndex", pTicket->m_userIndex);

        arrTickets.append(ticketObj);
    }

    rootAns.insert("timestamp", g_GlobalData->m_SeasonTicket.getLastUpdateTime());
    rootAns.insert("tickets", arrTickets);

    QByteArray answer = QJsonDocument(rootAns).toJson(QJsonDocument::Compact);

    qInfo().noquote() << QString("User %1 request Ticket List with %2 entries").arg(pUserCon->m_userName).arg(numbOfTickets);

    return new MessageProtocol(OP_CODE_CMD_RES::ACK_GET_TICKETS_LIST, answer);
}

MessageProtocol* cTicketManager::getSeasonTicketAddRequest(UserConData* pUserCon, MessageProtocol* request)
{
    if (!this->m_initialized)
        return NULL;

    QByteArray  data    = QByteArray(request->getPointerToData());
    QJsonObject rootObj = QJsonDocument::fromJson(data).object();

    qint32  index    = rootObj.value("index").toInt(-1);
    QString name     = rootObj.value("name").toString();
    QString place    = rootObj.value("place").toString();
    qint32  discount = rootObj.value("discount").toInt();

    qint32 rCode;
    if (index == 0)
        rCode = g_GlobalData->m_SeasonTicket.addNewSeasonTicket(pUserCon->m_userName, pUserCon->m_userID, name, discount);
    else
        rCode = g_GlobalData->m_SeasonTicket.changeSeasonTicketInfos(index, discount, name, place);

    if (rCode > ERROR_CODE_NO_ERROR) {
        qInfo().noquote() << QString("User %1 added/changed SeasonTicket %2")
                                 .arg(pUserCon->m_userName)
                                 .arg(name);
        return new MessageProtocol(OP_CODE_CMD_RES::ACK_ADD_TICKET, ERROR_CODE_SUCCESS);
    }
    return new MessageProtocol(OP_CODE_CMD_RES::ACK_ADD_TICKET, rCode);
}

MessageProtocol* cTicketManager::getSeasonTicketRemoveRequest(UserConData* pUserCon, MessageProtocol* request)
{
    if (!this->m_initialized)
        return NULL;

    QByteArray  data    = QByteArray(request->getPointerToData());
    QJsonObject rootObj = QJsonDocument::fromJson(data).object();

    qint32 index = rootObj.value("index").toInt(0);
    qint32 rCode;
    if ((rCode = g_GlobalData->m_SeasonTicket.removeItem(index)) == ERROR_CODE_SUCCESS) {
        qInfo().noquote() << QString("User %1 removed SeasonTicket %2")
                                 .arg(pUserCon->m_userName)
                                 .arg(index);
        return new MessageProtocol(OP_CODE_CMD_RES::ACK_REMOVE_TICKET, ERROR_CODE_SUCCESS);
    }
    return new MessageProtocol(OP_CODE_CMD_RES::ACK_REMOVE_TICKET, rCode);
}
