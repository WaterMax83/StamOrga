#include <QtCore/QDataStream>

#include "dataconnection.h"
#include "../Common/General/globalfunctions.h"
#include "../Common/General/config.h"
#include "../Common/Network/messagecommand.h"
#include "../Data/seasonticket.h"

DataConnection::DataConnection(GlobalData *pGData, QObject *parent) : QObject(parent)
{
    this->m_pGlobalData = pGData;
}

/*
 * 0                Header          12
 * 12   String      password        X
 */
MessageProtocol *DataConnection::requestCheckUserLogin(MessageProtocol *msg)
{
    MessageProtocol *ack = NULL;
    const char *pData = msg->getPointerToData();
    quint16 size = qFromBigEndian(*((quint16 *)pData));
    if (size > msg->getDataLength())
        return new MessageProtocol(OP_CODE_CMD_RES::ACK_LOGIN_USER, ERROR_CODE_WRONG_SIZE);

    QString passw(QByteArray(pData + 2, size));
    if (this->m_pGlobalData->m_UserList.userCheckPassword(this->m_pUserConData->userName, passw)) {
        ack = new MessageProtocol(OP_CODE_CMD_RES::ACK_LOGIN_USER, ERROR_CODE_SUCCESS);
        this->m_pUserConData->bIsConnected = true;
        qInfo().noquote() << QString("User %1 logged in").arg(this->m_pUserConData->userName);
    }
    else {
        ack = new MessageProtocol(OP_CODE_CMD_RES::ACK_LOGIN_USER, ERROR_CODE_WRONG_PASSWORD);
        this->m_pUserConData->bIsConnected = false;
    }
    return ack;
}

/* Answer
 * 0                Header          12
 * 12               SUCCESS         4
 * 16               PROPS           4
 * 20               size            2
 * 22               readableName    X
 */
MessageProtocol *DataConnection::requestGetUserProperties()
{
    QByteArray answer;
    QDataStream wAnswer(&answer, QIODevice::WriteOnly);
    wAnswer.setByteOrder(QDataStream::BigEndian);
    wAnswer << ERROR_CODE_SUCCESS;
    wAnswer << this->m_pGlobalData->m_UserList.getUserProperties(this->m_pUserConData->userName);

    QString readableName = this->m_pGlobalData->m_UserList.getReadableName(this->m_pUserConData->userName);
    wAnswer << quint16(readableName.toUtf8().size());
    answer.append(readableName);

    MessageProtocol *ack = new MessageProtocol(OP_CODE_CMD_RES::ACK_GET_USER_PROPS, answer);

    return ack;
}

/*
 * 0                    Header          12
 * 12       quint16     size            2
 * 14       String      actual Passw    X
 * 14+X     quint16     size            2
 * 16+X     String      new Passw       Y
 */
MessageProtocol *DataConnection::requestUserChangeLogin(MessageProtocol *msg)
{
    if (msg->getDataLength() <= 8) {
        qWarning() << QString("Getting no user login data from %1").arg(this->m_pUserConData->userName);
        return NULL;
    }

    qint32 totalLength = (qint32)msg->getDataLength();
    const char *pData = msg->getPointerToData();

    quint16 actLength = qFromBigEndian(*((quint16 *)pData));
    if (actLength + 2 > totalLength)
        return new MessageProtocol(OP_CODE_CMD_RES::ACK_USER_CHANGE_LOGIN, ERROR_CODE_WRONG_SIZE);
    QString actPassw(QByteArray(pData + 2, actLength));

    quint16 newLength = qFromBigEndian(*((quint16 *)(pData + 2 + actLength)));
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
MessageProtocol *DataConnection::requestUserChangeReadname(MessageProtocol *msg)
{
    if (msg->getDataLength() <= 4) {
        qWarning() << QString("Getting no readname from %1").arg(this->m_pUserConData->userName);
        return NULL;
    }

    qint32 totalLength = (qint32)msg->getDataLength();
    const char *pData = msg->getPointerToData();

    quint16 actLength = qFromBigEndian(*((quint16 *)pData));
    if (actLength + 2 > totalLength)
        return new MessageProtocol(OP_CODE_CMD_RES::ACK_USER_CHANGE_LOGIN, ERROR_CODE_WRONG_SIZE);
    QString newReadName(QByteArray(pData + 2, actLength));

    if (!this->m_pGlobalData->m_UserList.userChangeReadName(this->m_pUserConData->userName, newReadName))
        return new MessageProtocol(OP_CODE_CMD_RES::ACK_USER_CHANGE_LOGIN, ERROR_CODE_COMMON);

    return new MessageProtocol(OP_CODE_CMD_RES::ACK_USER_CHANGE_READNAME, ERROR_CODE_SUCCESS);
}

/*
 * 0                Header          12
 * 12   quint32     version         4
 * 16   quint16     size            2
 * 16   String      version         X
 */
MessageProtocol *DataConnection::requestGetProgramVersion(MessageProtocol *msg)
{
    if (msg->getDataLength() <= 6) {
        qWarning() << QString("Getting no version data from %1").arg(this->m_pUserConData->userName);
        return NULL;
    }

    const char *pData = msg->getPointerToData();
    quint16 actLength = qFromBigEndian(*((quint16 *)(pData + 4)));
    if (actLength > msg->getDataLength())
        return new MessageProtocol(OP_CODE_CMD_RES::ACK_GET_VERSION,ERROR_CODE_WRONG_SIZE);
    QString remVersion(QByteArray(pData + 6, actLength));
    qInfo().noquote() << QString("Version from %1 = %2").arg(this->m_pUserConData->userName).arg(remVersion);
    QByteArray ownVersion;
    QDataStream wVersion(&ownVersion, QIODevice::WriteOnly);
    wVersion.setByteOrder(QDataStream::BigEndian);
    wVersion << (quint32)STAM_ORGA_VERSION_I;
    wVersion << quint16(QString(STAM_ORGA_VERSION_S).toUtf8().size());

    ownVersion.append(QString(STAM_ORGA_VERSION_S));
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
 * 32   QString     infoGame        X
 * 32+X qutin16     sizePack2       2
 */

#define GAMES_OFFSET    1 + 1 + 8           // sIndex + comp + datetime

MessageProtocol *DataConnection::requestGetGamesList(/*MessageProtocol *msg*/)
{
    QByteArray ackArray;
    QDataStream wAckArray(&ackArray, QIODevice::WriteOnly);
    wAckArray.setByteOrder(QDataStream::BigEndian);

    quint16 numbOfGames = this->m_pGlobalData->m_GamesList.startRequestGetGamesPlay();
    wAckArray << (quint32)ERROR_CODE_SUCCESS << numbOfGames;
    wAckArray << quint16(0x1);                                      //Version

    for (quint32 i=0; i<numbOfGames; i++) {
        GamesPlay *pGame = this->m_pGlobalData->m_GamesList.getRequestGamesPlay(i);
        if (pGame == NULL)
            continue;

        QString game(pGame->home + ";" + pGame->away +  ";" + pGame->score);


        wAckArray.device()->seek(ackArray.size());
        wAckArray << quint16(game.toUtf8().size() + GAMES_OFFSET);
        wAckArray << quint8(pGame->saisonIndex);
        wAckArray << quint8(pGame->competition);
        wAckArray << pGame->datetime;

        ackArray.append(game);
    }


    this->m_pGlobalData->m_GamesList.stopRequestGetGamesPlay();

    return new MessageProtocol(OP_CODE_CMD_RES::ACK_GET_GAMES_LIST, ackArray);
}

/*
 * 0                Header          12
 * 12     quint32     result          4
 * 16     quint16     numbOfTickets   2
 * 18     quint16     version         2
 * 20     quint16     sizeTick1       2
 * 22     quint8      discount        1
 * 25     QString     name+place      X
 * 25+X   qutin16     sizeTick2       2
 */

#define TICKET_OFFSET    1          // discount

MessageProtocol *DataConnection::requestGetTicketsList(/*MessageProtocol *msg*/)
{
    QByteArray ackArray;
    QDataStream wAckArray(&ackArray, QIODevice::WriteOnly);
    wAckArray.setByteOrder(QDataStream::BigEndian);

    quint16 numbOfTickets = this->m_pGlobalData->m_SeasonTicket.startRequestGetTicketInfoList();
    wAckArray << (quint32)ERROR_CODE_SUCCESS << numbOfTickets;
    wAckArray << quint16(0x1);                                      //Version

    for (quint32 i=0; i<numbOfTickets; i++) {
        TicketInfo *pTicket = this->m_pGlobalData->m_SeasonTicket.getRequestTicketInfo(i);
        if (pTicket == NULL)
            continue;

        QString ticket(pTicket->name + ";" + pTicket->place);


        wAckArray.device()->seek(ackArray.size());
        wAckArray << quint16(ticket.toUtf8().size() + TICKET_OFFSET);
        wAckArray << quint8(pTicket->discount);

        ackArray.append(ticket);
    }


    this->m_pGlobalData->m_SeasonTicket.stopRequestGetTicketInfoList();

    return new MessageProtocol(OP_CODE_CMD_RES::ACK_GET_TICKETS_LIST, ackArray);
}

/*
 * 0                Header          12
 * 12   quint32     discount        4
 * 16   quint16     size            2
 * 18   String      name            X
 */
MessageProtocol *DataConnection::requestAddSeasonTicket(MessageProtocol *msg)
{
    if (msg->getDataLength() <= 6) {
        qWarning() << QString("Message for adding season ticket to short for user %1").arg(this->m_pUserConData->userName);
        return NULL;
    }

    const char *pData = msg->getPointerToData();
    quint32 discount = qFromBigEndian(*((quint32 *)(pData)));
    quint16 actLength = qFromBigEndian(*((quint16 *)(pData + 4)));
    if (actLength > msg->getDataLength())
        return new MessageProtocol(OP_CODE_CMD_RES::ACK_ADD_TICKET, ERROR_CODE_WRONG_SIZE);
    QString ticketName(QByteArray(pData + 6, actLength));
//    qInfo().noquote() << QString("Version from %1 = %2").arg(this->m_pUserConData->userName).arg(remVersion);

    if (this->m_pGlobalData->m_SeasonTicket.addNewSeasonTicket(this->m_pUserConData->userName, ticketName, discount) > 0)
        return new MessageProtocol(OP_CODE_CMD_RES::ACK_ADD_TICKET, ERROR_CODE_SUCCESS);
    return new MessageProtocol(OP_CODE_CMD_RES::ACK_ADD_TICKET, ERROR_CODE_COMMON);
}

/*
 * 0                Header          12
 * 12   quint16     size            2
 * 14   String      name            X
 */
MessageProtocol *DataConnection::requestRemoveSeasonTicket(MessageProtocol *msg)
{
    if (msg->getDataLength() <= 2) {
        qWarning() << QString("Message for removing season ticket to short for user %1").arg(this->m_pUserConData->userName);
        return NULL;
    }

    const char *pData = msg->getPointerToData();
    quint16 actLength = qFromBigEndian(*((quint16 *)(pData + 2)));
    if (actLength > msg->getDataLength())
        return new MessageProtocol(OP_CODE_CMD_RES::ACK_REMOVE_TICKET, ERROR_CODE_WRONG_SIZE);
    QString ticketName(QByteArray(pData + 2, actLength));

    if (this->m_pGlobalData->m_SeasonTicket.removeTicket(ticketName) == ERROR_CODE_SUCCESS)
        return new MessageProtocol(OP_CODE_CMD_RES::ACK_REMOVE_TICKET, ERROR_CODE_SUCCESS);
    return new MessageProtocol(OP_CODE_CMD_RES::ACK_REMOVE_TICKET, ERROR_CODE_COMMON);
}
