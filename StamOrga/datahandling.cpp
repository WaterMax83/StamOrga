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
#include <QtCore/QtEndian>
#include <QtQml/QQmlEngine>

#include "../Common/General/config.h"
#include "../Common/General/globalfunctions.h"
#include "../Data/appuserevents.h"
#include "../Data/gameplay.h"
#include "../Data/statistic.h"
#include "datahandling.h"


extern AppUserEvents* g_AppUserEvents;
extern Statistic*     g_Statistics;

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

    qInfo().noquote() << QString("Version from server %1:0x%2").arg(remVersion, QString::number(uVersion, 16));

    if ((uVersion & 0xFFFFFF00) > (STAM_ORGA_VERSION_I & 0xFFFFFF00)) {
        version->append(QString("Deine Version: %2<br>Aktuelle Version: %1<br><br>").arg(remVersion, STAM_ORGA_VERSION_S));
        version->append(QString(STAM_ORGA_VERSION_LINK_WITH_TEXT).arg(remVersion.toLower(), remVersion));
        this->m_pGlobalData->setUpdateLink(QString(STAM_ORGA_VERSION_LINK).arg(remVersion.toLower()));
        //#ifdef Q_OS_WIN
        //        version->append(QString("<a href=\"https://github.com/WaterMax83/StamOrga/releases/download/%1/StamOrga.Winx64.%1.exe\">Lade %2</a>\n")
        //                            .arg(remVersion.toLower(), remVersion));
        //#endif
        //#ifdef Q_OS_ANDROID
        //        version->append(QString("<a href=\"https://github.com/WaterMax83/StamOrga/releases/download/%1/StamOrga.Android.%1.apk\">Lade %2</a>\n")
        //                            .arg(remVersion.toLower(), remVersion));
        //#endif
        return ERROR_CODE_NEW_VERSION;
    }
    version->append(remVersion);
    return result;
}

qint32 DataHandling::getHandleUserPropsResponse(MessageProtocol* msg)
{
    QByteArray  data(msg->getPointerToData());
    QJsonObject rootObj = QJsonDocument::fromJson(data).object();

    qint32  rValue       = rootObj.value("rvalue").toInt(ERROR_CODE_MISSING_PARAMETER);
    qint32  index        = rootObj.value("index").toInt(-1);
    qint32  properties   = rootObj.value("property").toInt(0);
    QString readableName = rootObj.value("readableName").toString();

    this->m_pGlobalData->setUserIndex(index);

    SeasonTicketItem* seasonTicket;
    int               i = 0;
    while ((seasonTicket = this->m_pGlobalData->getSeasonTicketFromArrayIndex(i++)) != NULL)
        seasonTicket->checkTicketOwn(index);

    if (rValue == ERROR_CODE_SUCCESS) {
        if (this->m_pGlobalData->getUserProperties() != (quint32)properties) {
            this->m_pGlobalData->SetUserProperties(properties);
            qInfo().noquote() << QString("Setting user properties to 0x%1").arg(QString::number(properties, 16));
        }
    } else
        this->m_pGlobalData->SetUserProperties(0x0);

    this->m_pGlobalData->setReadableName(readableName);

    if (rootObj.contains("tickets")) {
        GameUserData* pGameUserData = this->m_pGlobalData->getGameUserDataHandler();
        pGameUserData->clearTicketGameList();

        QJsonArray arrTickets = rootObj.value("tickets").toArray();
        for (int i = 0; i < arrTickets.count(); i++) {
            QJsonObject ticket    = arrTickets.at(i).toObject();
            qint32      gameIndex = ticket.value("gameIndex").toInt(-1);
            if (ticket.value("type").toString("") == "reserved")
                pGameUserData->setTicketGameIndex(gameIndex, TICKET_STATE_RESERVED);
            else if (ticket.value("type").toString("") == "free")
                pGameUserData->setTicketGameIndex(gameIndex, TICKET_STATE_FREE);
        }
    }

    return rValue;
}

qint32 DataHandling::getHandleUserEventsResponse(MessageProtocol* msg)
{
    if (msg->getDataLength() < 4)
        return ERROR_CODE_WRONG_SIZE;

    const char* pData = msg->getPointerToData();
    qint32      rValue;
    memcpy(&rValue, pData, sizeof(qint32));
    rValue = qFromLittleEndian(rValue);

    if (msg->getDataLength() > 4) {
        g_AppUserEvents->resetCurrentEvents();

        QByteArray  jsonByteArray(pData + sizeof(qint32));
        QJsonObject jsRoot = QJsonDocument::fromJson(jsonByteArray).object();
        if (!jsRoot.contains("events") || !jsRoot.value("events").isArray())
            return ERROR_CODE_WRONG_PARAMETER;

        QJsonArray jsArr = jsRoot.value("events").toArray();
        for (int i = 0; i < jsArr.size(); i++) {
            QJsonObject jsObj = jsArr.at(i).toObject();
            g_AppUserEvents->addNewUserEvents(jsObj);
        }
    }
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

    qint16 updateIndex;
    memcpy(&updateIndex, pData + offset, sizeof(qint16));
    updateIndex = qFromLittleEndian(updateIndex);
    offset += sizeof(qint16);

    quint16 size;
    quint8  tmp;
    qint64  timeStamp;
    this->m_pGlobalData->startUpdateGamesPlay(updateIndex);
    while (offset + GAMES_OFFSET < totalSize) {
        GamePlay* play = new GamePlay();
        memcpy(&size, pData + offset, sizeof(quint16));
        size = qFromLittleEndian(size);
        offset += sizeof(quint16);

        if (size <= GAMES_OFFSET) {
            qWarning().noquote() << QString("Size is to small %1").arg(size);
            delete play;
            break;
        }
        memcpy(&tmp, pData + offset, sizeof(quint8));
        play->setSeasonIndex(qFromLittleEndian(tmp));
        offset += sizeof(quint8);
        memcpy(&tmp, pData + offset, sizeof(quint8));
        tmp = qFromLittleEndian(tmp);
        play->setCompetition(CompetitionIndex(tmp & 0x7F));
        play->setTimeFixed((tmp & 0x80) > 0 ? true : false);
        offset += 1;

        /* On Android there are problems reading from qint64 pointers???? SIGBUS*/
        //        play->setTimeStamp(qFromLittleEndian(*(qint64 *)(pData + offset)));
        quint32 tmp32 = qFromLittleEndian(*(qint32*)(pData + offset));
        timeStamp     = qint64(tmp32);
        tmp32         = qFromLittleEndian(*(quint32*)(pData + offset + 4));
        timeStamp |= qint64(tmp32) << 32;

        play->setTimeStamp(timeStamp);
        offset += 8;

        play->setIndex(qFromLittleEndian(*(quint32*)(pData + offset)));
        offset += 4;

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
        this->m_pGlobalData->addNewGamePlay(play, updateIndex);
    }
    memcpy(&timeStamp, pData + offset, sizeof(qint64));
    offset += sizeof(qint64);

    this->m_pGlobalData->saveCurrentGamesList(qFromLittleEndian(timeStamp));

    return rValue;
}

qint32 DataHandling::getHandleGamesInfoListResponse(MessageProtocol* msg)
{
    if (msg->getDataLength() < 4)
        return ERROR_CODE_WRONG_SIZE;

    const char* pData = msg->getPointerToData();
    qint32      rValue;
    memcpy(&rValue, pData, sizeof(qint32));
    rValue = qFromLittleEndian(rValue);

    if (rValue != ERROR_CODE_SUCCESS)
        return rValue;
    /* Status is not yet used */
    quint32 offset = 8;
    quint16 gameSize, readInfo;
    memcpy(&gameSize, pData + offset, sizeof(quint16));
    memcpy(&readInfo, pData + offset + 2, sizeof(quint16));
    offset += 2 * sizeof(quint16);
    gameSize = qFromLittleEndian(gameSize);
    readInfo = qFromLittleEndian(readInfo);

    if ((readInfo & 0x1) == 0x0) {
        quint32 numbOfGames = this->m_pGlobalData->getGamePlayLength();
        for (quint32 i = 0; i < numbOfGames; i++) {
            GamePlay* play = this->m_pGlobalData->getGamePlayFromArrayIndex(i);
            if (play == NULL)
                continue;
            play->setFreeTickets(0);
            play->setBlockedTickets(0);
            play->setReservedTickets(0);
            play->setAcceptedMeetingCount(0);
            play->setInterestedMeetingCount(0);
            play->setDeclinedMeetingCount(0);
            play->setMeetingInfo(0);
            play->setAcceptedTripCount(0);
            play->setInterestedTripCount(0);
            play->setDeclinedTripCount(0);
            play->setTripInfo(0);
        }
    }

    quint32 totalSize = msg->getDataLength();
    quint32 gameIndex;
    quint16 freeTicks, reservTicks, blockTicks;
    quint16 acceptMeet, interestMeet, declineMeet, meetInfo;
    while (offset + gameSize <= totalSize) {

        memcpy(&gameIndex, pData + offset, sizeof(quint32));
        gameIndex = qFromLittleEndian(gameIndex);

        GamePlay* play = this->m_pGlobalData->getGamePlay(gameIndex);
        if (play == NULL) {
            offset += gameSize;
            continue;
        }
        offset += sizeof(quint32);

        memcpy(&freeTicks, pData + offset, sizeof(quint16));
        freeTicks = qFromLittleEndian(freeTicks);
        offset += sizeof(quint16);

        memcpy(&blockTicks, pData + offset, sizeof(quint16));
        blockTicks = qFromLittleEndian(blockTicks);
        offset += sizeof(quint16);

        memcpy(&reservTicks, pData + offset, sizeof(quint16));
        reservTicks = qFromLittleEndian(reservTicks);
        offset += sizeof(quint16);

        play->setFreeTickets(freeTicks);
        play->setBlockedTickets(blockTicks);
        play->setReservedTickets(reservTicks);

        memcpy(&acceptMeet, pData + offset, sizeof(quint16));
        acceptMeet = qFromLittleEndian(acceptMeet);
        offset += sizeof(quint16);

        memcpy(&interestMeet, pData + offset, sizeof(quint16));
        interestMeet = qFromLittleEndian(interestMeet);
        offset += sizeof(quint16);

        memcpy(&declineMeet, pData + offset, sizeof(quint16));
        declineMeet = qFromLittleEndian(declineMeet);
        offset += sizeof(quint16);

        play->setAcceptedMeetingCount(acceptMeet);
        play->setInterestedMeetingCount(interestMeet);
        play->setDeclinedMeetingCount(declineMeet);

        memcpy(&meetInfo, pData + offset, sizeof(quint16));
        meetInfo = qFromLittleEndian(meetInfo);
        offset += sizeof(quint16);

        play->setMeetingInfo(meetInfo);

        memcpy(&acceptMeet, pData + offset, sizeof(quint16));
        acceptMeet = qFromLittleEndian(acceptMeet);
        offset += sizeof(quint16);

        memcpy(&interestMeet, pData + offset, sizeof(quint16));
        interestMeet = qFromLittleEndian(interestMeet);
        offset += sizeof(quint16);

        memcpy(&declineMeet, pData + offset, sizeof(quint16));
        declineMeet = qFromLittleEndian(declineMeet);
        offset += sizeof(quint16);

        play->setAcceptedTripCount(acceptMeet);
        play->setInterestedTripCount(interestMeet);
        play->setDeclinedTripCount(declineMeet);

        memcpy(&meetInfo, pData + offset, sizeof(quint16));
        meetInfo = qFromLittleEndian(meetInfo);
        offset += sizeof(quint16);

        play->setTripInfo(meetInfo);
    }

    return rValue;
}

#define TICKET_OFFSET (1 + 4 + 4)

qint32 DataHandling::getHandleSeasonTicketListResponse(MessageProtocol* msg)
{
    if (msg->getDataLength() < 6)
        return ERROR_CODE_WRONG_SIZE;

    quint32     userIndex = this->m_pGlobalData->userIndex();
    const char* pData     = msg->getPointerToData();
    qint32      rValue;
    memcpy(&rValue, pData, sizeof(qint32));
    rValue = qFromLittleEndian(rValue);

    if (rValue != ERROR_CODE_SUCCESS)
        return rValue;

    quint32 totalSize = msg->getDataLength();
    quint32 offset    = 4;

    quint16 updateIndex;
    memcpy(&updateIndex, pData + offset, sizeof(quint16));
    updateIndex = qFromLittleEndian(updateIndex);
    offset += 2;

    this->m_pGlobalData->startUpdateSeasonTickets(updateIndex);
    while (offset + TICKET_OFFSET < totalSize) {
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
        this->m_pGlobalData->addNewSeasonTicket(sTicket, updateIndex);
    }

    qint64 serverTimeStamp;
    memcpy(&serverTimeStamp, pData + offset, sizeof(qint64));
    serverTimeStamp = qFromLittleEndian(serverTimeStamp);

    this->m_pGlobalData->saveCurrentSeasonTickets(serverTimeStamp);

    return rValue;
}

qint32 DataHandling::getHandleChangeTicketStateResponse(MessageProtocol* msg)
{
    if (msg->getDataLength() < 12)
        return ERROR_CODE_WRONG_SIZE;

    const char* pData = msg->getPointerToData();
    qint32      result;
    memcpy(&result, pData, sizeof(quint32));
    result = qFromLittleEndian(result);

    qint64 messageID;
    memcpy(&messageID, pData + sizeof(quint32), sizeof(qint64));
    messageID = qFromLittleEndian(messageID);

    return result;
}

/*  answer
 * 0   quint32     result          4
 * 4   quint16     freeCount       2
 * 6   quint16     reserveCount    2
 * 8   quint32     fticketIndex1   4
 * 12   quint32     fticketIndex2   4
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
    qint32      result;
    memcpy(&result, pData, sizeof(quint32));
    result = qFromLittleEndian(result);
    if (result != ERROR_CODE_SUCCESS)
        return result;

    quint32 offset = 4;
    if (msg->getDataLength() <= 4)
        return ERROR_CODE_WRONG_SIZE;

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
        else {
            qWarning().noquote() << QString("Ticket with number %1 is missing for availableTicket Free").arg(ticketIndex);
            result = ERROR_CODE_MISSING_TICKET;
        }

        offset += 4;
    }
    for (int i = 0; i < countOfReservedTickets; i++) {
        if (offset + AVAILABLE_HEAD_OFFSET > msg->getDataLength()) {
            qWarning() << "Error in message for get available ticket list";
            return result;
        }
        quint32           ticketIndex = qFromLittleEndian(*((quint32*)(pData + offset)));
        SeasonTicketItem* item        = this->m_pGlobalData->getSeasonTicket(ticketIndex);
        offset += 4;
        if (item != NULL)
            item->setTicketState(TICKET_STATE_RESERVED);
        else {
            qWarning().noquote() << QString("Ticket with number %1 is missing for availableTicket Reserved").arg(ticketIndex);
            result = ERROR_CODE_MISSING_TICKET;
            continue;
        }
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

qint32 DataHandling::getHandleChangeMeetingResponse(MessageProtocol* msg)
{
    if (msg->getDataLength() < 12)
        return ERROR_CODE_WRONG_SIZE;

    const char* pData = msg->getPointerToData();
    qint32      result;
    memcpy(&result, pData, sizeof(quint32));
    result = qFromLittleEndian(result);

    qint64 messageID;
    memcpy(&messageID, pData + sizeof(quint32), sizeof(qint64));
    messageID = qFromLittleEndian(messageID);

    return result;
}

qint32 DataHandling::getHandleAcceptMeetingResponse(MessageProtocol* msg)
{
    if (msg->getDataLength() < 12)
        return ERROR_CODE_WRONG_SIZE;

    const char* pData = msg->getPointerToData();
    qint32      result;
    memcpy(&result, pData, sizeof(quint32));
    result = qFromLittleEndian(result);

    qint64 messageID;
    memcpy(&messageID, pData + sizeof(quint32), sizeof(qint64));
    messageID = qFromLittleEndian(messageID);

    return result;
}

/*  answer
 * 0   quint32     result          4
 * 4   quint32     gameIndex       4
 * 8   QString     when
 * X    Qstring     where
 * Y    QString     info
 *      quint32     acceptIndex     4
 *      quint32     acceptValue     4
 *      QString     name            Z
 */
qint32 DataHandling::getHandleLoadMeetingInfo(MessageProtocol* msg, const quint32 type)
{
    if (msg->getDataLength() < 4)
        return ERROR_CODE_WRONG_SIZE;

    const char*  pData = msg->getPointerToData();
    MeetingInfo* pInfo = this->m_pGlobalData->getMeetingInfo(type);
    quint32      gameIndex;
    qint32       result;
    memcpy(&result, pData, sizeof(quint32));
    if (msg->getDataLength() >= 8) {
        memcpy(&gameIndex, pData + 4, sizeof(quint32));
    }
    result = qFromLittleEndian(result);
    if (result != ERROR_CODE_SUCCESS) {
        pInfo->setWhen("");
        pInfo->setWhere("");
        pInfo->setInfo("");
        return result;
    }

    gameIndex = qFromLittleEndian(gameIndex);

    quint32 offset = 8;
    QString when(QByteArray(pData + offset));
    offset += when.toUtf8().size() + 1;
    QString where(QByteArray(pData + offset));
    offset += where.toUtf8().size() + 1;
    QString info(QByteArray(pData + offset));
    offset += info.toUtf8().size() + 1;

    pInfo->setWhen(when);
    pInfo->setWhere(where);
    pInfo->setInfo(info);

    bool bInfoSet = (when.isEmpty() && where.isEmpty() && info.isEmpty()) ? false : true;

    quint32 size = msg->getDataLength();
    quint32 index, value, userID;
    pInfo->clearAcceptInfoList();
    quint32 counter       = 0;
    quint32 acceptMeeting = 0, interestMeeting = 0, declineMeeting = 0;
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

        //        if (type == MEETING_TYPE_MEETING) {
        if (value == ACCEPT_STATE_ACCEPT)
            acceptMeeting++;
        else if (value == ACCEPT_STATE_MAYBE)
            interestMeeting++;
        else if (value == ACCEPT_STATE_DECLINE)
            declineMeeting++;
        //        }
    }

    GamePlay* game = this->m_pGlobalData->getGamePlay(gameIndex);
    if (game != NULL) {
        if (type == MEETING_TYPE_MEETING) {
            game->setAcceptedMeetingCount(acceptMeeting);
            game->setInterestedMeetingCount(interestMeeting);
            game->setDeclinedMeetingCount(declineMeeting);
            if (bInfoSet || acceptMeeting > 0 || interestMeeting > 0)
                game->setMeetingInfo(1);
            else
                game->setMeetingInfo(0);
        } else if (type == MEETING_TYPE_AWAYTRIP) {
            game->setAcceptedTripCount(acceptMeeting);
            game->setInterestedTripCount(interestMeeting);
            game->setDeclinedTripCount(declineMeeting);
            if (bInfoSet || acceptMeeting > 0 || interestMeeting > 0)
                game->setTripInfo(1);
            else
                game->setMeetingInfo(0);
        }
    }

    return result;
}

/* answer
 * 0    qint32      result         4
 * 4    qint32      newsIndex      4
 * 8    qint64      messageId      8
 */
qint32 DataHandling::getHandleFanclubNewsChangeResponse(MessageProtocol* msg, QString& returnData)
{
    if (msg->getDataLength() < 16)
        return ERROR_CODE_WRONG_SIZE;

    const char* pData = msg->getPointerToData();
    qint32      result;
    memcpy(&result, pData, sizeof(qint32));
    result = qFromLittleEndian(result);
    pData += sizeof(qint32);

    qint32 newsIndex;
    memcpy(&newsIndex, pData, sizeof(qint32));
    newsIndex = qFromLittleEndian(newsIndex);
    pData += sizeof(qint32);

    returnData = QString::number(newsIndex);

    qint64 messageID;
    memcpy(&messageID, pData, sizeof(qint64));
    messageID = qFromLittleEndian(messageID);

    return result;
}


/* answer
 * 0    quint32      result         4
 * 4    quint32      updateIndex    4
 * 8    qint64       updateTime     8
 * 16   quint32      index          4
 * 20   qint64       time           8
 * 24   QString      name           X
 * 24+X QString      header         Y
 */
#define FAN_NEWS_LIST_HEAD_OFFSET 4 + 8 + 1 + 1
qint32 DataHandling::getHandleFanclubNewsListResponse(MessageProtocol* msg)
{
    if (msg->getDataLength() < 16)
        return ERROR_CODE_WRONG_SIZE;

    const char* pData = msg->getPointerToData();
    qint32      result;
    quint32     updateIndex;
    memcpy(&result, pData, sizeof(qint32));
    memcpy(&updateIndex, pData + 4, sizeof(quint32));
    qint64 serverTimeStamp;
    memcpy(&serverTimeStamp, pData + 8, sizeof(qint64));

    result = qFromLittleEndian(result);
    if (result != ERROR_CODE_SUCCESS) {
        return result;
    }
    updateIndex = qFromLittleEndian(updateIndex);

    quint64 offset = 16;
    quint32 index;
    qint64  timestamp;
    this->m_pGlobalData->startUpdateNewsDataItem(updateIndex);
    while (offset + FAN_NEWS_LIST_HEAD_OFFSET < msg->getDataLength()) {

        NewsDataItem* pItem = new NewsDataItem();

        memcpy(&index, pData + offset, sizeof(quint32));
        offset += sizeof(quint32);
        pItem->setIndex(qFromLittleEndian(index));

        memcpy(&timestamp, pData + offset, sizeof(qint64));
        offset += sizeof(qint64);
        pItem->setTimeStamp(qFromLittleEndian(timestamp));

        pItem->setUser(QString(QByteArray(pData + offset)));
        offset += pItem->user().toUtf8().length() + 1;

        pItem->setHeader(QString(QByteArray(pData + offset)));
        offset += pItem->header().toUtf8().length() + 1;

        QQmlEngine::setObjectOwnership(pItem, QQmlEngine::CppOwnership);
        this->m_pGlobalData->addNewNewsDataItem(pItem, updateIndex);
    }

    this->m_pGlobalData->saveCurrentNewsDataList(qFromLittleEndian(serverTimeStamp));

    return result;
}

qint32 DataHandling::getHandleFanclubNewsItemResponse(MessageProtocol* msg)
{
    if (msg->getDataLength() < 4)
        return ERROR_CODE_WRONG_SIZE;

    const char* pData = msg->getPointerToData();
    qint32      result;
    memcpy(&result, pData, sizeof(qint32));
    result = qFromLittleEndian(result);
    if (result != ERROR_CODE_SUCCESS) {
        return result;
    }
    if (msg->getDataLength() < 22)
        return ERROR_CODE_WRONG_SIZE;

    quint32 newsIndex;
    memcpy(&newsIndex, pData + 4, sizeof(quint32));
    newsIndex = qFromLittleEndian(newsIndex);

    NewsDataItem* pItem = this->m_pGlobalData->getNewsDataItem(newsIndex);
    if (pItem == NULL)
        return ERROR_CODE_NOT_FOUND;

    qint64 timestamp;
    memcpy(&timestamp, pData + 8, sizeof(qint64));
    timestamp = qFromLittleEndian(timestamp);

    quint32 infoSize;
    memcpy(&infoSize, pData + 16, sizeof(quint32));
    infoSize = qFromLittleEndian(infoSize);

    quint32 offset   = 20;
    QString userName = QString(QByteArray(pData + offset));
    offset += userName.toUtf8().length() + 1;

    QString header = QString(QByteArray(pData + offset));
    offset += header.toUtf8().length() + 1;

    QByteArray cryptInfo = QByteArray(pData + offset, infoSize);
    QString    info("");
    if (cryptInfo.length() > 0)
        info = QString(qUncompress(cryptInfo));

    pItem->setTimeStamp(timestamp);
    pItem->setUser(userName);
    pItem->setHeader(header);
    pItem->setInfo(info);

    return ERROR_CODE_SUCCESS;
}

qint32 DataHandling::getHandleStatisticsCommandResponse(MessageProtocol* msg)
{
    if (msg->getDataLength() < 4)
        return ERROR_CODE_WRONG_SIZE;

    const char* pData = msg->getPointerToData();
    qint32      rValue;
    memcpy(&rValue, pData, sizeof(qint32));
    rValue = qFromLittleEndian(rValue);

    if (msg->getDataLength() > 4 && rValue == ERROR_CODE_SUCCESS) {

        QByteArray jsonByteArray(pData + sizeof(qint32));
        rValue = g_Statistics->handleStatisticResponse(jsonByteArray);
    }

    return rValue;
}
