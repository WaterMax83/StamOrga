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

    this->m_currentSeason = getSeasonFromTimeStamp(QDateTime::currentMSecsSinceEpoch());

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
                if (tInfo == NULL || tInfo->isTicketRemoved())
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
    nameFilter.clear();
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

qint32 GlobalData::requestChangeStateSeasonTicket(const qint32 ticketIndex, const qint32 gameIndex,
                                                  const qint32 newState, const QString reserveName,
                                                  const qint32 userID, qint64& messageID)
{
    if (!this->m_initalized)
        return ERROR_CODE_NOT_READY;

    QMutexLocker lock(&this->m_globalDataMutex);

    GamesPlay*  pGame   = (GamesPlay*)this->m_GamesList.getItem(gameIndex);
    TicketInfo* pTicket = (TicketInfo*)this->m_SeasonTicket.getItem(ticketIndex);
    if (pGame == NULL || pTicket == NULL || pTicket->isTicketRemoved())
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
            qint32  currentState = ticket->getTicketState(ticketIndex);
            QString currentName  = ticket->getTicketName(ticketIndex);
            if (currentState == newState && currentName == reserveName)
                qInfo().noquote() << QString("Ticket %1 at game %3 already has state %4 and name %2").arg(pTicket->m_itemName, currentName).arg(pGame->m_index).arg(currentState);
            else if (newState == TICKET_STATE_FREE || newState == TICKET_STATE_BLOCKED) {
                if (currentState == TICKET_STATE_NOT_POSSIBLE) /* Not found, add new */
                    result = ticket->addNewTicket(ticketIndex, userID, newState);
                else /* ticket found, just change actual state */
                    result = ticket->changeTicketState(ticketIndex, userID, newState);
                if (newState == TICKET_STATE_FREE) {
                    messageID    = g_pushNotify->sendNewTicketNotification(pGame, userID, ticketIndex);
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
    if (ticket->initialize(pGame->m_season, pGame->m_competition, pGame->m_seasonIndex, pGame->m_index)) {
        this->m_availableTickets.append(ticket);
        ticket->addNewTicket(ticketIndex, userID, TICKET_STATE_FREE, reserveName);
        qInfo().noquote() << QString("Changed ticketState from %1 at game %2 to %3").arg(pTicket->m_itemName).arg(pGame->m_index).arg(TICKET_STATE_FREE);
        messageID    = g_pushNotify->sendNewTicketNotification(pGame, userID, ticketIndex);
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
qint32 GlobalData::requestGetAvailableSeasonTicket(const qint32 gameIndex, const QString userName, QByteArray& data)
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
                if (tInfo == NULL || tInfo->isTicketRemoved()) {
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
                                     .arg(pGame->m_seasonIndex);

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
                             .arg(pGame->m_seasonIndex);

    return ERROR_CODE_SUCCESS;
}

qint32 GlobalData::requestGetAvailableTicketFromUser(const qint32 userID, QJsonArray& arrTickets)
{
    if (!this->m_initalized)
        return ERROR_CODE_NOT_READY;

    QMutexLocker lock(&this->m_globalDataMutex);

    qint64 currentTimeStamp = QDateTime::currentMSecsSinceEpoch();
    foreach (AvailableGameTickets* pAvTicket, this->m_availableTickets) {
        GamesPlay* pGame = (GamesPlay*)this->m_GamesList.getItem(pAvTicket->getGameIndex());
        if (pGame == NULL || pGame->m_timestamp < currentTimeStamp) // past is not important
            continue;

        for (int i = 0; i < pAvTicket->getNumberOfInternalList(); i++) {
            AvailableTicketInfo* pTicketInfo = (AvailableTicketInfo*)pAvTicket->getRequestConfigItemFromListIndex(i);
            if (pTicketInfo == NULL || pTicketInfo->m_state == TICKET_STATE_BLOCKED)
                continue;

            QJsonObject gameObj;
            /* First check if user reserved this ticket */
            if (pTicketInfo->m_userID == userID && pTicketInfo->m_state == TICKET_STATE_RESERVED) {
                gameObj.insert("type", "reserved");
            } else { /* Then check if this users card is not blocked */
                TicketInfo* pTicket = (TicketInfo*)this->m_SeasonTicket.getItem(pTicketInfo->m_ticketID);
                if (pTicket == NULL || pTicket->isTicketRemoved() || pTicket->m_userIndex != userID) // only tickets from this user
                    continue;
                gameObj.insert("type", "free");
            }

            gameObj.insert("gameIndex", (qint32)pGame->m_index);
            arrTickets.append(gameObj);
            break;
        }
    }

    return ERROR_CODE_SUCCESS;
}

quint16 GlobalData::getTicketNumber(const qint32 gamesIndex, const qint32 state)
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

quint16 GlobalData::getAcceptedNumber(const qint32 type, const qint32 gamesIndex, const qint32 state)
{
    if (!this->m_initalized)
        return ERROR_CODE_NOT_READY;

    QMutexLocker lock(&this->m_globalDataMutex);

    GamesPlay* pGame = (GamesPlay*)this->m_GamesList.getItem(gamesIndex);
    if (pGame == NULL)
        return 0;

    quint16 rValue = 0;
    if (type == MEETING_TYPE_MEETING) {
        foreach (MeetingInfo* info, this->m_meetingInfos) {
            if (info->getGameIndex() == gamesIndex) {
                rValue += info->getAcceptedNumber(state);
                break;
            }
        }
    } else if (type == MEETING_TYPE_AWAYTRIP) {
        foreach (MeetingInfo* info, this->m_awayTripInfos) {
            if (info->getGameIndex() == gamesIndex) {
                rValue += info->getAcceptedNumber(state);
                break;
            }
        }
    }
    return rValue;
}

quint16 GlobalData::getMeetingInfoValue(const qint32 type, const qint32 gamesIndex)
{
    if (!this->m_initalized)
        return ERROR_CODE_NOT_READY;

    QMutexLocker lock(&this->m_globalDataMutex);

    GamesPlay* pGame = (GamesPlay*)this->m_GamesList.getItem(gamesIndex);
    if (pGame == NULL)
        return 0;

    if (type == MEETING_TYPE_MEETING) {
        foreach (MeetingInfo* info, this->m_meetingInfos) {
            if (info->getGameIndex() == gamesIndex) {
                QString sInfo, when, where;
                info->getMeetingInfo(when, where, sInfo);
                if (!when.isEmpty() || !where.isEmpty() || !sInfo.isEmpty())
                    return 1;
                if (info->getAcceptedNumber(ACCEPT_STATE_ACCEPT) > 0)
                    return 1;
                if (info->getAcceptedNumber(ACCEPT_STATE_MAYBE) > 0)
                    return 1;
                break;
            }
        }
    } else if (type == MEETING_TYPE_AWAYTRIP) {
        foreach (MeetingInfo* info, this->m_awayTripInfos) {
            if (info->getGameIndex() == gamesIndex) {
                QString sInfo, when, where;
                info->getMeetingInfo(when, where, sInfo);
                if (!when.isEmpty() || !where.isEmpty() || !sInfo.isEmpty())
                    return 1;
                if (info->getAcceptedNumber(ACCEPT_STATE_ACCEPT) > 0)
                    return 1;
                if (info->getAcceptedNumber(ACCEPT_STATE_MAYBE) > 0)
                    return 1;
                break;
            }
        }
    }
    return 0;
}

qint32 GlobalData::requestChangeMeetingInfo(const qint32 gameIndex, const qint32 version,
                                            const QString when, const QString where, const QString info,
                                            const qint32 userID, const qint32 type, qint64& messageID)
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
    QString bigText;
    bigText.append(QString("\nWo: %1").arg(where));
    bigText.append(QString("\nWann: %1").arg(when));
    bigText.append(QString("\nInfo: %1\n").arg(info));
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
                    messageID = g_pushNotify->sendChangeMeetingNotification(pGame, bigText, userID, type);
                } else {
                    messageID    = g_pushNotify->sendNewMeetingNotification(pGame, bigText, userID, type);
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

    if (mInfo->initialize(pGame->m_season, pGame->m_competition, pGame->m_seasonIndex, pGame->m_index)) {
        pList->append(mInfo);
        mInfo->changeMeetingInfo(when, where, info);
        qInfo().noquote() << QString("Added MeetingInfo at game %1:%2, %3").arg(pGame->m_itemName, pGame->m_away).arg(pGame->m_index);

        messageID    = g_pushNotify->sendNewMeetingNotification(pGame, bigText, userID, type);
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
qint32 GlobalData::requestGetMeetingInfo(const qint32 gameIndex, const qint32 version, char* pData, const qint32 type, qint32& size)
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


#define BODY_NEW_AWAY_ACCEPT "%1 fährt nach %2 am %3\nKommst du auch mit?"

/*  answer
 * 0                Header          12
 * 12   quint32     result          4
 */
qint32 GlobalData::requestAcceptMeetingInfo(const qint32 gameIndex, const qint32 version,
                                            const qint32 acceptValue, const qint32 acceptIndex,
                                            const QString name, const qint32 userID, const qint32 type,
                                            qint64& messageID)
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
            if (acceptIndex == 0)
                result = mInfo->addNewAcceptation(acceptValue, userID, name);
            else
                result = mInfo->changeAcceptation(acceptIndex, acceptValue, userID, name);
            if (result == ERROR_CODE_SUCCESS) {
                qInfo().noquote() << QString("Changed Acceptation of %2 at game %1").arg(pGame->m_index).arg(name);
                /* Send an info if the first person is going to a game which is not at home */
                if (type == MEETING_TYPE_AWAYTRIP && pGame->m_away == "KSC" && acceptValue == ACCEPT_STATE_ACCEPT && mInfo->getAcceptedNumber(ACCEPT_STATE_ACCEPT) == 1) {
                    QString date = QDateTime::fromMSecsSinceEpoch(pGame->m_timestamp).toString("dd.MM.yy");
                    QString bigText = QString(BODY_NEW_AWAY_ACCEPT).arg(name, pGame->m_itemName, date);
                    messageID    = g_pushNotify->sendNewFirstAwayAccept(bigText, userID, gameIndex);
                }
            } else
                qWarning().noquote() << QString("Error setting Acceptation at game %1: %2").arg(pGame->m_index).arg(result);
            return result;
        }
    }

    MeetingInfo* mInfo;
    if (type == MEETING_TYPE_MEETING)
        mInfo = new MeetingInfo();
    else
        mInfo = new AwayTripInfo();
    if (mInfo->initialize(pGame->m_season, pGame->m_competition, pGame->m_seasonIndex, pGame->m_index)) {
        pList->append(mInfo);
        mInfo->addNewAcceptation(acceptValue, userID, name);
        qInfo().noquote() << QString("Changed Acceptation of %2 at game %1").arg(pGame->m_index).arg(name);

        /* Send an info if the first person is going to an game which is not at home */
        if (type == MEETING_TYPE_AWAYTRIP && pGame->m_away == "KSC" && acceptValue == ACCEPT_STATE_ACCEPT) {
            QString date = QDateTime::fromMSecsSinceEpoch(pGame->m_timestamp).toString("dd.MM.yy");
            QString bigText = QString(BODY_NEW_AWAY_ACCEPT).arg(name, pGame->m_itemName, date);
            messageID    = g_pushNotify->sendNewFirstAwayAccept(bigText, userID, gameIndex);
        }
    } else {
        delete mInfo;
        qWarning().noquote() << QString("Error creating meeting info file for game %1").arg(pGame->m_index);
        return ERROR_CODE_NOT_POSSIBLE;
    }

    return ERROR_CODE_SUCCESS;
}

qint32 GlobalData::requestSendCommentMeeting(const qint32 gameIndex, const qint32 type, const qint32 userID,
                                             const QString comment, qint64& messageID)
{
    if (!this->m_initalized)
        return ERROR_CODE_NOT_READY;

    QMutexLocker lock(&this->m_globalDataMutex);

    GamesPlay* pGame = (GamesPlay*)this->m_GamesList.getItem(gameIndex);
    if (pGame == NULL)
        return ERROR_CODE_NOT_FOUND;

    QList<MeetingInfo*>* pList;
    if (type == MEETING_TYPE_MEETING)
        pList = &this->m_meetingInfos;
    else
        pList = (QList<MeetingInfo*>*)&this->m_awayTripInfos;

//    QString userName  = this->m_UserList.getReadableName(userID);
    qint64  timestamp = QDateTime::currentMSecsSinceEpoch();
    qint32  result    = ERROR_CODE_SUCCESS;
    for (int i = 0; i < pList->size(); i++) {
        MeetingInfo* mInfo = pList->at(i);
        if (mInfo->getGameIndex() == gameIndex) {
            result = mInfo->addMeetingComment(userID, timestamp, comment);
            if (result == ERROR_CODE_SUCCESS) {
                qInfo().noquote() << QString("Added Comment %2 at game %1").arg(pGame->m_index).arg(comment);

                messageID    = g_pushNotify->sendNewMeetingComment(comment, userID, pGame);
            } else
                qWarning().noquote() << QString("Error adding comment at game %1: %2").arg(pGame->m_index).arg(result);
            return result;
        }
    }

    MeetingInfo* mInfo;
    if (type == MEETING_TYPE_MEETING)
        mInfo = new MeetingInfo();
    else
        mInfo = new AwayTripInfo();
    if (mInfo->initialize(pGame->m_season, pGame->m_competition, pGame->m_seasonIndex, pGame->m_index)) {
        pList->append(mInfo);
        result = mInfo->addMeetingComment(userID, timestamp, comment);
        qInfo().noquote() << QString("Added Comment %2 at game %1").arg(pGame->m_index).arg(comment);

        messageID    = g_pushNotify->sendNewMeetingComment(comment, userID, pGame);
    } else {
        delete mInfo;
        qWarning().noquote() << QString("Error creating meeting info file for game %1").arg(pGame->m_index);
        return ERROR_CODE_NOT_POSSIBLE;
    }

    return result;
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

    QMutexLocker lock(&this->m_globalDataMutex);

    UserEvents* pUserEvent = new UserEvents();
    if (pUserEvent->initialize(type, info, userID) == ERROR_CODE_SUCCESS)
        this->m_userEvents.append(pUserEvent);
    else
        delete pUserEvent;

    return ERROR_CODE_SUCCESS;
}

qint32 GlobalData::getCurrentUserEvents(QJsonArray& destArray, const qint32 userID)
{
    QMutexLocker lock(&this->m_globalDataMutex);

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

        destArray.append(json);
    }

    //    QJsonObject jsRootObj;
    //    jsRootObj.insert("version", "1.0.0");
    //    jsRootObj.insert("events", jsArr);

    //    QJsonDocument jsDoc(jsRootObj);
    //    destArray = jsDoc.toJson(QJsonDocument::Compact);

    return ERROR_CODE_SUCCESS;
}

qint32 GlobalData::acceptUserEvent(const qint64 eventID, const qint32 userID, const qint32 status)
{
    QMutexLocker lock(&this->m_globalDataMutex);

    foreach (UserEvents* pEvent, this->m_userEvents) {
        if (pEvent->getEventID() == eventID) {
            pEvent->addNewUser(userID);
            break;
        }
    }

    Q_UNUSED(status);

    return ERROR_CODE_SUCCESS;
}
