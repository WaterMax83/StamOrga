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
#include <QtCore/QDir>
#include <QtCore/QFileInfo>
#include <QtCore/QJsonArray>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>
#include <QtCore/QtEndian>

#include "../Common/General/globalfunctions.h"
#include "globaldata.h"
#include "pushnotification.h"

GlobalData::GlobalData()
{
    this->m_initalized = false;
}

void GlobalData::initialize()
{
    QString userSetDirPath = getUserHomeConfigPath() + "/Settings/";

    QDir userSetDir(userSetDirPath + "AvailableTickets/");

    QStringList nameFilter;

    nameFilter << "Tickets_Game_*.ini";
    QStringList infoConfigList = userSetDir.entryList(nameFilter, QDir::Files | QDir::Readable);
    foreach (QString file, infoConfigList) {
        AvailableGameTickets* ticket = new AvailableGameTickets();

        if (ticket->initialize(userSetDir.path() + "/" + file) >= 0) {
            quint16 totalCount = ticket->getNumberOfInternalList();

            for (int i = 0; i < totalCount; i++) {
                AvailableTicketInfo* info = (AvailableTicketInfo*)ticket->getRequestConfigItemFromListIndex(i);
                if (info == NULL)
                    continue;
                TicketInfo* tInfo = (TicketInfo*)this->m_SeasonTicket.getItem(info->m_ticketID);
                if (tInfo == NULL)
                    ticket->removeItem(info->m_index); /* Ticket is no longer present, remove it */
            }
            this->m_availableTickets.append(ticket);
        } else
            delete ticket;
    }

    nameFilter.clear();

    userSetDir.setPath(userSetDirPath + "Meetings/");
    nameFilter << "Meetings_Game_*.ini";
    infoConfigList = userSetDir.entryList(nameFilter, QDir::Files | QDir::Readable);
    foreach (QString file, infoConfigList) {
        MeetingInfo* mInfo = new MeetingInfo();

        if (mInfo->initialize(userSetDir.path() + "/" + file) >= 0)
            this->m_meetingInfos.append(mInfo);
        else
            delete mInfo;
    }

    userSetDir.setPath(userSetDirPath + "AwayTrips/");
    nameFilter << "AwayTrips_Game_*.ini";
    infoConfigList = userSetDir.entryList(nameFilter, QDir::Files | QDir::Readable);
    foreach (QString file, infoConfigList) {
        AwayTripInfo* mInfo = new AwayTripInfo();

        if (mInfo->initialize(userSetDir.path() + "/" + file) >= 0)
            this->m_awayTripInfos.append(mInfo);
        else
            delete mInfo;
    }


    userSetDir.setPath(userSetDirPath + "UserEvents/");
    nameFilter << "Event_*.ini";
    infoConfigList = userSetDir.entryList(nameFilter, QDir::Files | QDir::Readable);
    foreach (QString file, infoConfigList) {
        UserEvents* mEvent = new UserEvents();

        if (mEvent->initialize(userSetDir.path() + "/" + file) >= 0)
            this->m_userEvents.append(mEvent);
        else
            delete mEvent;
    }

    this->m_initalized = true;
}

qint32 GlobalData::requestChangeStateSeasonTicket(const quint32 ticketIndex, const quint32 gameIndex,
                                                  const quint32 newState, const QString reserveName,
                                                  const qint32 userID, qint64& messageID)
{
    if (!this->m_initalized)
        return ERROR_CODE_NOT_READY;

    QMutexLocker lock(&this->m_globalDataMutex);

    GamesPlay*  pGame   = (GamesPlay*)this->m_GamesList.getItem(gameIndex);
    TicketInfo* pTicket = (TicketInfo*)this->m_SeasonTicket.getItem(ticketIndex);
    if (pGame == NULL || pTicket == NULL)
        return ERROR_CODE_NOT_FOUND;

    if (newState == TICKET_STATE_NOT_POSSIBLE)
        return ERROR_CODE_NOT_FOUND;

#undef ENABLE_PAST_CHECK
#ifndef QT_DEBUG
#define ENABLE_PAST_CHECK
#endif
#ifdef ENABLE_PAST_CHECK
    if (pGame->m_timestamp < QDateTime::currentMSecsSinceEpoch())
        return ERROR_CODE_IN_PAST;
#endif

    qint32 result = ERROR_CODE_SUCCESS;
    foreach (AvailableGameTickets* ticket, this->m_availableTickets) {
        if (ticket->getGameIndex() == gameIndex) {
            quint32 currentState = ticket->getTicketState(ticketIndex);
            QString currentName  = ticket->getTicketName(ticketIndex);
            if (currentState == newState && currentName == reserveName)
                qInfo().noquote() << QString("Ticket %1 at game %3 already has state %4 and name %2").arg(pTicket->m_itemName, currentName).arg(pGame->m_index).arg(currentState);
            else if (newState == TICKET_STATE_FREE || newState == TICKET_STATE_BLOCKED) {
                if (currentState == TICKET_STATE_NOT_POSSIBLE) /* Not found, add new */
                    result = ticket->addNewTicket(ticketIndex, userID, newState);
                else /* ticket found, just change actual state */
                    result = ticket->changeTicketState(ticketIndex, userID, newState);
                if (newState == TICKET_STATE_FREE) {
                    QString body = QString("Karte \'%1\' beim Spiel %2 : %3 ist frei").arg(pTicket->m_itemName, pGame->m_itemName, pGame->m_away);
                    messageID    = g_pushNotify->sendNewTicketNotification(body, userID, gameIndex, ticketIndex);
                } else
                    messageID = g_pushNotify->removeNewTicketNotification(gameIndex, ticketIndex);
            } else if (newState == TICKET_STATE_RESERVED) {
                if (currentState == TICKET_STATE_FREE || currentState == TICKET_STATE_RESERVED) {
                    result    = ticket->changeTicketState(ticketIndex, userID, newState, reserveName);
                    messageID = g_pushNotify->removeNewTicketNotification(gameIndex, ticketIndex);
                } else
                    result = ERROR_CODE_NOT_POSSIBLE;
                /* Anything else is not possible */
            } else
                result = ERROR_CODE_NOT_POSSIBLE;

            if (result == ERROR_CODE_SUCCESS)
                qInfo().noquote() << QString("Changed ticketState from %1 at game %2 to %3").arg(pTicket->m_itemName).arg(pGame->m_index).arg(newState);
            else
                qWarning().noquote() << QString("Error setting ticket state %1: %2").arg(newState).arg(result);
            return result;
        }
    }

    if (newState != TICKET_STATE_FREE) {
        qWarning().noquote() << QString("Error setting ticket state %1: %2").arg(newState).arg("Not possible because it does not exist");
        return ERROR_CODE_NOT_POSSIBLE;
    }

    AvailableGameTickets* ticket = new AvailableGameTickets();
    if (ticket->initialize(pGame->m_saison, pGame->m_competition, pGame->m_saisonIndex, pGame->m_index)) {
        this->m_availableTickets.append(ticket);
        result = ticket->addNewTicket(ticketIndex, userID, TICKET_STATE_FREE, reserveName);
        qInfo().noquote() << QString("Changed ticketState from %1 at game %2 to %3").arg(pTicket->m_itemName).arg(pGame->m_index).arg(TICKET_STATE_FREE);
        QString body = QString("Karte \'%1\' beim Spiel %2 : %3 ist frei").arg(pTicket->m_itemName, pGame->m_itemName, pGame->m_away);
        messageID    = g_pushNotify->sendNewTicketNotification(body, userID, gameIndex, ticketIndex);
    } else {
        delete ticket;
        qWarning().noquote() << QString("Error creating available ticket file for game %1").arg(pGame->m_index);
        return ERROR_CODE_NOT_POSSIBLE;
    }

    return ERROR_CODE_SUCCESS;
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
qint32 GlobalData::requestGetAvailableSeasonTicket(const quint32 gameIndex, const QString userName, QByteArray& data)
{
    if (!this->m_initalized)
        return ERROR_CODE_NOT_READY;

    QMutexLocker lock(&this->m_globalDataMutex);

    GamesPlay* pGame = (GamesPlay*)this->m_GamesList.getItem(gameIndex);
    if (pGame == NULL)
        return ERROR_CODE_NOT_FOUND;

    Q_UNUSED(userName);

    foreach (AvailableGameTickets* ticket, this->m_availableTickets) {
        if (ticket->getGameIndex() == gameIndex) {
            quint16 totalCount = ticket->getNumberOfInternalList();

            QByteArray  freeTickets;
            QDataStream wFreeTickets(&freeTickets, QIODevice::WriteOnly);
            wFreeTickets.setByteOrder(QDataStream::LittleEndian);

            QByteArray  reservedTickets;
            QDataStream wReserveds(&reservedTickets, QIODevice::WriteOnly);
            wReserveds.setByteOrder(QDataStream::LittleEndian);

            quint16 freeTicktetCount    = 0;
            quint16 reservedTicketCount = 0;
            for (int i = 0; i < totalCount; i++) {
                AvailableTicketInfo* info = (AvailableTicketInfo*)ticket->getRequestConfigItemFromListIndex(i);
                if (info == NULL) {
                    wFreeTickets << quint32(0x0);
                    continue;
                }
                TicketInfo* tInfo = (TicketInfo*)this->m_SeasonTicket.getItem(info->m_ticketID);
                if (tInfo == NULL) {
                    ticket->removeItem(info->m_index);
                    continue; /* Ticket is no longer present, remove it */
                }
                if (info->m_state == TICKET_STATE_FREE) {
                    wFreeTickets << quint32(info->m_ticketID);
                    freeTicktetCount++;
                } else if (info->m_state == TICKET_STATE_RESERVED) {
                    wReserveds.device()->seek(reservedTickets.size());
                    wReserveds << quint32(info->m_ticketID);
                    reservedTickets.append(info->m_itemName);
                    reservedTickets.append(char(0x00));
                    reservedTicketCount++;
                }
            }

            QDataStream wData(&data, QIODevice::WriteOnly);
            wData.setByteOrder(QDataStream::LittleEndian);

            wData << quint32(ERROR_CODE_SUCCESS) << quint16(freeTicktetCount) << quint16(reservedTicketCount);
            data.append(freeTickets);
            data.append(reservedTickets);

            qInfo().noquote() << QString("User %1 got available SeasonTicket List for game %2:%3:%4")
                                     .arg(userName)
                                     .arg(gameIndex)
                                     .arg(pGame->m_competition)
                                     .arg(pGame->m_saisonIndex);

            return ERROR_CODE_SUCCESS;
        }
    }

    QDataStream wData(&data, QIODevice::WriteOnly);
    wData.setByteOrder(QDataStream::LittleEndian);

    wData << quint32(ERROR_CODE_SUCCESS) << quint16(0x0) << quint16(0x0);

    qInfo().noquote() << QString("User %1 got available SeasonTicket List for game %2:%3:%4 with no entries")
                             .arg(userName)
                             .arg(gameIndex)
                             .arg(pGame->m_competition)
                             .arg(pGame->m_saisonIndex);

    return ERROR_CODE_SUCCESS;
}

quint16 GlobalData::getTicketNumber(const quint32 gamesIndex, const quint32 state)
{
    if (!this->m_initalized)
        return ERROR_CODE_NOT_READY;

    QMutexLocker lock(&this->m_globalDataMutex);

    GamesPlay* pGame = (GamesPlay*)this->m_GamesList.getItem(gamesIndex);
    if (pGame == NULL)
        return 0;

    foreach (AvailableGameTickets* ticket, this->m_availableTickets) {
        if (ticket->getGameIndex() == gamesIndex)
            return ticket->getTicketNumber(state);
    }
    return 0;
}

quint16 GlobalData::getAcceptedNumber(const quint32 gamesIndex, const quint32 state)
{
    if (!this->m_initalized)
        return ERROR_CODE_NOT_READY;

    QMutexLocker lock(&this->m_globalDataMutex);

    GamesPlay* pGame = (GamesPlay*)this->m_GamesList.getItem(gamesIndex);
    if (pGame == NULL)
        return 0;

    foreach (MeetingInfo* info, this->m_meetingInfos) {
        if (info->getGameIndex() == gamesIndex)
            return info->getAcceptedNumber(state);
    }
    return 0;
}

quint16 GlobalData::getMeetingInfoValue(const quint32 gamesIndex)
{
    if (!this->m_initalized)
        return ERROR_CODE_NOT_READY;

    QMutexLocker lock(&this->m_globalDataMutex);

    GamesPlay* pGame = (GamesPlay*)this->m_GamesList.getItem(gamesIndex);
    if (pGame == NULL)
        return 0;

    foreach (MeetingInfo* info, this->m_meetingInfos) {
        if (info->getGameIndex() == gamesIndex) {
            return 1;
        }
    }
    return 0;
}

#define BODY_CHANGE_MEET "Beim Spiel %1 : %2 wurde das Treffen geändert"
#define BODY_ADD_MEETING "Kommst du zu %1 : %2 ?"

qint32 GlobalData::requestChangeMeetingInfo(const quint32 gameIndex, const quint32 version,
                                            const QString when, const QString where, const QString info,
                                            const qint32 userID, const quint32 type, qint64& messageID)
{
    if (!this->m_initalized)
        return ERROR_CODE_NOT_READY;

    QMutexLocker lock(&this->m_globalDataMutex);

    Q_UNUSED(version);
    GamesPlay* pGame = (GamesPlay*)this->m_GamesList.getItem(gameIndex);
    if (pGame == NULL)
        return ERROR_CODE_NOT_FOUND;

#ifdef ENABLE_PAST_CHECK
    if (pGame->m_timestamp < QDateTime::currentMSecsSinceEpoch())
        return ERROR_CODE_IN_PAST;
#endif
    QList<MeetingInfo*>* pList;
    if (type == MEETING_TYPE_MEETING)
        pList = &this->m_meetingInfos;
    else
        pList = (QList<MeetingInfo*>*)&this->m_awayTripInfos;

    qint32 result = ERROR_CODE_SUCCESS;
    for (int i = 0; i < pList->size(); i++) {
        MeetingInfo* mInfo = pList->at(i);
        if (mInfo->getGameIndex() == gameIndex) {

            /* Check old infos if this only exist someone accepted it */
            QString oldWhen, oldWhere, oldInfo;
            mInfo->getMeetingInfo(oldWhen, oldWhere, oldInfo);

            result = mInfo->changeMeetingInfo(when, where, info);
            if (result == ERROR_CODE_SUCCESS) {
                qInfo().noquote() << QString("Changed Meeting info at game %1:%2, %3").arg(pGame->m_itemName, pGame->m_away).arg(type);

                /* When there were no infos saved, this was also a new meeting */
                if (oldWhen.length() > 0 || oldWhere.length() > 0 || oldInfo.length() > 0) {
                    QString body = QString(BODY_CHANGE_MEET).arg(pGame->m_itemName, pGame->m_away);
                    messageID    = g_pushNotify->sendChangeMeetingNotification(body, userID, gameIndex);
                } else {
                    QString body = QString(BODY_ADD_MEETING).arg(pGame->m_itemName, pGame->m_away);
                    messageID    = g_pushNotify->sendNewMeetingNotification(body, userID, gameIndex);
                }
            } else
                qWarning().noquote() << QString("Error setting meeting info at game %1: %2").arg(pGame->m_index).arg(result);
            return result;
        }
    }

    MeetingInfo* mInfo;
    if (type == MEETING_TYPE_MEETING)
        mInfo = new MeetingInfo();
    else
        mInfo = new AwayTripInfo();

    if (mInfo->initialize(pGame->m_saison, pGame->m_competition, pGame->m_saisonIndex, pGame->m_index)) {
        pList->append(mInfo);
        result = mInfo->changeMeetingInfo(when, where, info);
        qInfo().noquote() << QString("Added MeetingInfo at game %1:%2, %3").arg(pGame->m_itemName, pGame->m_away).arg(pGame->m_index);
        QString body = QString(BODY_ADD_MEETING).arg(pGame->m_itemName, pGame->m_away);
        messageID    = g_pushNotify->sendNewMeetingNotification(body, userID, gameIndex);
    } else {
        delete mInfo;
        qWarning().noquote() << QString("Error creating meeting info file for game %1").arg(pGame->m_index);
        return ERROR_CODE_NOT_POSSIBLE;
    }

    return ERROR_CODE_SUCCESS;
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
qint32 GlobalData::requestGetMeetingInfo(const quint32 gameIndex, const quint32 version, char* pData, const quint32 type, quint32& size)
{
    if (!this->m_initalized)
        return ERROR_CODE_NOT_READY;

    QMutexLocker lock(&this->m_globalDataMutex);

    Q_UNUSED(version);
    GamesPlay* pGame = (GamesPlay*)this->m_GamesList.getItem(gameIndex);
    if (pGame == NULL)
        return ERROR_CODE_NOT_FOUND;

    QString              when;
    QString              where;
    QString              info;
    qint32               result = ERROR_CODE_NOT_FOUND;
    QList<MeetingInfo*>* pList;
    if (type == MEETING_TYPE_MEETING)
        pList = &this->m_meetingInfos;
    else
        pList = (QList<MeetingInfo*>*)&this->m_awayTripInfos;

    MeetingInfo* mInfo = NULL;
    for (int i = 0; i < pList->size(); i++) {
        MeetingInfo* mi = pList->at(i);
        if (mi->getGameIndex() == gameIndex) {
            result = mi->getMeetingInfo(when, where, info);
            mInfo  = mi;
            break;
        }
    }
    if (result != ERROR_CODE_SUCCESS)
        return result;

    quint32    offset = 0, gIndex;
    QByteArray tmpA;
    memset(pData, 0x0, size);

    result = qToLittleEndian(result);
    memcpy(pData + offset, &result, sizeof(quint32));
    offset += sizeof(quint32);

    gIndex = qToLittleEndian(gameIndex);
    memcpy(pData + offset, &gIndex, sizeof(quint32));
    offset += sizeof(quint32);

    tmpA = when.toUtf8();
    memcpy(pData + offset, tmpA.constData(), tmpA.size());
    offset += tmpA.size() + 1;

    tmpA = where.toUtf8();
    memcpy(pData + offset, tmpA.constData(), tmpA.size());
    offset += tmpA.size() + 1;

    tmpA = info.toUtf8();
    memcpy(pData + offset, tmpA.constData(), tmpA.size());
    offset += tmpA.size() + 1;

    quint32 tmp;
    qint32  acceptCount = mInfo->getNumberOfInternalList();
    for (int i = 0; i < acceptCount; i++) {
        AcceptMeetingInfo* ami = (AcceptMeetingInfo*)mInfo->getRequestConfigItemFromListIndex(i);

        tmp = qToLittleEndian(ami->m_index);
        memcpy(pData + offset, &tmp, sizeof(quint32));
        offset += sizeof(quint32);

        tmp = qToLittleEndian(ami->m_state);
        memcpy(pData + offset, &tmp, sizeof(quint32));
        offset += sizeof(quint32);

        tmp = qToLittleEndian(ami->m_userID);
        memcpy(pData + offset, &tmp, sizeof(quint32));
        offset += sizeof(quint32);

        tmpA = ami->m_itemName.toUtf8();
        memcpy(pData + offset, tmpA.constData(), tmpA.size());
        offset += tmpA.size() + 1;
    }

    size = offset;

    return ERROR_CODE_SUCCESS;
}


#define BODY_NEW_AWAY_ACCEPT "%1 fährt zu %2, kommst du mit?"

/*  answer
 * 0                Header          12
 * 12   quint32     result          4
 */
qint32 GlobalData::requestAcceptMeetingInfo(const quint32 gameIndex, const quint32 version,
                                            const quint32 acceptValue, const quint32 acceptIndex,
                                            const QString name, const qint32 userID, qint64& messageID)
{
    if (!this->m_initalized)
        return ERROR_CODE_NOT_READY;

    QMutexLocker lock(&this->m_globalDataMutex);

    Q_UNUSED(version);
    GamesPlay* pGame = (GamesPlay*)this->m_GamesList.getItem(gameIndex);
    if (pGame == NULL)
        return ERROR_CODE_NOT_FOUND;

#ifdef ENABLE_PAST_CHECK
    if (pGame->m_timestamp < QDateTime::currentMSecsSinceEpoch())
        return ERROR_CODE_IN_PAST;
#endif

    qint32 result = ERROR_CODE_SUCCESS;
    foreach (MeetingInfo* mInfo, this->m_meetingInfos) {
        if (mInfo->getGameIndex() == gameIndex) {
            if (acceptIndex == 0)
                result = mInfo->addNewAcceptation(acceptValue, userID, name);
            else
                result = mInfo->changeAcceptation(acceptIndex, acceptValue, userID, name);
            if (result == ERROR_CODE_SUCCESS) {
                qInfo().noquote() << QString("Changed Acceptation of %2 at game %1").arg(pGame->m_index).arg(name);
                /* Send an info if the first person is going to a game which is not at home */
                if (pGame->m_away == "KSC" && acceptValue == ACCEPT_STATE_ACCEPT && mInfo->getAcceptedNumber(ACCEPT_STATE_ACCEPT) == 1) {
                    QString body = QString(BODY_NEW_AWAY_ACCEPT).arg(name, pGame->m_itemName);
                    messageID    = g_pushNotify->sendNewFirstAwayAccept(body, userID, gameIndex);
                }
            } else
                qWarning().noquote() << QString("Error setting Acceptation at game %1: %2").arg(pGame->m_index).arg(result);
            return result;
        }
    }

    MeetingInfo* mInfo = new MeetingInfo();
    if (mInfo->initialize(pGame->m_saison, pGame->m_competition, pGame->m_saisonIndex, pGame->m_index)) {
        this->m_meetingInfos.append(mInfo);
        result = mInfo->addNewAcceptation(acceptValue, userID, name);
        qInfo().noquote() << QString("Changed Acceptation of %2 at game %1").arg(pGame->m_index).arg(name);

        /* Send an info if the first person is going to an game which is not at home */
        if (pGame->m_away == "KSC" && acceptValue == ACCEPT_STATE_ACCEPT) {
            QString body = QString(BODY_NEW_AWAY_ACCEPT).arg(name, pGame->m_itemName);
            messageID    = g_pushNotify->sendNewFirstAwayAccept(body, userID, gameIndex);
        }
    } else {
        delete mInfo;
        qWarning().noquote() << QString("Error creating meeting info file for game %1").arg(pGame->m_index);
        return ERROR_CODE_NOT_POSSIBLE;
    }

    return ERROR_CODE_SUCCESS;
}

qint32 GlobalData::addNewUserEvent(const QString type, const QString info, const qint32 userID)
{
    /* We need userevents with same type and info */
    //    for (int i = 0; i < this->m_userEvents.size(); i++) {
    //        if (this->m_userEvents[i]->getType() == type) {
    //            if (this->m_userEvents[i]->getInfo() == info)
    //                return ERROR_CODE_SUCCESS;
    //        }
    //    }

    UserEvents* pUserEvent = new UserEvents();
    if (pUserEvent->initialize(type, info, userID) == ERROR_CODE_SUCCESS)
        this->m_userEvents.append(pUserEvent);
    else
        delete pUserEvent;

    return ERROR_CODE_SUCCESS;
}

qint32 GlobalData::getCurrentUserEvents(QByteArray& destArray, const qint32 userID)
{
    QJsonArray jsArr;
    for (int i = 0; i < this->m_userEvents.size(); i++) {
        if (this->m_userEvents[i]->getHasUserGotEvent(userID))
            continue;

        /*
         * transport information
         */
        QJsonObject json;
        json.insert("type", this->m_userEvents[i]->getType());
        json.insert("info", this->m_userEvents[i]->getInfo());
        json.insert("id", this->m_userEvents[i]->getEventID());

        jsArr.append(json);
    }

    QJsonObject jsRootObj;
    jsRootObj.insert("version", "1.0.0");
    jsRootObj.insert("events", jsArr);

    QJsonDocument jsDoc(jsRootObj);
    destArray = jsDoc.toJson(QJsonDocument::Compact);

    return ERROR_CODE_SUCCESS;
}

qint32 GlobalData::acceptUserEvent(const qint64 eventID, const qint32 userID, const qint32 status)
{
    foreach (UserEvents* pEvent, this->m_userEvents) {
        if (pEvent->getEventID() == eventID) {
            pEvent->addNewUser(userID);
            break;
        }
    }

    Q_UNUSED(status);

    return ERROR_CODE_SUCCESS;
}
