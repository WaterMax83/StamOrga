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

#include <QtCore/QDataStream>
#include <QtCore/QDateTime>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>

#include "../Common/General/config.h"
#include "../Common/General/globalfunctions.h"
#include "../Common/General/globaltiming.h"
#include "../Common/Network/messagecommand.h"
#include "../Data/seasonticket.h"
#include "dataconnection.h"
#include "pushnotification.h"

DataConnection::DataConnection(GlobalData* pGData, QObject* parent)
    : QObject(parent)
{
    this->m_pGlobalData = pGData;
}

/* Request
 * 0                Header          12
 * 12   quint16     size            2
 * 14   String      password        X
 */
/* Answer
 * 0                Header          12
 * 12               SUCCESS         4
 */
MessageProtocol* DataConnection::requestCheckUserLogin(MessageProtocol* msg)
{
    const char* pData = msg->getPointerToData();
    quint16     size  = qFromLittleEndian(*((quint16*)pData));
    qint32      result;
    if (size > msg->getDataLength())
        return new MessageProtocol(OP_CODE_CMD_RES::ACK_LOGIN_USER, ERROR_CODE_WRONG_SIZE);

    QString passw(QByteArray(pData + 2, size));
    if (msg->getVersion() == MSG_HEADER_VERSION_START)
        result = this->m_pGlobalData->m_UserList.userCheckPassword(this->m_pUserConData->m_userName, passw);
    else
        result = this->m_pGlobalData->m_UserList.userCheckPasswordHash(this->m_pUserConData->m_userName, passw, this->m_pUserConData->m_randomLogin);
    if (result == ERROR_CODE_SUCCESS) {
        this->m_pUserConData->m_bIsConnected = true;
        qInfo().noquote() << QString("User %1 logged in").arg(this->m_pUserConData->m_userName);
    } else {
        this->m_pUserConData->m_bIsConnected = false;
        qWarning().noquote() << QString("User %1 tried to login with wrong password").arg(this->m_pUserConData->m_userName);
    }

    return new MessageProtocol(OP_CODE_CMD_RES::ACK_LOGIN_USER, result);
}

/* Request
 * 0                guid            X
 * X                token           Y
 * X+Y              os              4
 */

/* Answer
 * 0                Header          12
 * 12               rvalue          4
 * 16               property        4
 * 20               index           4
 * 24               readableName    X
 */
MessageProtocol* DataConnection::requestGetUserProperties(MessageProtocol* msg)
{
    QByteArray answer;
    QString    readableName = this->m_pGlobalData->m_UserList.getReadableName(this->m_pUserConData->m_userID);
    if (msg->getVersion() < MSG_HEADER_JSON_USERPROPS) {
        if (msg->getDataLength() > 0) {
            const char* pData  = msg->getPointerToData();
            quint32     offset = 0;
            QString     guid(QByteArray(pData + offset));
            offset += guid.toUtf8().length() + 1;
            QString token(QByteArray(pData + offset));
            offset += token.toUtf8().length() + 1;
            qint32 system;
            memcpy(&system, pData + offset, sizeof(qint32));
            system = qFromLittleEndian(system);

            this->m_pUserConData->m_guid = guid;
            g_pushNotify->addNewAppInformation(guid, token, system, this->m_pUserConData->m_userID);

            if (!this->m_pUserConData->m_version.isEmpty())
                g_pushNotify->addNewVersionInformation(guid, this->m_pUserConData->m_version);
        }

        QDataStream wAnswer(&answer, QIODevice::WriteOnly);
        wAnswer.setByteOrder(QDataStream::LittleEndian);
        wAnswer << ERROR_CODE_SUCCESS;
        wAnswer << this->m_pGlobalData->m_UserList.getUserProperties(this->m_pUserConData->m_userName);
        wAnswer << this->m_pUserConData->m_userID;

        answer.append(readableName.toUtf8());
        answer.append((char)0x00);

    } else {
        QByteArray  data(msg->getPointerToData());
        QJsonObject rootObj = QJsonDocument::fromJson(data).object();
        QString     guid    = rootObj.value("guid").toString();
        QString     token   = rootObj.value("token").toString();
        qint32      os      = rootObj.value("os").toInt();

        this->m_pUserConData->m_guid = guid;
        g_pushNotify->addNewAppInformation(guid, token, os, this->m_pUserConData->m_userID);

        if (!this->m_pUserConData->m_version.isEmpty())
            g_pushNotify->addNewVersionInformation(guid, this->m_pUserConData->m_version);

        QJsonObject rootObjAns;
        rootObjAns.insert("rvalue", ERROR_CODE_SUCCESS);
        rootObjAns.insert("property", (qint32) this->m_pGlobalData->m_UserList.getUserProperties(this->m_pUserConData->m_userName));
        rootObjAns.insert("index", this->m_pUserConData->m_userID);
        rootObjAns.insert("readableName", readableName);

        QJsonArray arrTickets;
        if (this->m_pGlobalData->requestGetAvailableTicketFromUser(this->m_pUserConData->m_userID, arrTickets) == ERROR_CODE_SUCCESS)
            rootObjAns.insert("tickets", arrTickets);

        answer = QJsonDocument(rootObjAns).toJson(QJsonDocument::Compact);
    }

    qInfo().noquote() << QString("User %1 getting user properties %2").arg(this->m_pUserConData->m_userName, readableName);

    return new MessageProtocol(OP_CODE_CMD_RES::ACK_GET_USER_PROPS, answer);
}

MessageProtocol* DataConnection::requestGetUserEvents(MessageProtocol* msg)
{
    if (msg->getDataLength() < 8) {
        qWarning().noquote() << QString("Getting no data from %1 for get events").arg(this->m_pUserConData->m_userName);
        return new MessageProtocol(OP_CODE_CMD_RES::ACK_GET_USER_EVENTS, ERROR_CODE_WRONG_SIZE);
    }

    //    qint64 timestamp;
    //    memcpy(&timestamp, msg->getPointerToData(), sizeof(qint64));
    //    timestamp = qFromLittleEndian(timestamp);

    QJsonArray jsArr;
    qint32     rCode = this->m_pGlobalData->getCurrentUserEvents(jsArr, this->m_pUserConData->m_userID);

    QJsonObject jsRootObj;
    jsRootObj.insert("version", "1.0.0");
    jsRootObj.insert("events", jsArr);

    QJsonDocument jsDoc(jsRootObj);
    QByteArray    userEvents = jsDoc.toJson(QJsonDocument::Compact);
    rCode                    = qToLittleEndian(rCode);
    userEvents.prepend(sizeof(qint32), 0x0);
    memcpy((void*)userEvents.constData(), &rCode, sizeof(qint32));

    return new MessageProtocol(OP_CODE_CMD_RES::ACK_GET_USER_EVENTS, userEvents);
}

MessageProtocol* DataConnection::requestSetUserEvents(MessageProtocol* msg)
{
    if (msg->getDataLength() < 12) {
        qWarning().noquote() << QString("Getting no data from %1 for set events").arg(this->m_pUserConData->m_userName);
        return new MessageProtocol(OP_CODE_CMD_RES::ACK_SET_USER_EVENTS, ERROR_CODE_WRONG_SIZE);
    }

    qint64 eventID;
    memcpy(&eventID, msg->getPointerToData(), sizeof(qint64));
    eventID = qFromLittleEndian(eventID);
    qint32 status;
    memcpy(&status, msg->getPointerToData() + sizeof(qint64), sizeof(qint32));
    status = qFromLittleEndian(status);

    qint32 rCode = this->m_pGlobalData->acceptUserEvent(eventID, this->m_pUserConData->m_userID, status);

    return new MessageProtocol(OP_CODE_CMD_RES::ACK_SET_USER_EVENTS, rCode);
}

/*
 * 0                    Header          12
 * 12       quint16     size            2
 * 14       String      actual Passw    X
 * 14+X     quint16     size            2
 * 16+X     String      new Passw       Y
 */
MessageProtocol* DataConnection::requestUserChangeLogin(MessageProtocol* msg)
{
    if (msg->getDataLength() <= 8) {
        qWarning().noquote() << QString("Getting no user login data from %1").arg(this->m_pUserConData->m_userName);
        return NULL;
    }

    qint32      totalLength = (qint32)msg->getDataLength();
    const char* pData       = msg->getPointerToData();

    quint16 actLength = qFromLittleEndian(*((quint16*)pData));
    if (actLength + 2 > totalLength)
        return new MessageProtocol(OP_CODE_CMD_RES::ACK_USER_CHANGE_LOGIN, ERROR_CODE_WRONG_SIZE);
    QString actPassw(QByteArray(pData + 2, actLength));

    quint16 newLength = qFromLittleEndian(*((quint16*)(pData + 2 + actLength)));
    if (newLength + actLength + 2 + 2 > totalLength)
        return new MessageProtocol(OP_CODE_CMD_RES::ACK_USER_CHANGE_LOGIN, ERROR_CODE_WRONG_SIZE);
    QString newPassw(QByteArray(pData + 2 + actLength + 2, newLength));

    bool rValue;
    if (msg->getVersion() == MSG_HEADER_VERSION_START)
        rValue = this->m_pGlobalData->m_UserList.userCheckPassword(this->m_pUserConData->m_userName, actPassw);
    else
        rValue = this->m_pGlobalData->m_UserList.userCheckPasswordHash(this->m_pUserConData->m_userName, actPassw, this->m_pUserConData->m_randomLogin);
    if (!rValue)
        return new MessageProtocol(OP_CODE_CMD_RES::ACK_USER_CHANGE_LOGIN, ERROR_CODE_WRONG_PASSWORD);

    if (msg->getVersion() == MSG_HEADER_VERSION_START)
        rValue = this->m_pGlobalData->m_UserList.userChangePassword(this->m_pUserConData->m_userName, newPassw);
    else
        rValue = this->m_pGlobalData->m_UserList.userChangePasswordHash(this->m_pUserConData->m_userName, newPassw);

    if (rValue)
        return new MessageProtocol(OP_CODE_CMD_RES::ACK_USER_CHANGE_LOGIN, ERROR_CODE_SUCCESS);
    else
        return new MessageProtocol(OP_CODE_CMD_RES::ACK_USER_CHANGE_LOGIN, ERROR_CODE_COMMON);
}

/*
 * 0                    Header          12
 * 12       quint16     size            2
 * 14       String      new readName    X
 */
MessageProtocol* DataConnection::requestUserChangeReadname(MessageProtocol* msg)
{
    if (msg->getDataLength() <= 4) {
        qWarning().noquote() << QString("Getting no readname from %1").arg(this->m_pUserConData->m_userName);
        return NULL;
    }

    qint32      totalLength = (qint32)msg->getDataLength();
    const char* pData       = msg->getPointerToData();

    quint16 actLength = qFromLittleEndian(*((quint16*)pData));
    if (actLength + 2 > totalLength)
        return new MessageProtocol(OP_CODE_CMD_RES::ACK_USER_CHANGE_LOGIN, ERROR_CODE_WRONG_SIZE);
    QString newReadName(QByteArray(pData + 2, actLength));

    if (!this->m_pGlobalData->m_UserList.userChangeReadName(this->m_pUserConData->m_userName, newReadName))
        return new MessageProtocol(OP_CODE_CMD_RES::ACK_USER_CHANGE_LOGIN, ERROR_CODE_COMMON);

    return new MessageProtocol(OP_CODE_CMD_RES::ACK_USER_CHANGE_READNAME, ERROR_CODE_SUCCESS);
}

/*
 * 0                Header          12
 * 12   quint32     SUCCESS          4
 * 16   quint32     version         4
 * 20   quint16     size            2
 * 22   String      version         X
 */
MessageProtocol* DataConnection::requestGetProgramVersion(MessageProtocol* msg)
{
    if (msg->getDataLength() <= 6) {
        qWarning().noquote() << QString("Getting no version data from %1").arg(this->m_pUserConData->m_userName);
        return NULL;
    }

    const char* pData      = msg->getPointerToData();
    quint32     intVersion = qFromLittleEndian(*((quint32*)pData));
    quint16     actLength  = qFromLittleEndian(*((quint16*)(pData + 4)));
    if (actLength > msg->getDataLength())
        return new MessageProtocol(OP_CODE_CMD_RES::ACK_GET_VERSION, ERROR_CODE_WRONG_SIZE);
    QString remVersion(QByteArray(pData + 6, actLength));
    qInfo().noquote() << QString("Version from %1 = %2:0x%3").arg(this->m_pUserConData->m_userName).arg(remVersion, QString::number(intVersion, 16));
    QByteArray  ownVersion;
    QDataStream wVersion(&ownVersion, QIODevice::WriteOnly);
    wVersion.setByteOrder(QDataStream::LittleEndian);
    wVersion << ERROR_CODE_SUCCESS;

//#define VERSION_TEST
#ifdef VERSION_TEST
#define ORGA_VERSION_I 0x0B0A0000 // VX.Y.Z => 0xXXYYZZZZ
#define ORGA_VERSION_S "VB.A.0"
    wVersion << (quint32)ORGA_VERSION_I;
    wVersion << quint16(QString(ORGA_VERSION_S).toUtf8().size());
    ownVersion.append(QString(ORGA_VERSION_S));
#else
    wVersion << (quint32)STAM_ORGA_VERSION_I;
    wVersion << quint16(QString(STAM_ORGA_VERSION_S).toUtf8().size());
    ownVersion.append(QString(STAM_ORGA_VERSION_S));
#endif

    this->m_pUserConData->m_version = remVersion;
    if (!this->m_pUserConData->m_guid.isEmpty())
        g_pushNotify->addNewVersionInformation(this->m_pUserConData->m_guid, remVersion);

    return new MessageProtocol(OP_CODE_CMD_RES::ACK_GET_VERSION, ownVersion);
}


/* request
 * 0    qint32      maxPastGames    4   // updateIndex since MSG_HEADER_VERSION_GAME_LIST
 * 4    qint64      lastUpdate      8
 */

// updateIndex = 0 => getAll
// updateIndex = 1 => only Update

/* answer
 * 0   quint32     result          4
 * 4   quint16     numbofGames     2   // updateIndex since MSG_HEADER_VERSION_GAME_LIST
 * 6   quint16     sizePack1       2
 * 8   quint8      sIndex          1
 * 9   quint8      comp            1
 * 10   quint64     datetime        8
 * 18   quint32     index           4
 * 22   quint16     freeGames       2
 * 24   quint16     blockedGames    2
 * 26   quint16     reserveGames    2
 * 28   QString     infoGame        X
 * 28+X qutin16     sizePack2       2
 */

#define GAMES_OFFSET (1 + 1 + 8 + 4) // sIndex + comp + datetime + index

MessageProtocol* DataConnection::requestGetGamesList(MessageProtocol* msg)
{
    if (msg->getDataLength() != 4 && msg->getVersion() < MSG_HEADER_VERSION_GAME_LIST) {
        qWarning().noquote() << QString("Error getting wrong message size %1 for get games list from %2, expected 4")
                                    .arg(msg->getDataLength())
                                    .arg(this->m_pUserConData->m_userName);
        return new MessageProtocol(OP_CODE_CMD_RES::ACK_GET_GAMES_LIST, ERROR_CODE_WRONG_SIZE);
    } else if (msg->getVersion() >= MSG_HEADER_VERSION_GAME_LIST && msg->getDataLength() != 12) {
        qWarning().noquote() << QString("Error getting wrong message size %1 for get games list from %2, expected 12")
                                    .arg(msg->getDataLength())
                                    .arg(this->m_pUserConData->m_userName);
        return new MessageProtocol(OP_CODE_CMD_RES::ACK_GET_GAMES_LIST, ERROR_CODE_WRONG_SIZE);
    }

    const char* pData = msg->getPointerToData();
    qint32      updateIndex;
    qint64      lastUpdateGamesFromApp = 0;
    memcpy(&updateIndex, pData, sizeof(qint32));
    if (msg->getVersion() >= MSG_HEADER_VERSION_GAME_LIST)
        memcpy(&lastUpdateGamesFromApp, pData + 4, sizeof(qint64));
    updateIndex            = qFromLittleEndian(updateIndex);
    lastUpdateGamesFromApp = qFromLittleEndian(lastUpdateGamesFromApp);

    QByteArray  ackArray;
    QDataStream wAckArray(&ackArray, QIODevice::WriteOnly);
    wAckArray.setByteOrder(QDataStream::LittleEndian);

    quint16 numbOfGames = this->m_pGlobalData->m_GamesList.getNumberOfInternalList();

    wAckArray << (quint32)ERROR_CODE_SUCCESS;

    qint64 now2HoursAgo = QDateTime::currentDateTime().addSecs(-(2 * 60 * 60)).toMSecsSinceEpoch();
    qint32 startValue   = 0;
    if (msg->getVersion() < MSG_HEADER_VERSION_GAME_LIST) {
        /* Number of past game were only checked and send in first versions, now only games which were not already loaded are send */
        qint32 gamesInPast = 0;
        for (quint32 i = 0; i < numbOfGames; i++) {
            GamesPlay* pGame = (GamesPlay*)(this->m_pGlobalData->m_GamesList.getRequestConfigItemFromListIndex(i));
            if (pGame->m_timestamp < now2HoursAgo)
                gamesInPast++;
        }

        qint32 loadLastGames = updateIndex;
        if (gamesInPast > loadLastGames)
            startValue = gamesInPast - loadLastGames;
        wAckArray << quint16(numbOfGames - startValue);
    } else {
        if (lastUpdateGamesFromApp == 0)
            updateIndex = UpdateIndex::UpdateAll;
        wAckArray << qint16(updateIndex);
    }

    quint16 numbOfLoadedGames = 0;
    for (qint32 i = startValue; i < numbOfGames; i++) {
        GamesPlay* pGame = (GamesPlay*)(this->m_pGlobalData->m_GamesList.getRequestConfigItemFromListIndex(i));
        if (pGame == NULL)
            continue;
        if (msg->getVersion() >= MSG_HEADER_VERSION_GAME_LIST && updateIndex == UpdateIndex::UpdateDiff) {
            if (pGame->m_lastUpdate <= lastUpdateGamesFromApp)
                continue; // Skip game because user already has all info
        }

        QString game(pGame->m_itemName + ";" + pGame->m_away + ";" + pGame->m_score);

        wAckArray.device()->seek(ackArray.size());
        wAckArray << quint16(game.toUtf8().size() + GAMES_OFFSET);
        if (msg->getVersion() >= MSG_HEADER_ADD_FANCLUB || pGame->m_competition != BADISCHER_POKAL)
            wAckArray << quint8(pGame->m_seasonIndex);
        else {
            if (pGame->m_seasonIndex > 6)
                wAckArray << quint8(pGame->m_seasonIndex - 3);
            else
                wAckArray << quint8(pGame->m_seasonIndex);
        }


        if (msg->getVersion() >= MSG_HEADER_VERSION_GAME_LIST)
            wAckArray << quint8(quint8(pGame->m_competition) | quint8(IS_PLAY_FIXED(pGame->m_options) ? 0x80 : 0x0));
        else
            wAckArray << quint8(pGame->m_competition);
        wAckArray << pGame->m_timestamp;
        wAckArray << pGame->m_index;
        if (msg->getVersion() < MSG_HEADER_VERSION_GAME_LIST) {
            quint16 freeTickets     = this->m_pGlobalData->getTicketNumber(pGame->m_index, TICKET_STATE_FREE);
            quint16 blockTickets    = this->m_pGlobalData->getTicketNumber(pGame->m_index, TICKET_STATE_BLOCKED);
            quint16 reservedTickets = this->m_pGlobalData->getTicketNumber(pGame->m_index, TICKET_STATE_RESERVED);
            wAckArray << freeTickets << blockTickets << reservedTickets;
        }

        numbOfLoadedGames++;
        ackArray.append(game);
    }
    if (msg->getVersion() >= MSG_HEADER_VERSION_GAME_LIST) {
        qint64 lastUpdateGameFromServer = this->m_pGlobalData->m_GamesList.getLastUpdateTime();
        if (lastUpdateGameFromServer == 0)
            lastUpdateGameFromServer = lastUpdateGamesFromApp;
        wAckArray.device()->seek(ackArray.size());
        wAckArray << lastUpdateGameFromServer;
    }

    qInfo().noquote() << QString("User %1 request Games List with %2 entries").arg(this->m_pUserConData->m_userName).arg(numbOfLoadedGames);

    return new MessageProtocol(OP_CODE_CMD_RES::ACK_GET_GAMES_LIST, ackArray);
}

/* answer
 * 0   quint32     result          4
 * 4   quint32     status          4
 * 8   quint16     gameSize        2
 * 10  quint16     reserved        2
 * 12  quint32     gameIndex       4
 * 16  quint16     freeTicket      2
 * 18  quint16     blockedTicket   2
 * 20  quint16     reservedTicket  2
 * 22  quint16     acceptMeeting   2
 * 24  quint16     interestMeeting 2
 * 26  quint16     declineMeeting  2
 * 28  quint16     meetingInfo     2
 */

#define GAME_INFO_SIZE 18
#define GAME_INFO_TRIP 8

MessageProtocol* DataConnection::requestGetGamesInfoList(MessageProtocol* msg)
{
    if (msg->getDataLength() != 8) {
        qWarning().noquote() << QString("Error getting wrong message size %1 for get games info list from %2")
                                    .arg(msg->getDataLength())
                                    .arg(this->m_pUserConData->m_userName);
        return new MessageProtocol(OP_CODE_CMD_RES::ACK_GET_GAMES_INFO_LIST, ERROR_CODE_WRONG_SIZE);
    }

    if (msg->getVersion() >= MSG_HEADER_VERSION_GAME_LIST) {
        const char* pData                    = msg->getPointerToData();
        qint64      lastUpdateTicketsFromApp = 0;
        memcpy(&lastUpdateTicketsFromApp, pData, sizeof(qint64));
        lastUpdateTicketsFromApp = qFromLittleEndian(lastUpdateTicketsFromApp);
        if (this->m_pGlobalData->m_GamesList.getLastUpdateTime() > lastUpdateTicketsFromApp)
            return new MessageProtocol(OP_CODE_CMD_RES::ACK_GET_GAMES_INFO_LIST, ERROR_CODE_UPDATE_LIST);
    }

    char buffer[5000];
    memset(&buffer[0], 0x0, 5000);
    quint32 offset = 0;
    qint32  result = ERROR_CODE_SUCCESS;

    result = qToLittleEndian(result);
    memcpy(&buffer[offset], &result, sizeof(qint32));
    offset += sizeof(qint32);

    offset += sizeof(quint32); // status is not yet used

    quint16 sizeOfGame = GAME_INFO_SIZE;
    if (msg->getVersion() >= MSG_HEADER_ADD_AWAYTRIP_INFO)
        sizeOfGame += GAME_INFO_TRIP;
    quint16 size = qToLittleEndian(sizeOfGame);
    memcpy(&buffer[offset], &size, sizeof(quint16));
    offset += sizeof(quint32); // reserved is not yet used

    qint32 numbOfGames = this->m_pGlobalData->m_GamesList.getNumberOfInternalList();
#ifndef QT_DEBUG
    qint64 currentTime = QDateTime::currentMSecsSinceEpoch();
#endif

    for (qint32 i = 0; i < numbOfGames; i++) {

        if (offset + sizeOfGame > 5000)
            break;

        GamesPlay* pGame = (GamesPlay*)(this->m_pGlobalData->m_GamesList.getRequestConfigItemFromListIndex(i));
        if (pGame == NULL)
            continue;

#ifndef QT_DEBUG
        if (pGame->m_timestamp + 2 * MSEC_PER_HOUR < currentTime)
            continue;
#endif
        quint16 freeTickets     = this->m_pGlobalData->getTicketNumber(pGame->m_index, TICKET_STATE_FREE);
        quint16 blockTickets    = this->m_pGlobalData->getTicketNumber(pGame->m_index, TICKET_STATE_BLOCKED);
        quint16 reservedTickets = this->m_pGlobalData->getTicketNumber(pGame->m_index, TICKET_STATE_RESERVED);
        quint16 acceptedMeeting = this->m_pGlobalData->getAcceptedNumber(MEETING_TYPE_MEETING, pGame->m_index, ACCEPT_STATE_ACCEPT);
        quint16 interestMeeting = this->m_pGlobalData->getAcceptedNumber(MEETING_TYPE_MEETING, pGame->m_index, ACCEPT_STATE_MAYBE);
        quint16 declinedMeeting = this->m_pGlobalData->getAcceptedNumber(MEETING_TYPE_MEETING, pGame->m_index, ACCEPT_STATE_DECLINE);
        quint16 meetInfo        = this->m_pGlobalData->getMeetingInfoValue(MEETING_TYPE_MEETING, pGame->m_index);
        quint16 acceptedTrip    = this->m_pGlobalData->getAcceptedNumber(MEETING_TYPE_AWAYTRIP, pGame->m_index, ACCEPT_STATE_ACCEPT);
        quint16 interestTrip    = this->m_pGlobalData->getAcceptedNumber(MEETING_TYPE_AWAYTRIP, pGame->m_index, ACCEPT_STATE_MAYBE);
        quint16 declinedTrip    = this->m_pGlobalData->getAcceptedNumber(MEETING_TYPE_AWAYTRIP, pGame->m_index, ACCEPT_STATE_DECLINE);
        quint16 tripInfo        = this->m_pGlobalData->getMeetingInfoValue(MEETING_TYPE_AWAYTRIP, pGame->m_index);

        if (freeTickets == 0 && reservedTickets == 0 && acceptedMeeting == 0 && interestMeeting == 0 && declinedMeeting == 0 && meetInfo == 0 && acceptedTrip == 0 && interestTrip == 0 && declinedTrip == 0 && tripInfo == 0)
            continue;

        quint32 gameIndex = qToLittleEndian(pGame->m_index);
        memcpy(&buffer[offset], &gameIndex, sizeof(quint16));
        offset += sizeof(quint32);

        freeTickets = qToLittleEndian(freeTickets);
        memcpy(&buffer[offset], &freeTickets, sizeof(quint16));
        offset += sizeof(quint16);

        blockTickets = qToLittleEndian(blockTickets);
        memcpy(&buffer[offset], &blockTickets, sizeof(quint16));
        offset += sizeof(quint16);

        reservedTickets = qToLittleEndian(reservedTickets);
        memcpy(&buffer[offset], &reservedTickets, sizeof(quint16));
        offset += sizeof(quint16);

        acceptedMeeting = qToLittleEndian(acceptedMeeting);
        memcpy(&buffer[offset], &acceptedMeeting, sizeof(quint16));
        offset += sizeof(quint16);

        interestMeeting = qToLittleEndian(interestMeeting);
        memcpy(&buffer[offset], &interestMeeting, sizeof(quint16));
        offset += sizeof(quint16);

        declinedMeeting = qToLittleEndian(declinedMeeting);
        memcpy(&buffer[offset], &declinedMeeting, sizeof(quint16));
        offset += sizeof(quint16);

        meetInfo = qToLittleEndian(meetInfo);
        memcpy(&buffer[offset], &meetInfo, sizeof(quint16));
        offset += sizeof(quint16);

        if (msg->getVersion() >= MSG_HEADER_ADD_AWAYTRIP_INFO) {
            acceptedTrip = qToLittleEndian(acceptedTrip);
            memcpy(&buffer[offset], &acceptedTrip, sizeof(quint16));
            offset += sizeof(quint16);

            interestTrip = qToLittleEndian(interestTrip);
            memcpy(&buffer[offset], &interestTrip, sizeof(quint16));
            offset += sizeof(quint16);

            declinedTrip = qToLittleEndian(declinedTrip);
            memcpy(&buffer[offset], &declinedTrip, sizeof(quint16));
            offset += sizeof(quint16);

            tripInfo = qToLittleEndian(tripInfo);
            memcpy(&buffer[offset], &tripInfo, sizeof(quint16));
            offset += sizeof(quint16);
        }
    }

    qInfo().noquote() << QString("User %1 request Games Info List").arg(this->m_pUserConData->m_userName);

    return new MessageProtocol(OP_CODE_CMD_RES::ACK_GET_GAMES_INFO_LIST, &buffer[0], offset);
}

MessageProtocol* DataConnection::requestSetFixedGameTime(MessageProtocol* msg)
{
    Q_UNUSED(msg);

    return new MessageProtocol(OP_CODE_CMD_RES::ACK_SET_FIXED_GAME_TIME, ERROR_CODE_NOT_IMPLEMENTED);
}

/*
 * 0     quint32     result          4
 * 4     quint16     numbOfTickets   2
 * 6     quint16     sizeTick1       2
 * 8     quint8      discount        1
 * 9     quint32     index           4
 * 13     quint32     userIndex       4
 * 17     QString     name+place      X
 * 17+X   qutin16     sizeTick2       2
 */

#define TICKET_OFFSET (1 + 4 + 4) // discount + isOwnUser + index + userIndex

MessageProtocol* DataConnection::requestGetTicketsList(MessageProtocol* msg)
{
    if (msg->getVersion() >= MSG_HEADER_ADD_FANCLUB && msg->getDataLength() != 12) {
        qWarning().noquote() << QString("Error getting wrong message size %1 for get ticket list from %2, expected 12")
                                    .arg(msg->getDataLength())
                                    .arg(this->m_pUserConData->m_userName);
        return new MessageProtocol(OP_CODE_CMD_RES::ACK_GET_TICKETS_LIST, ERROR_CODE_WRONG_SIZE);
    }

    quint32 updInd                   = UpdateIndex::UpdateAll;
    qint64  lastUpdateTicketsFromApp = 0;
    if (msg->getVersion() >= MSG_HEADER_ADD_FANCLUB) {
        const char* pData = msg->getPointerToData();
        memcpy(&updInd, pData, sizeof(qint64));
        memcpy(&lastUpdateTicketsFromApp, pData + 4, sizeof(qint64));
        lastUpdateTicketsFromApp = qFromLittleEndian(lastUpdateTicketsFromApp);
        if (lastUpdateTicketsFromApp == 0)
            updInd = UpdateIndex::UpdateAll;
    }

    QByteArray  ackArray;
    QDataStream wAckArray(&ackArray, QIODevice::WriteOnly);
    wAckArray.setByteOrder(QDataStream::LittleEndian);

    quint16 updateIndex   = (quint16)qFromLittleEndian(updInd);
    quint16 numbOfTickets = this->m_pGlobalData->m_SeasonTicket.getNumberOfInternalList();
    wAckArray << (quint32)ERROR_CODE_SUCCESS;
    if (msg->getVersion() >= MSG_HEADER_VERSION_GAME_LIST)
        wAckArray << updateIndex;
    else
        wAckArray << numbOfTickets;


    for (quint32 i = 0; i < numbOfTickets; i++) {
        TicketInfo* pTicket = (TicketInfo*)(this->m_pGlobalData->m_SeasonTicket.getRequestConfigItemFromListIndex(i));
        if (pTicket == NULL || pTicket->isTicketRemoved())
            continue;

        if (msg->getVersion() >= MSG_HEADER_ADD_FANCLUB && updateIndex == UpdateIndex::UpdateDiff) {
            if (pTicket->m_timestamp <= lastUpdateTicketsFromApp)
                continue; // Skip ticket because user already has all info
        }

        QString ticket(pTicket->m_itemName + ";" + pTicket->m_place);

        wAckArray.device()->seek(ackArray.size());
        wAckArray << quint16(ticket.toUtf8().size() + TICKET_OFFSET);
        wAckArray << quint8(pTicket->m_discount);
        wAckArray << pTicket->m_index;
        wAckArray << pTicket->m_userIndex;

        ackArray.append(ticket);
    }
    if (msg->getVersion() >= MSG_HEADER_VERSION_GAME_LIST) {
        wAckArray.device()->seek(ackArray.size());
        wAckArray << this->m_pGlobalData->m_SeasonTicket.getLastUpdateTime();
    }

    qInfo().noquote() << QString("User %1 request Ticket List with %2 entries").arg(this->m_pUserConData->m_userName).arg(numbOfTickets);

    return new MessageProtocol(OP_CODE_CMD_RES::ACK_GET_TICKETS_LIST, ackArray);
}

/*
 * 0                Header          12
 * 12   quint32     discount        4
 * 16   quint16     size            2
 * 18   String      name            X
 */
MessageProtocol* DataConnection::requestAddSeasonTicket(MessageProtocol* msg)
{
    if (msg->getDataLength() <= 6) {
        qWarning().noquote() << QString("Message for adding season ticket to short for user %1").arg(this->m_pUserConData->m_userName);
        return new MessageProtocol(OP_CODE_CMD_RES::ACK_ADD_TICKET, ERROR_CODE_WRONG_SIZE);
    }

    const char* pData     = msg->getPointerToData();
    quint32     discount  = qFromLittleEndian(*((quint32*)(pData)));
    quint16     actLength = qFromLittleEndian(*((quint16*)(pData + 4)));
    if (actLength > msg->getDataLength())
        return new MessageProtocol(OP_CODE_CMD_RES::ACK_ADD_TICKET, ERROR_CODE_WRONG_SIZE);
    QString ticketName(QByteArray(pData + 6, actLength));

    QString userName  = this->m_pUserConData->m_userName;
    qint32  userIndex = this->m_pUserConData->m_userID;
    qint32  rCode     = this->m_pGlobalData->m_SeasonTicket.addNewSeasonTicket(userName, userIndex, ticketName, discount);
    if (rCode > ERROR_CODE_NO_ERROR) {
        qInfo().noquote() << QString("User %1 added SeasonTicket %2")
                                 .arg(this->m_pUserConData->m_userName)
                                 .arg(ticketName);
        return new MessageProtocol(OP_CODE_CMD_RES::ACK_ADD_TICKET, ERROR_CODE_SUCCESS);
    }
    return new MessageProtocol(OP_CODE_CMD_RES::ACK_ADD_TICKET, rCode);
}

/*  request
 * 0                Header          12
 * 12   quint32      index           4
 */
MessageProtocol* DataConnection::requestRemoveSeasonTicket(MessageProtocol* msg)
{
    qint32 rCode;
    if (msg->getDataLength() != 4) {
        qWarning().noquote() << QString("Wrong message size for removing season ticket for user %1").arg(this->m_pUserConData->m_userName);
        return new MessageProtocol(OP_CODE_CMD_RES::ACK_REMOVE_TICKET, ERROR_CODE_WRONG_SIZE);
    }

    const char* pData = msg->getPointerToData();
    quint32     index = qFromLittleEndian(*((quint32*)pData));

    if ((rCode = this->m_pGlobalData->m_SeasonTicket.removeTicketItem(index)) == ERROR_CODE_SUCCESS) {
        qInfo().noquote() << QString("User %1 removed SeasonTicket %2")
                                 .arg(this->m_pUserConData->m_userName)
                                 .arg(index);
        return new MessageProtocol(OP_CODE_CMD_RES::ACK_REMOVE_TICKET, ERROR_CODE_SUCCESS);
    }
    return new MessageProtocol(OP_CODE_CMD_RES::ACK_REMOVE_TICKET, rCode);
}

/*  request
 * 0                Header          12
 * 12   quint32      index           4
 * 16   quint16      size            2
 * 18   QString      newPlace        X
 */
/* OBSOLETE, MAYBE REMOVE IN FUTURE */
MessageProtocol* DataConnection::requestNewPlaceSeasonTicket(MessageProtocol* msg)
{
    qint32 rCode;
    if (msg->getDataLength() <= 6) {
        qWarning().noquote() << QString("Wrong message size for new place season ticket for user %1").arg(this->m_pUserConData->m_userName);
        return new MessageProtocol(OP_CODE_CMD_RES::ACK_REMOVE_TICKET, ERROR_CODE_WRONG_SIZE);
    }

    const char* pData     = msg->getPointerToData();
    quint32     index     = qFromLittleEndian(*((quint32*)pData));
    quint16     actLength = qFromLittleEndian(*((quint16*)(pData + 4)));
    if (actLength > msg->getDataLength())
        return new MessageProtocol(OP_CODE_CMD_RES::ACK_ADD_TICKET, ERROR_CODE_WRONG_SIZE);
    QString newPlace(QByteArray(pData + 6, actLength));

    if ((rCode = this->m_pGlobalData->m_SeasonTicket.changeSeasonTicketInfos(index, -1, "", newPlace)) == ERROR_CODE_SUCCESS) {
        qInfo().noquote() << QString("User %1 changed SeasonTicket place to %2")
                                 .arg(this->m_pUserConData->m_userName)
                                 .arg(newPlace);
        return new MessageProtocol(OP_CODE_CMD_RES::ACK_NEW_TICKET_PLACE, ERROR_CODE_SUCCESS);
    }
    return new MessageProtocol(OP_CODE_CMD_RES::ACK_NEW_TICKET_PLACE, rCode);
}

/*  request
 * 0                Header          Z
 * 0    quint32     index           4
 * 4    quint32     discount        4
 * 8    QString     name            X
 * 9+X  QString     place           Y
 */
MessageProtocol* DataConnection::requestChangeSeasonTicket(MessageProtocol* msg)
{
    qint32 rCode;
    /* The first 1 in ACK was missing, for older version */
    quint32 msgIndex = OP_CODE_CMD_RES::ACK_CHANGE_TICKET;
    if (msg->getVersion() < MSG_HEADER_ADD_FANCLUB)
        msgIndex &= 0x0FFFFFFF;
    if (msg->getDataLength() < 10) {
        qWarning().noquote() << QString("Wrong message size for chance season ticket for user %1").arg(this->m_pUserConData->m_userName);
        return new MessageProtocol(msgIndex, ERROR_CODE_WRONG_SIZE);
    }

    const char* pData = msg->getPointerToData();
    quint32     ticketIndex, discount;
    memcpy(&ticketIndex, pData, sizeof(quint32));
    ticketIndex = qFromLittleEndian(ticketIndex);
    memcpy(&discount, pData + 4, sizeof(quint32));
    discount       = qFromLittleEndian(discount);
    quint32 offset = 8;

    QString name(QByteArray(pData + offset));
    offset += name.toUtf8().size() + 1;
    QString place(QByteArray(pData + offset));

    if ((rCode = this->m_pGlobalData->m_SeasonTicket.changeSeasonTicketInfos(ticketIndex, discount, name, place)) == ERROR_CODE_SUCCESS) {
        qInfo().noquote() << QString("User %1 changed SeasonTicket infos from index %2")
                                 .arg(this->m_pUserConData->m_userName)
                                 .arg(ticketIndex);
        return new MessageProtocol(msgIndex, ERROR_CODE_SUCCESS);
    }
    return new MessageProtocol(msgIndex, rCode);
}


/*  request
 * 0                Header          12
 * 12   quint32      ticketIndex     4
 * 16   quint32      gameIndex       4
 * 20   quint32      state           4
 * 24   quint16      size            2
 * 26   QString      reserveName     X
 */
MessageProtocol* DataConnection::requestChangeStateSeasonTicket(MessageProtocol* msg)
{
    qint32 rCode;
    if (msg->getDataLength() < 14) {
        qWarning().noquote() << QString("Wrong message size for free Season ticket for user %1").arg(this->m_pUserConData->m_userName);
        return new MessageProtocol(OP_CODE_CMD_RES::ACK_STATE_CHANGE_SEASON_TICKET, ERROR_CODE_WRONG_SIZE);
    }

    const char* pData       = msg->getPointerToData();
    quint32     ticketIndex = qFromLittleEndian(*((quint32*)pData));
    quint32     gameIndex   = qFromLittleEndian(*((quint32*)(pData + 4)));
    quint32     state       = qFromLittleEndian(*((quint32*)(pData + 8)));
    quint16     actLength   = qFromLittleEndian(*((quint16*)(pData + 12)));
    if (actLength > msg->getDataLength())
        return new MessageProtocol(OP_CODE_CMD_RES::ACK_STATE_CHANGE_SEASON_TICKET, ERROR_CODE_WRONG_SIZE);

    QString reserveName(QByteArray(pData + 14, actLength));

    qint64 messageID;
    rCode = this->m_pGlobalData->requestChangeStateSeasonTicket(ticketIndex, gameIndex, state,
                                                                reserveName, this->m_pUserConData->m_userID,
                                                                messageID);
    if (rCode == ERROR_CODE_SUCCESS)
        qInfo().noquote() << QString("User %1 set SeasonTicket %2 state to %3")
                                 .arg(this->m_pUserConData->m_userName)
                                 .arg(this->m_pGlobalData->m_SeasonTicket.getItemName(ticketIndex))
                                 .arg(state);

    if (msg->getVersion() < MSG_HEADER_VERSION_MESSAGE_ID)
        return new MessageProtocol(OP_CODE_CMD_RES::ACK_STATE_CHANGE_SEASON_TICKET, rCode);

    qint32 data[3];
    data[0]   = qToLittleEndian(rCode);
    messageID = qToLittleEndian(messageID);
    memcpy(&data[1], &messageID, sizeof(qint64));
    return new MessageProtocol(OP_CODE_CMD_RES::ACK_STATE_CHANGE_SEASON_TICKET, (char*)&data[0], sizeof(qint32) * 3);
}

/*  request
 * 0   quint32      gameIndex       4
 * 4   qint64       lastTimeStamp   8
 */
MessageProtocol* DataConnection::requestGetAvailableTicketList(MessageProtocol* msg)
{
    qint32 rCode;
    if (msg->getDataLength() != 4 && msg->getVersion() < MSG_HEADER_VERSION_GAME_LIST) {
        qWarning().noquote() << QString("Wrong message size %2 for get available Season ticket for user %1, expected 4").arg(this->m_pUserConData->m_userName).arg(msg->getDataLength());
        return new MessageProtocol(OP_CODE_CMD_RES::ACK_GET_AVAILABLE_TICKETS, ERROR_CODE_WRONG_SIZE);
    } else if (msg->getDataLength() != 12 && msg->getVersion() >= MSG_HEADER_VERSION_GAME_LIST) {
        qWarning().noquote() << QString("Wrong message size %2 for get available Season ticket for user %1, expected 12").arg(this->m_pUserConData->m_userName).arg(msg->getDataLength());
        return new MessageProtocol(OP_CODE_CMD_RES::ACK_GET_AVAILABLE_TICKETS, ERROR_CODE_WRONG_SIZE);
    }

    const char* pData                    = msg->getPointerToData();
    quint32     gameIndex                = qFromLittleEndian(*((quint32*)pData));
    qint64      lastUpdateTicketsFromApp = 0;
    if (msg->getVersion() >= MSG_HEADER_VERSION_GAME_LIST) {
        memcpy(&lastUpdateTicketsFromApp, pData + 4, sizeof(qint64));
        lastUpdateTicketsFromApp = qFromLittleEndian(lastUpdateTicketsFromApp);
        if (this->m_pGlobalData->m_SeasonTicket.getLastUpdateTime() > lastUpdateTicketsFromApp)
            return new MessageProtocol(OP_CODE_CMD_RES::ACK_GET_AVAILABLE_TICKETS, ERROR_CODE_UPDATE_LIST);
    }

    QByteArray data;
    if ((rCode = this->m_pGlobalData->requestGetAvailableSeasonTicket(gameIndex, this->m_pUserConData->m_userName, data)) == ERROR_CODE_SUCCESS)
        return new MessageProtocol(OP_CODE_CMD_RES::ACK_GET_AVAILABLE_TICKETS, data);
    return new MessageProtocol(OP_CODE_CMD_RES::ACK_GET_AVAILABLE_TICKETS, rCode);
}

MessageProtocol* DataConnection::requestChangeGame(MessageProtocol* msg)
{
    if (msg->getDataLength() < 4) {
        qWarning().noquote() << QString("Wrong message size for request change game for user %1").arg(this->m_pUserConData->m_userName);
        return new MessageProtocol(OP_CODE_CMD_RES::ACK_CHANGE_GAME_UDP, ERROR_CODE_WRONG_SIZE);
    }

    QString     info(msg->getPointerToData());
    QStringList parts = info.split(";");
    if (parts.length() < 7) {
        qWarning().noquote() << QString("Wrong message content count %1 for request change game for user %2").arg(parts.length()).arg(this->m_pUserConData->m_userName);
        return new MessageProtocol(OP_CODE_CMD_RES::ACK_CHANGE_GAME_UDP, ERROR_CODE_WRONG_PARAMETER);
    }


    QString   home      = parts[0];
    QString   away      = parts[1];
    QDateTime test      = QDateTime::fromString(parts[2], "dd.MM.yyyy hh:mm");
    qint64    timestamp = test.toMSecsSinceEpoch();
    if (timestamp < 0) {
        qWarning().noquote() << QString("Wrong timestamp %1 for request change game for user %2").arg(parts[2], this->m_pUserConData->m_userName);
        return new MessageProtocol(OP_CODE_CMD_RES::ACK_CHANGE_GAME_UDP, ERROR_CODE_WRONG_PARAMETER);
    }
    QString          score  = parts[3];
    qint32           index  = parts[4].toInt();
    quint32          sIndex = parts[5].toUInt();
    CompetitionIndex comp   = CompetitionIndex(parts[6].toUInt());

    /* game already exists, should only be changed */
    if (index > 0) {
        quint16    saison = getSeasonFromTimeStamp(timestamp);
        GamesPlay* pGame  = this->m_pGlobalData->m_GamesList.gameExists(sIndex, comp, saison, timestamp);
        if (pGame == NULL) {
            qWarning().noquote() << QString("user %1 tried to change game %2, but game would be added, abort it").arg(this->m_pUserConData->m_userName).arg(index);
            return new MessageProtocol(OP_CODE_CMD_RES::ACK_CHANGE_GAME_UDP, ERROR_CODE_NOT_FOUND);
        }
    }

    qint32 result = this->m_pGlobalData->m_GamesList.addNewGame(home, away, timestamp, sIndex, score, comp);
    if (result < 0)
        return new MessageProtocol(OP_CODE_CMD_RES::ACK_CHANGE_GAME_UDP, result);
    if (index > 0 && index != result) {
        qWarning().noquote() << QString("user %1 tried to changed game %2, but added game %3").arg(this->m_pUserConData->m_userName).arg(index).arg(result);
    }
    return new MessageProtocol(OP_CODE_CMD_RES::ACK_CHANGE_GAME_UDP, ERROR_CODE_SUCCESS);
}

/*  request
 * 0   quint32      gameIndex       4
 * 4   QString      when
 * X   QString       where
 * Y   QString       info
 */
MessageProtocol* DataConnection::requestChangeMeetingInfo(MessageProtocol* msg, const quint32 ack)
{
    qint32 rCode;
    if (msg->getDataLength() <= 4) {
        qWarning().noquote() << QString("Wrong message size for change meeting for user %1").arg(this->m_pUserConData->m_userName);
        return new MessageProtocol(ack, ERROR_CODE_WRONG_SIZE);
    }

    const char* pData = msg->getPointerToData();
    quint32     gameIndex;
    memcpy(&gameIndex, pData, sizeof(quint32));
    gameIndex = qFromLittleEndian(gameIndex);

    quint32 offset = 4;
    QString when(QByteArray(pData + offset));
    offset += when.toUtf8().size() + 1;
    QString where(QByteArray(pData + offset));
    offset += where.toUtf8().size() + 1;
    QString info(QByteArray(pData + offset));
    offset += info.toUtf8().size() + 1;

    quint32 type = MEETING_TYPE_MEETING;
    if (offset + 4 <= msg->getDataLength())
        memcpy(&type, pData + offset, sizeof(quint32));
    type = qFromLittleEndian(type);

    qint64 messageID = -1;
    rCode            = this->m_pGlobalData->requestChangeMeetingInfo(gameIndex, 0, when, where, info, this->m_pUserConData->m_userID, type, messageID);
    if (rCode == ERROR_CODE_SUCCESS)
        qInfo().noquote() << QString("User %1 set MeetingInfo of game %2")
                                 .arg(this->m_pUserConData->m_userName)
                                 .arg(this->m_pGlobalData->m_GamesList.getItemName(gameIndex));

    if (msg->getVersion() < MSG_HEADER_VERSION_MESSAGE_ID)
        return new MessageProtocol(ack, rCode);

    qint32 data[3];
    data[0]   = qToLittleEndian(rCode);
    messageID = qToLittleEndian(messageID);
    memcpy(&data[1], &messageID, sizeof(qint64));
    return new MessageProtocol(ack, (char*)&data[0], sizeof(qint32) * 3);
}

/*  request
 * 0   quint32      gameIndex       4
 */
MessageProtocol* DataConnection::requestGetMeetingInfo(MessageProtocol* msg, const quint32 ack)
{
    qint32 rCode;
    if (msg->getDataLength() < 4) {
        qWarning().noquote() << QString("Wrong message size for get meeting for user %1").arg(this->m_pUserConData->m_userName);
        return new MessageProtocol(ack, ERROR_CODE_WRONG_SIZE);
    }

    const char* pData = msg->getPointerToData();
    quint32     gameIndex;
    memcpy(&gameIndex, pData, sizeof(quint32));
    gameIndex = qFromLittleEndian(gameIndex);

    quint32 type = MEETING_TYPE_MEETING;
    if (msg->getDataLength() >= 8)
        memcpy(&type, pData + sizeof(quint32), sizeof(quint32));
    type = qFromLittleEndian(type);

    qint32 size = 10000;
    char   buffer[10000];

    if ((rCode = this->m_pGlobalData->requestGetMeetingInfo(gameIndex, 0, &buffer[0], type, size)) == ERROR_CODE_SUCCESS) {
        GamesPlay* pGame = (GamesPlay*)this->m_pGlobalData->m_GamesList.getItem(gameIndex);
        qInfo().noquote() << QString("User %1 got MeetingInfo of game %2:%3:%4")
                                 .arg(this->m_pUserConData->m_userName)
                                 .arg(gameIndex)
                                 .arg(pGame->m_competition)
                                 .arg(pGame->m_seasonIndex);
        return new MessageProtocol(ack, &buffer[0], size);
    }
    qInfo().noquote() << QString("User %1 got MeetingInfo of game %2 with result %3")
                             .arg(this->m_pUserConData->m_userName)
                             .arg(this->m_pGlobalData->m_GamesList.getItemName(gameIndex))
                             .arg(rCode);
    return new MessageProtocol(ack, rCode);
}

/*  request
 * 0   quint32      gameIndex       4
 * 4   quint32      acceptValue     4
 * 8   quint32      acceptIndex     4
 * 12   QString      name            4
 */
MessageProtocol* DataConnection::requestAcceptMeeting(MessageProtocol* msg, const quint32 ack, const MeetingType type)
{
    qint32 rCode;
    if (msg->getDataLength() < 16) {
        qWarning().noquote() << QString("Wrong message size for accept meeting for user %1").arg(this->m_pUserConData->m_userName);
        return new MessageProtocol(ack, ERROR_CODE_WRONG_SIZE);
    }

    const quint32* pData = (quint32*)msg->getPointerToData();
    quint32        gameIndex, acceptValue, acceptIndex;

    gameIndex   = qFromLittleEndian(pData[0]);
    acceptValue = qFromLittleEndian(pData[1]);
    acceptIndex = qFromLittleEndian(pData[2]);

    QString name(QByteArray((char*)(pData + 3)));

    qint64 messageID;
    rCode = this->m_pGlobalData->requestAcceptMeetingInfo(gameIndex, 0, acceptValue, acceptIndex, name, this->m_pUserConData->m_userID, type, messageID);
    if (rCode == ERROR_CODE_SUCCESS) {
        qInfo().noquote() << QString("User %1 accepted MeetingInfo of game %2 with value %3")
                                 .arg(this->m_pUserConData->m_userName)
                                 .arg(this->m_pGlobalData->m_GamesList.getItemName(gameIndex))
                                 .arg(acceptValue);
    }
    if (msg->getVersion() <= MSG_HEADER_VERSION_MESSAGE_ID)
        return new MessageProtocol(ack, rCode);

    qint32 data[3];
    data[0]   = qToLittleEndian(rCode);
    messageID = qToLittleEndian(messageID);
    memcpy(&data[1], &messageID, sizeof(qint64));
    return new MessageProtocol(ack, (char*)&data[0], sizeof(qint32) * 3);
}

/* request
 * 0    quint32      newsIndex      4
 * 4    quint32      infoSize       4
 * 8    QString      header         X
 * 8+X  QString      text           Y
 */
MessageProtocol* DataConnection::requestChangeNewsData(MessageProtocol* msg)
{
    qint32 rCode = ERROR_CODE_NOT_POSSIBLE;
    if (msg->getDataLength() < 10) {
        qWarning().noquote() << QString("Wrong message size for ChangeNewsData for user %1").arg(this->m_pUserConData->m_userName);
        return new MessageProtocol(OP_CODE_CMD_RES::ACK_CHANGE_NEWS_DATA, ERROR_CODE_WRONG_SIZE);
    }

    const quint32* pData = (quint32*)msg->getPointerToData();
    quint32        newsIndex, infoSize;

    newsIndex = qFromLittleEndian(pData[0]);
    infoSize  = qFromLittleEndian(pData[1]);

    QString    header(QByteArray((char*)(pData + 2)));
    int        offset = header.toUtf8().length() + 1 + 2 * sizeof(quint32);
    QByteArray infoCompressed(msg->getPointerToData() + offset, infoSize);

    QByteArray infoNotC = qUncompress(infoCompressed);
    QString    text(infoNotC);
    Q_UNUSED(text);

    qint32 returnData[4];
    if (newsIndex == 0) {
        rCode = this->m_pGlobalData->m_fanclubNews.addNewFanclubNews(header, infoCompressed, this->m_pUserConData->m_userID);
        if (rCode > ERROR_CODE_NO_ERROR) {
            qInfo().noquote() << QString("User %1 added news %2").arg(this->m_pUserConData->m_userName, header);
            qint64 messageID = g_pushNotify->sendNewFanclubNewsNotification("Es gibt eine neue Nachricht", this->m_pUserConData->m_userID, rCode);
            returnData[0]    = qToLittleEndian(ERROR_CODE_SUCCESS);
            returnData[1]    = qToLittleEndian(rCode);
            memcpy(&returnData[2], &messageID, sizeof(qint64));
            return new MessageProtocol(OP_CODE_CMD_RES::ACK_CHANGE_NEWS_DATA, (char*)&returnData[0], 16);
        }
    } else {
        rCode = this->m_pGlobalData->m_fanclubNews.changeFanclubNews(newsIndex, header, infoCompressed, this->m_pUserConData->m_userID);
        if (rCode == ERROR_CODE_SUCCESS) {
            qInfo().noquote() << QString("User %1 changed news %2").arg(this->m_pUserConData->m_userName, header);
            qint64 messageID = g_pushNotify->sendNewFanclubNewsNotification("Eine Nachricht wurde geändert", this->m_pUserConData->m_userID, newsIndex);
            returnData[0]    = qToLittleEndian(ERROR_CODE_SUCCESS);
            returnData[1]    = qToLittleEndian(newsIndex);
            memcpy(&returnData[2], &messageID, sizeof(qint64));
            return new MessageProtocol(OP_CODE_CMD_RES::ACK_CHANGE_NEWS_DATA, (char*)&returnData[0], 16);
        }
    }
    returnData[0] = qToLittleEndian(rCode);
    returnData[1] = qToLittleEndian(-1);
    returnData[2] = 0x0;
    returnData[3] = 0x0;

    return new MessageProtocol(OP_CODE_CMD_RES::ACK_CHANGE_NEWS_DATA, (char*)&returnData[0], 16);
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
MessageProtocol* DataConnection::requestGetNewsDataList(MessageProtocol* msg)
{
    Q_UNUSED(msg);

    quint32     updateIndex           = UpdateIndex::UpdateAll;
    qint64      lastUpdateNewsFromApp = 0;
    const char* pData                 = msg->getPointerToData();
    memcpy(&updateIndex, pData, sizeof(quint32));
    memcpy(&lastUpdateNewsFromApp, pData + 4, sizeof(qint64));
    lastUpdateNewsFromApp = qFromLittleEndian(lastUpdateNewsFromApp);
    updateIndex           = qFromLittleEndian(updateIndex);

    if (lastUpdateNewsFromApp == 0)
        updateIndex = UpdateIndex::UpdateAll;

    qint32 numbOfNews = this->m_pGlobalData->m_fanclubNews.getNumberOfInternalList();
    char*  buffer;
    if (numbOfNews > 0) {
        buffer = new char[numbOfNews * 200];
        memset(&buffer[0], 0x0, numbOfNews * 200);
    } else {
        buffer = new char[20];
        memset(&buffer[0], 0x0, 20);
    }

    quint32 offset = 0;
    qint32  result = ERROR_CODE_SUCCESS;

    result = qToLittleEndian(result);
    memcpy(&buffer[offset], &result, sizeof(qint32));
    offset += sizeof(qint32);

    memcpy(&buffer[offset], &updateIndex, sizeof(quint32));
    offset += sizeof(quint32); // Status is not yet used

    qint64 lastUpdateNewsFromServer = qToLittleEndian(this->m_pGlobalData->m_fanclubNews.getLastUpdateTime());
    memcpy(&buffer[offset], &lastUpdateNewsFromServer, sizeof(qint64));
    offset += sizeof(qint64);

    quint32    index;
    qint64     timestamp;
    QByteArray name, header;
    for (qint32 i = 0; i < numbOfNews; i++) {
        NewsData* pItem = (NewsData*)(this->m_pGlobalData->m_fanclubNews.getRequestConfigItemFromListIndex(i));
        if (pItem == NULL)
            continue;

        if (updateIndex == UpdateIndex::UpdateDiff) {
            if (pItem->m_timestamp <= lastUpdateNewsFromApp)
                continue; // Skip ticket because user already has all info
        }

        index = qToLittleEndian(pItem->m_index);
        memcpy(&buffer[offset], &index, sizeof(quint32));
        offset += sizeof(quint32);

        timestamp = qToLittleEndian(pItem->m_timestamp);
        memcpy(&buffer[offset], &timestamp, sizeof(qint64));
        offset += sizeof(qint64);

        name = g_ListedUser->getReadableName(pItem->m_userID).toUtf8();
        memcpy(&buffer[offset], name.constData(), name.length());
        offset += name.length() + 1;

        header = pItem->m_itemName.toUtf8();
        memcpy(&buffer[offset], header.constData(), header.length());
        offset += header.length() + 1;
    }

    MessageProtocol* ack = new MessageProtocol(OP_CODE_CMD_RES::ACK_GET_NEWS_DATA_LIST, buffer, offset);
    delete[] buffer;
    qInfo().noquote() << QString("User %1 got fanclub news list with %2 entries").arg(this->m_pUserConData->m_userName).arg(numbOfNews);
    return ack;
}

/* answer
 * 0    quint32      result         4
 * 4    quint32      newsIndex      4
 * 8    qint64       timestamp      8
 * 16   quint32      infoSize       4
 * 20   QString      userName       X
 * X    QString      header         Y
 * Y    QString      info           Z
 */
MessageProtocol* DataConnection::requestGetNewsDataItem(MessageProtocol* msg)
{
    qint32 rCode = ERROR_CODE_NOT_FOUND;
    if (msg->getDataLength() != 4) {
        qWarning().noquote() << QString("Wrong message size for get News data item for user %1").arg(this->m_pUserConData->m_userName);
        return new MessageProtocol(OP_CODE_CMD_RES::ACK_GET_NEWS_DATA_ITEM, ERROR_CODE_WRONG_SIZE);
    }

    const quint32* pData = (quint32*)msg->getPointerToData();
    quint32        newsIndex;

    newsIndex = qFromLittleEndian(pData[0]);
    if (newsIndex != 0) {
        NewsData* pItem = (NewsData*)this->m_pGlobalData->m_fanclubNews.getItem(newsIndex);
        if (pItem != NULL) {
            QByteArray userName  = g_ListedUser->getReadableName(pItem->m_userID).toUtf8();
            QByteArray header    = pItem->m_itemName.toUtf8();
            QByteArray info      = pItem->m_newsText;
            quint32    totalSize = 20 + userName.length() + header.length() + info.length() + 2;

            char* pData = new char[totalSize];
            memset(pData, 0x0, totalSize);

            rCode = qToLittleEndian(ERROR_CODE_SUCCESS);
            memcpy(pData, &rCode, sizeof(qint32));

            newsIndex = qToLittleEndian(newsIndex);
            memcpy(&pData[4], &newsIndex, sizeof(quint32));

            qint64 timestamp = qToLittleEndian(pItem->m_timestamp);
            memcpy(&pData[8], &timestamp, sizeof(qint64));

            quint32 infoSize = qToLittleEndian(info.length());
            memcpy(&pData[16], &infoSize, sizeof(quint32));

            memcpy(&pData[20], userName.constData(), userName.length());
            memcpy(&pData[21 + userName.length()], header.constData(), header.length());
            memcpy(&pData[22 + userName.length() + header.length()], info.constData(), info.length());

            qInfo().noquote() << QString("User %1 got fanclub news item %2").arg(this->m_pUserConData->m_userName, pItem->m_itemName);

            MessageProtocol* ack = new MessageProtocol(OP_CODE_CMD_RES::ACK_GET_NEWS_DATA_ITEM, pData, totalSize);
            delete[] pData;
            return ack;
        }
    }

    return new MessageProtocol(OP_CODE_CMD_RES::ACK_GET_NEWS_DATA_ITEM, rCode);
}

MessageProtocol* DataConnection::requestDeleteNewsDataItem(MessageProtocol* msg)
{
    qint32 rCode = ERROR_CODE_NOT_FOUND;
    if (msg->getDataLength() != 4) {
        qWarning().noquote() << QString("Wrong message size for delete News data item for user %1").arg(this->m_pUserConData->m_userName);
        return new MessageProtocol(OP_CODE_CMD_RES::ACK_DEL_NEWS_DATA_ITEM, ERROR_CODE_WRONG_SIZE);
    }

    const quint32* pData = (quint32*)msg->getPointerToData();
    quint32        newsIndex;

    newsIndex = qFromLittleEndian(pData[0]);
    if (newsIndex != 0) {

        rCode = this->m_pGlobalData->m_fanclubNews.removeItem(newsIndex);
        if (rCode == ERROR_CODE_SUCCESS)
            qInfo().noquote() << QString("User %1 removed news data item with index %2").arg(this->m_pUserConData->m_userName).arg(newsIndex);
        else
            qWarning().noquote() << QString("User %1 Could not remove news data with index %2 : %3").arg(this->m_pUserConData->m_userName).arg(newsIndex).arg(rCode);
    }

    return new MessageProtocol(OP_CODE_CMD_RES::ACK_DEL_NEWS_DATA_ITEM, rCode);
}

#include "../Manager/cstatisticmanager.h"

MessageProtocol* DataConnection::requestCommandStatistic(MessageProtocol* msg)
{
    //    const char* pData = msg->getPointerToData();
    //    QByteArray  data(pData, (int)msg->getDataLength());

    //    QByteArray answer;
    //    qint32     rCode = g_StatisticManager.handleStatisticCommand(data, answer);
    //    qInfo().noquote() << QString("Handle statistics command from %1 with %2").arg(this->m_pUserConData->m_userName).arg(rCode);
    //    rCode = qToLittleEndian(rCode);
    //    answer.prepend(sizeof(qint32), 0x0);
    //    memcpy((void*)answer.constData(), &rCode, sizeof(qint32));

    //    return new MessageProtocol(OP_CODE_CMD_RES::ACK_CMD_STATISTIC, answer);
    MessageProtocol* ack = g_StatisticManager.handleStatisticCommand(this->m_pUserConData, msg);

    QByteArray data  = QByteArray(ack->getPointerToData());
    qint32     rCode = ERROR_CODE_SUCCESS;
    data.prepend(sizeof(qint32), 0x0);
    memcpy((void*)data.constData(), &rCode, sizeof(qint32));

    MessageProtocol* ack2 = new MessageProtocol(ack->getIndex(), data);
    delete ack;
    return ack2;
}
