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

#include <QtCore/QtEndian>
#include <QtQml/QQmlEngine>

#include "../Common/General/config.h"
#include "../Common/General/globalfunctions.h"
#include "../Data/gameplay.h"
#include "datahandling.h"

DataHandling::DataHandling(GlobalData* pData)
{
    this->m_pGlobalData = pData;
}

qint32 DataHandling::getHandleLoginResponse(MessageProtocol* msg)
{
    qint32 result;
    if (msg->getDataLength() != 4)
        return ERROR_CODE_WRONG_SIZE;
    const char* pData = msg->getPointerToData();

    result = qFromLittleEndian(*((qint32*)pData));

    return result;
}

qint32 DataHandling::getHandleVersionResponse(MessageProtocol* msg, QString* version)
{
    if (msg->getDataLength() <= 4)
        return ERROR_CODE_WRONG_SIZE;

    const char* pData    = msg->getPointerToData();
    qint32      result   = qFromLittleEndian(*((qint32*)pData));
    quint32     uVersion = qFromLittleEndian(*((quint32*)(pData + 4)));

    quint16 size = qFromLittleEndian(*((quint16*)(pData + 8)));
    if (size > msg->getDataLength())
        return ERROR_CODE_WRONG_SIZE;

    QString remVersion(QByteArray(pData + 10, size));

    if (uVersion > STAM_ORGA_VERSION_I) {
        version->append(QString("Deine Version: %2\nAktuelle Version: %1").arg(remVersion, STAM_ORGA_VERSION_S));
        return ERROR_CODE_NEW_VERSION;
    }
    version->append(remVersion);
    return result;
}

qint32 DataHandling::getHandleUserPropsResponse(MessageProtocol* msg, QString* props)
{
    if (msg->getDataLength() < 12)
        return ERROR_CODE_WRONG_SIZE;

    const char* pData  = msg->getPointerToData();
    qint32      rValue = qFromLittleEndian(*((qint32*)pData));
    qint32      index  = qFromLittleEndian(*((qint32*)(pData + 8)));
    this->m_pGlobalData->setUserIndex(index);

    SeasonTicketItem* seasonTicket;
    int               i = 0;
    while ((seasonTicket = this->m_pGlobalData->getSeasonTicketFromArrayIndex(i++)) != NULL)
        seasonTicket->checkTicketOwn(index);

    *props = QString::number(qFromLittleEndian(*((quint32*)(pData + 8))));

    quint16 readableNameSize = qFromLittleEndian(*((quint16*)(pData + 12)));
    QString readableName(QByteArray(pData + 14, readableNameSize));

    this->m_pGlobalData->setReadableName(readableName);

    return rValue;
}

#define GAMES_OFFSET (1 + 1 + 8 + 4)

qint32 DataHandling::getHandleGamesListResponse(MessageProtocol* msg)
{
    if (msg->getDataLength() < 8)
        return ERROR_CODE_WRONG_SIZE;

    const char* pData  = msg->getPointerToData();
    qint32      rValue = qFromLittleEndian(*((qint32*)pData));

    if (rValue != ERROR_CODE_SUCCESS)
        return rValue;

    quint32 totalSize = msg->getDataLength();
    quint32 offset    = 4;
    quint16 version   = qFromLittleEndian(*(qint16*)(pData + offset));
    if (version > 0x3) {
        qWarning().noquote() << QString("Unknown game version %1").arg(version);
        return -1;
    }
    offset += 2;

    quint16 totalPacks = qFromLittleEndian(*(quint16*)(pData + offset));
    offset += 2;

    this->m_pGlobalData->startUpdateGamesPlay();
    while (offset < totalSize && totalPacks > 0) {
        GamePlay* play = new GamePlay();
        quint16   size = qFromLittleEndian(*(qint16*)(pData + offset));
        offset += 2;

        if (size <= 8) {
            qWarning().noquote() << QString("Size is to small %1").arg(size);
            delete play;
            break;
        }

        play->setSeasonIndex(*(qint8*)(pData + offset));
        offset += 1;
        play->setCompetition(CompetitionIndex(*(qint8*)(pData + offset)));
        offset += 1;

        /* On Android there are problems reading from qint64 pointers???? SIGBUS*/
        //        play->setTimeStamp(qFromLittleEndian(*(qint64 *)(pData + offset)));
        quint32 tmp       = qFromLittleEndian(*(qint32*)(pData + offset));
        qint64  timeStamp = qint64(tmp);
        tmp               = qFromLittleEndian(*(quint32*)(pData + offset + 4));
        timeStamp |= qint64(tmp) << 32;

        play->setTimeStamp(timeStamp);
        offset += 8;

        play->setIndex(qFromLittleEndian(*(quint32*)(pData + offset)));
        offset += 4;

        play->setFreeTickets(qFromLittleEndian(*(quint16*)(pData + offset)));
        offset += 2;
        play->setBlockedTickets(qFromLittleEndian(*(quint16*)(pData + offset)));
        offset += 2;
        play->setReservedTickets(qFromLittleEndian(*(quint16*)(pData + offset)));
        offset += 2;

        QString playString(QByteArray(pData + offset, size - GAMES_OFFSET));
        offset += (size - GAMES_OFFSET);
        QStringList lplayString = playString.split(";");

        if (lplayString.size() > 0)
            play->setHome(lplayString.value(0));
        if (lplayString.size() > 1)
            play->setAway(lplayString.value(1));
        if (lplayString.size() > 2)
            play->setScore(lplayString.value(2));

        QQmlEngine::setObjectOwnership(play, QQmlEngine::CppOwnership);
        this->m_pGlobalData->addNewGamePlay(play);
        totalPacks--;
    }

    this->m_pGlobalData->saveActualGamesList();

    return rValue;
}

#define TICKET_OFFSET (1 + 4 + 4)

qint32 DataHandling::getHandleSeasonTicketListResponse(MessageProtocol* msg)
{
    if (msg->getDataLength() < 8)
        return ERROR_CODE_WRONG_SIZE;

    quint32     userIndex = this->m_pGlobalData->userIndex();
    const char* pData     = msg->getPointerToData();
    qint32      rValue    = qFromLittleEndian(*((qint32*)pData));

    if (rValue != ERROR_CODE_SUCCESS)
        return rValue;

    quint32 totalSize = msg->getDataLength();
    quint32 offset    = 4;
    quint16 version   = qFromLittleEndian(*(qint16*)(pData + offset));
    if (version != 0x1) {
        qWarning().noquote() << QString("Unknown season ticket version %1").arg(version);
        return -1;
    }
    offset += 2;

    quint16 totalPacks = qFromLittleEndian(*(quint16*)(pData + offset));
    offset += 2;

    this->m_pGlobalData->startUpdateSeasonTickets();
    while (offset < totalSize && totalPacks > 0) {
        SeasonTicketItem* sTicket = new SeasonTicketItem();
        quint16           size    = qFromLittleEndian(*(qint16*)(pData + offset));
        offset += 2;

        if (size <= 8) {
            qWarning().noquote() << QString("Size is to small %1").arg(size);
            delete sTicket;
            break;
        }

        sTicket->setDiscount(*(quint8*)(pData + offset));
        offset += 1;
        sTicket->setIndex(qFromLittleEndian(*(quint32*)(pData + offset)));
        offset += 4;
        sTicket->setUserIndex(qFromLittleEndian(*(quint32*)(pData + offset)));
        offset += 4;
        sTicket->checkTicketOwn(userIndex);

        QString ticketString(QByteArray(pData + offset, size - TICKET_OFFSET));
        offset += (size - TICKET_OFFSET);
        QStringList lsticketString = ticketString.split(";");

        if (lsticketString.size() > 0)
            sTicket->setName(lsticketString.value(0));
        if (lsticketString.size() > 1)
            sTicket->setPlace(lsticketString.value(1));

        QQmlEngine::setObjectOwnership(sTicket, QQmlEngine::CppOwnership);
        this->m_pGlobalData->addNewSeasonTicket(sTicket);
        totalPacks--;
    }

    this->m_pGlobalData->saveCurrentSeasonTickets();

    return rValue;
}

/*  answer
 * 0                Header          12
 * 12   quint32     result          4
 * 16   quint32     version         4
 * 20   quint16     freeCount       2
 * 22   quint16     reserveCount    2
 * 24   quint32     fticketIndex1   4
 * 28   quint32     fticketIndex2   4
 *
 * X    quint32     rticketIndex1   4
 * X+4  quint32     rTicketName     Y
 * x+4+Yquint8      0x0             1
 */

#define AVAILABLE_HEAD_OFFSET 2
qint32 DataHandling::getHandleAvailableTicketListResponse(MessageProtocol* msg, const quint32 gameIndex)
{
    if (msg->getDataLength() < 4)
        return ERROR_CODE_WRONG_SIZE;

    const char* pData = msg->getPointerToData();
    quint32     version;
    qint32      result;
    memcpy(&result, pData, sizeof(quint32));
    result = qFromLittleEndian(result);
    if (result != ERROR_CODE_SUCCESS)
        return result;

    quint32 offset = 4;
    if (msg->getDataLength() >= 12)
        memcpy(&version, pData + offset, sizeof(quint32));
    else
        return ERROR_CODE_WRONG_SIZE;
    version = qFromLittleEndian(version);
    offset += sizeof(quint32);

    for (uint i = 0; i < this->m_pGlobalData->getSeasonTicketLength(); i++) {
        SeasonTicketItem* item = this->m_pGlobalData->getSeasonTicketFromArrayIndex(i);
        if (item != NULL)
            item->setTicketState(TICKET_STATE_BLOCKED);
    }

    qint16 countOfFreeTickets = qFromLittleEndian(*((qint16*)(pData + offset)));
    offset += sizeof(qint16);
    qint16 countOfReservedTickets = qFromLittleEndian(*((qint16*)(pData + offset)));
    offset += sizeof(qint16);
    for (int i = 0; i < countOfFreeTickets; i++) {
        if (offset + AVAILABLE_HEAD_OFFSET > msg->getDataLength()) {
            qWarning() << "Error in message for get available ticket list";
            return result;
        }
        quint32           ticketIndex = qFromLittleEndian(*((quint32*)(pData + offset)));
        SeasonTicketItem* item        = this->m_pGlobalData->getSeasonTicket(ticketIndex);
        if (item != NULL)
            item->setTicketState(TICKET_STATE_FREE);
        else
            result = ERROR_CODE_MISSING_TICKET;

        offset += 4;
    }
    for (int i = 0; i < countOfReservedTickets; i++) {
        if (offset + AVAILABLE_HEAD_OFFSET > msg->getDataLength()) {
            qWarning() << "Error in message for get available ticket list";
            return result;
        }
        quint32           ticketIndex = qFromLittleEndian(*((quint32*)(pData + offset)));
        SeasonTicketItem* item        = this->m_pGlobalData->getSeasonTicket(ticketIndex);
        if (item != NULL)
            item->setTicketState(TICKET_STATE_RESERVED);
        else
            result = ERROR_CODE_MISSING_TICKET;
        offset += 4;
        QString name(pData + offset);
        item->setReserveName(name);
        offset += name.size() + 1;
    }

    GamePlay* game = this->m_pGlobalData->getGamePlay(gameIndex);
    if (game != NULL) {
        game->setFreeTickets(countOfFreeTickets);
        game->setReservedTickets(countOfReservedTickets);
        game->setBlockedTickets(this->m_pGlobalData->getSeasonTicketLength() - countOfFreeTickets - countOfReservedTickets);
    }

    return result;
}

/*  answer
 * 0                Header          12
 * 12   quint32     result          4
 * 16   quint32     version         4
 * 20   quint32     gameIndex       4
 * 24   QString     when
 * X    Qstring     where
 * Y    QString     info
 */
qint32 DataHandling::getHandleLoadMeetingInfo(MessageProtocol* msg)
{
    if (msg->getDataLength() < 4)
        return ERROR_CODE_WRONG_SIZE;

    const char* pData = msg->getPointerToData();
    quint32     gameIndex, version;
    qint32      result;
    memcpy(&result, pData, sizeof(quint32));
    if (msg->getDataLength() >= 12) {
        memcpy(&version, pData + 4, sizeof(quint32));
        memcpy(&gameIndex, pData + 8, sizeof(quint32));
    }
    result = qFromLittleEndian(result);
    if (result != ERROR_CODE_SUCCESS)
        return result;

    version   = qFromLittleEndian(version);
    gameIndex = qFromLittleEndian(gameIndex);

    quint32 offset = 12;
    QString when(QByteArray(pData + offset));
    offset += when.toUtf8().size() + 1;
    QString where(QByteArray(pData + offset));
    offset += where.toUtf8().size() + 1;
    QString info(QByteArray(pData + offset));
    offset += info.toUtf8().size() + 1;

    MeetingInfo* pInfo = this->m_pGlobalData->getMeetingInfo();
    pInfo->setWhen(when);
    pInfo->setWhere(where);
    pInfo->setInfo(info);

    quint32 size = msg->getDataLength();
    quint32 index, value, userID;
    pInfo->clearAcceptInfoList();
    quint32 counter = 0;
    while (offset + 9 < size) {
        AcceptMeetingInfo* ami = new AcceptMeetingInfo();
        memcpy(&index, pData + offset, sizeof(quint32));
        offset += sizeof(quint32);
        memcpy(&value, pData + offset, sizeof(quint32));
        offset += sizeof(quint32);
        memcpy(&userID, pData + offset, sizeof(quint32));
        offset += sizeof(quint32);

        ami->setIndex(qFromLittleEndian(index));
        ami->setValue(qFromLittleEndian(value));
        ami->setUserIndex(qFromLittleEndian(userID));
        ami->setName(QString(QByteArray(pData + offset)));
        offset += ami->name().toUtf8().size() + 1;

        QQmlEngine::setObjectOwnership(ami, QQmlEngine::CppOwnership);
        pInfo->addNewAcceptInfo(ami);
        counter++;
    }

    return result;
}
