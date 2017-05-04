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

#include "../Common/General/config.h"
#include "../Common/General/globalfunctions.h"
#include "../Common/Network/messagecommand.h"
#include "../Data/seasonticket.h"
#include "dataconnection.h"

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
    quint16     size  = qFromBigEndian(*((quint16*)pData));
    qint32      result;
    if (size > msg->getDataLength())
        return new MessageProtocol(OP_CODE_CMD_RES::ACK_LOGIN_USER, ERROR_CODE_WRONG_SIZE);

    QString passw(QByteArray(pData + 2, size));
    if (this->m_pGlobalData->m_UserList.userCheckPassword(this->m_pUserConData->userName, passw)) {
        result                             = ERROR_CODE_SUCCESS;
        this->m_pUserConData->bIsConnected = true;
        qInfo().noquote() << QString("User %1 logged in").arg(this->m_pUserConData->userName);
    } else {
        result                             = ERROR_CODE_WRONG_PASSWORD;
        this->m_pUserConData->bIsConnected = false;
        qWarning().noquote() << QString("User %1 tried to login with wrong password").arg(this->m_pUserConData->userName);
    }

    return new MessageProtocol(OP_CODE_CMD_RES::ACK_LOGIN_USER, result);
}

/* Answer
 * 0                Header          12
 * 12               SUCCESS         4
 * 16               PROPS           4
 * 20               index           4
 * 24               size            2
 * 26               readableName    X
 */
MessageProtocol* DataConnection::requestGetUserProperties()
{
    QByteArray  answer;
    QDataStream wAnswer(&answer, QIODevice::WriteOnly);
    wAnswer.setByteOrder(QDataStream::BigEndian);
    wAnswer << ERROR_CODE_SUCCESS;
    wAnswer << this->m_pGlobalData->m_UserList.getUserProperties(this->m_pUserConData->userName);
    wAnswer << this->m_pGlobalData->m_UserList.getItemIndex(this->m_pUserConData->userName);

    QString readableName = this->m_pGlobalData->m_UserList.getReadableName(this->m_pUserConData->userName);
    wAnswer << quint16(readableName.toUtf8().size());
    answer.append(readableName);

    qInfo().noquote() << QString("User %1 getting user properties %2").arg(this->m_pUserConData->userName, readableName);

    MessageProtocol* ack = new MessageProtocol(OP_CODE_CMD_RES::ACK_GET_USER_PROPS, answer);

    return ack;
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
        qWarning() << QString("Getting no user login data from %1").arg(this->m_pUserConData->userName);
        return NULL;
    }

    qint32      totalLength = (qint32)msg->getDataLength();
    const char* pData       = msg->getPointerToData();

    quint16 actLength = qFromBigEndian(*((quint16*)pData));
    if (actLength + 2 > totalLength)
        return new MessageProtocol(OP_CODE_CMD_RES::ACK_USER_CHANGE_LOGIN, ERROR_CODE_WRONG_SIZE);
    QString actPassw(QByteArray(pData + 2, actLength));

    quint16 newLength = qFromBigEndian(*((quint16*)(pData + 2 + actLength)));
    if (newLength + actLength + 2 + 2 > totalLength)
        return new MessageProtocol(OP_CODE_CMD_RES::ACK_USER_CHANGE_LOGIN, ERROR_CODE_WRONG_SIZE);
    QString newPassw(QByteArray(pData + 2 + actLength + 2, newLength));

    if (!this->m_pGlobalData->m_UserList.userCheckPassword(this->m_pUserConData->userName, actPassw))
        return new MessageProtocol(OP_CODE_CMD_RES::ACK_USER_CHANGE_LOGIN, ERROR_CODE_WRONG_PASSWORD);

    if (this->m_pGlobalData->m_UserList.userChangePassword(this->m_pUserConData->userName, newPassw))
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
        qWarning() << QString("Getting no readname from %1").arg(this->m_pUserConData->userName);
        return NULL;
    }

    qint32      totalLength = (qint32)msg->getDataLength();
    const char* pData       = msg->getPointerToData();

    quint16 actLength = qFromBigEndian(*((quint16*)pData));
    if (actLength + 2 > totalLength)
        return new MessageProtocol(OP_CODE_CMD_RES::ACK_USER_CHANGE_LOGIN, ERROR_CODE_WRONG_SIZE);
    QString newReadName(QByteArray(pData + 2, actLength));

    if (!this->m_pGlobalData->m_UserList.userChangeReadName(this->m_pUserConData->userName, newReadName))
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
        qWarning() << QString("Getting no version data from %1").arg(this->m_pUserConData->userName);
        return NULL;
    }

    const char* pData     = msg->getPointerToData();
    quint16     actLength = qFromBigEndian(*((quint16*)(pData + 4)));
    if (actLength > msg->getDataLength())
        return new MessageProtocol(OP_CODE_CMD_RES::ACK_GET_VERSION, ERROR_CODE_WRONG_SIZE);
    QString remVersion(QByteArray(pData + 6, actLength));
    qInfo().noquote() << QString("Version from %1 = %2").arg(this->m_pUserConData->userName).arg(remVersion);
    QByteArray  ownVersion;
    QDataStream wVersion(&ownVersion, QIODevice::WriteOnly);
    wVersion.setByteOrder(QDataStream::BigEndian);
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


    return new MessageProtocol(OP_CODE_CMD_RES::ACK_GET_VERSION, ownVersion);
}


/*
 * 0                Header          12
 * 12   quint32     result          4
 * 16   quint16     version         2
 * 16   quint16     numbofGames     2
 * 20   quint16     sizePack1       2
 * 22   quint8      sIndex          1
 * 23   quint8      comp            1
 * 24   quint64     datetime        8
 * 32   quint32     index           4
 * 36   QString     infoGame        X
 * 36+X qutin16     sizePack2       2
 */

#define GAMES_OFFSET (1 + 1 + 8 + 4) // sIndex + comp + datetime + index

MessageProtocol* DataConnection::requestGetGamesList(/*MessageProtocol *msg*/)
{
    QByteArray  ackArray;
    QDataStream wAckArray(&ackArray, QIODevice::WriteOnly);
    wAckArray.setByteOrder(QDataStream::BigEndian);

    quint16 numbOfGames = this->m_pGlobalData->m_GamesList.startRequestGetItemList();
    wAckArray << (quint32)ERROR_CODE_SUCCESS;
    wAckArray << quint16(0x1) << numbOfGames; //Version

    for (quint32 i = 0; i < numbOfGames; i++) {
        GamesPlay* pGame = (GamesPlay*)(this->m_pGlobalData->m_GamesList.getRequestConfigItem(i));
        if (pGame == NULL)
            continue;

        QString game(pGame->m_itemName + ";" + pGame->away + ";" + pGame->score);


        wAckArray.device()->seek(ackArray.size());
        wAckArray << quint16(game.toUtf8().size() + GAMES_OFFSET);
        wAckArray << quint8(pGame->m_saisonIndex);
        wAckArray << quint8(pGame->m_competition);
        wAckArray << pGame->m_timestamp;
        wAckArray << pGame->m_index;

        ackArray.append(game);
    }


    this->m_pGlobalData->m_GamesList.stopRequestGetItemList();

    qInfo().noquote() << QString("User %1 request Games List with %2 entries").arg(this->m_pUserConData->userName).arg(numbOfGames);

    return new MessageProtocol(OP_CODE_CMD_RES::ACK_GET_GAMES_LIST, ackArray);
}

/*
 * 0                Header          12
 * 12     quint32     result          4
 * 16     quint16     version         2
 * 18     quint16     numbOfTickets   2
 * 20     quint16     sizeTick1       2
 * 22     quint8      discount        1
 * 23     quint32     index           4
 * 27     quint32     userIndex       4
 * 31     QString     name+place      X
 * 31+X   qutin16     sizeTick2       2
 */

#define TICKET_OFFSET (1 + 4 + 4) // discount + isOwnUser + index + userIndex

MessageProtocol* DataConnection::requestGetTicketsList(/*MessageProtocol *msg*/)
{
    QByteArray  ackArray;
    QDataStream wAckArray(&ackArray, QIODevice::WriteOnly);
    wAckArray.setByteOrder(QDataStream::BigEndian);

    quint16 numbOfTickets = this->m_pGlobalData->m_SeasonTicket.startRequestGetItemList();
    wAckArray << (quint32)ERROR_CODE_SUCCESS;
    wAckArray << quint16(0x1) << numbOfTickets; //Version

    for (quint32 i = 0; i < numbOfTickets; i++) {
        TicketInfo* pTicket = (TicketInfo*)(this->m_pGlobalData->m_SeasonTicket.getRequestConfigItem(i));
        if (pTicket == NULL)
            continue;

        QString ticket(pTicket->m_itemName + ";" + pTicket->place);


        wAckArray.device()->seek(ackArray.size());
        wAckArray << quint16(ticket.toUtf8().size() + TICKET_OFFSET);
        wAckArray << quint8(pTicket->discount);
        wAckArray << pTicket->m_index;
        wAckArray << pTicket->userIndex;

        ackArray.append(ticket);
    }

    qInfo().noquote() << QString("User %1 request Ticket List with %2 entries").arg(this->m_pUserConData->userName).arg(numbOfTickets);

    this->m_pGlobalData->m_SeasonTicket.stopRequestGetItemList();

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
        qWarning() << QString("Message for adding season ticket to short for user %1").arg(this->m_pUserConData->userName);
        return NULL;
    }

    const char* pData     = msg->getPointerToData();
    quint32     discount  = qFromBigEndian(*((quint32*)(pData)));
    quint16     actLength = qFromBigEndian(*((quint16*)(pData + 4)));
    if (actLength > msg->getDataLength())
        return new MessageProtocol(OP_CODE_CMD_RES::ACK_ADD_TICKET, ERROR_CODE_WRONG_SIZE);
    QString ticketName(QByteArray(pData + 6, actLength));
    //    qInfo().noquote() << QString("Version from %1 = %2").arg(this->m_pUserConData->userName).arg(remVersion);

    QString userName  = this->m_pUserConData->userName;
    qint32  userIndex = this->m_pGlobalData->m_UserList.getItemIndex(userName);
    qint32  rCode     = this->m_pGlobalData->m_SeasonTicket.addNewSeasonTicket(userName, userIndex, ticketName, discount);
    if (rCode > ERROR_CODE_NO_ERROR) {
        qInfo().noquote() << QString("User %1 added SeasonTicket %2")
                                 .arg(this->m_pUserConData->userName)
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
        qWarning() << QString("Wrong message size for removing season ticket for user %1").arg(this->m_pUserConData->userName);
        return new MessageProtocol(OP_CODE_CMD_RES::ACK_REMOVE_TICKET, ERROR_CODE_WRONG_SIZE);
    }

    const char* pData = msg->getPointerToData();
    quint32     index = qFromBigEndian(*((quint32*)pData));

    if ((rCode = this->m_pGlobalData->m_SeasonTicket.removeItem(index)) == ERROR_CODE_SUCCESS) {
        qInfo().noquote() << QString("User %1 removed SeasonTicket %2")
                                 .arg(this->m_pUserConData->userName)
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
MessageProtocol* DataConnection::requestNewPlaceSeasonTicket(MessageProtocol* msg)
{
    qint32 rCode;
    if (msg->getDataLength() <= 6) {
        qWarning() << QString("Wrong message size for new place season ticket for user %1").arg(this->m_pUserConData->userName);
        return new MessageProtocol(OP_CODE_CMD_RES::ACK_REMOVE_TICKET, ERROR_CODE_WRONG_SIZE);
    }

    const char* pData     = msg->getPointerToData();
    quint32     index     = qFromBigEndian(*((quint32*)pData));
    quint16     actLength = qFromBigEndian(*((quint16*)(pData + 4)));
    if (actLength > msg->getDataLength())
        return new MessageProtocol(OP_CODE_CMD_RES::ACK_ADD_TICKET, ERROR_CODE_WRONG_SIZE);
    QString newPlace(QByteArray(pData + 6, actLength));

    if ((rCode = this->m_pGlobalData->m_SeasonTicket.changePlaceFromTicket(index, newPlace)) == ERROR_CODE_SUCCESS) {
        qInfo().noquote() << QString("User %1 changed SeasonTicket place to %2")
                                 .arg(this->m_pUserConData->userName)
                                 .arg(newPlace);
        return new MessageProtocol(OP_CODE_CMD_RES::ACK_NEW_TICKET_PLACE, ERROR_CODE_SUCCESS);
    }
    return new MessageProtocol(OP_CODE_CMD_RES::ACK_NEW_TICKET_PLACE, rCode);
}
