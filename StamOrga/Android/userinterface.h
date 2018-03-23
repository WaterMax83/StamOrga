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

#ifndef USERINTERFACE_H
#define USERINTERFACE_H

#include <QtCore/QObject>

#include "../../Common/General/globalfunctions.h"
#include "../Data/globaldata.h"
//#include "../connectionhandling.h"

class UserInterface : public QObject
{
    Q_OBJECT

    //    Q_PROPERTY(GlobalData* globalData READ globalData WRITE setGlobalData)
public:
    explicit UserInterface(QObject* parent = 0);

    Q_INVOKABLE qint32 startMainConnection(QString name, QString passw);

    Q_INVOKABLE qint32 startGetUserEvents();

    Q_INVOKABLE qint32 startSetUserEvents(qint64 eventID, qint32 status);

    Q_INVOKABLE qint32 startListGettingGames();

    Q_INVOKABLE qint32 startListGettingGamesInfo();

    Q_INVOKABLE qint32 startSetFixedGameTime(const quint32 gameIndex, const quint32 fixed);

    Q_INVOKABLE qint32 startAddSeasonTicket(QString name, quint32 discount);

    Q_INVOKABLE qint32 startRemoveSeasonTicket(quint32 index);

    Q_INVOKABLE qint32 startEditSeasonTicket(quint32 index, QString name, QString place, quint32 discount);

    Q_INVOKABLE qint32 startListSeasonTickets();

    Q_INVOKABLE qint32 startRequestAvailableTickets(quint32 gameIndex);

    Q_INVOKABLE qint32 startChangeAvailableTicketState(quint32 ticketIndex, quint32 gameIndex, quint32 state, QString name = "");

    Q_INVOKABLE qint32 startChangeGame(const quint32 index, const quint32 sIndex, const QString competition,
                                       const QString home, const QString away, const QString date, const QString score);

    Q_INVOKABLE qint32 startSaveMeetingInfo(const quint32 gameIndex, const QString when, const QString where, const QString info,
                                            const quint32 type);

    Q_INVOKABLE qint32 startLoadMeetingInfo(const quint32 gameIndex, const quint32 type);

    Q_INVOKABLE qint32 startAcceptMeetingInfo(const quint32 gameIndex, const quint32 accept,
                                              const QString name, const quint32 type,
                                              const quint32 acceptIndex = 0);

    Q_INVOKABLE qint32 startChangeFanclubNews(const quint32 newsIndex, const QString header,
                                              const QString info);

    Q_INVOKABLE qint32 startListFanclubNews();

    Q_INVOKABLE qint32 startGetFanclubNewsItem(const quint32 newsIndex);

    Q_INVOKABLE qint32 startDeleteFanclubNewsItem(const quint32 newsIndex);

    Q_INVOKABLE qint32 startStatisticsCommand(const QByteArray& command);

    Q_INVOKABLE bool isDebuggingEnabled()
    {
#ifdef QT_DEBUG
        return true;
#else
        return false;
#endif
    }

    Q_INVOKABLE bool isDeviceMobile()
    {
#if defined(Q_OS_ANDROID) || defined(Q_OS_IOS)
        return true;
#else
        return false;
#endif
    }

    Q_INVOKABLE QString getErrorCodeToString(qint32 code)
    {
        return getErrorCodeString(code);
    }

    //    GlobalData* globalData()
    //    {
    //        return this->m_pConHandle->getGlobalData();
    //    }
    //    void setGlobalData(GlobalData* pData)
    //    {
    //        this->m_pConHandle->setGlobalData(pData);
    //    }

signals:
    void notifyConnectionFinished(qint32 result, const QString msg);
    void notifyVersionRequestFinished(qint32 result);
    void notifyUserPropertiesFinished(qint32 result);
    void notifyUpdatePasswordRequestFinished(qint32 result);
    void notifyUpdateReadableNameRequest(qint32 result);
    void notifyGamesListFinished(qint32 result);
    void notifyGamesInfoListFinished(qint32 result);
    void notifySetGamesFixedTimeFinished(qint32 result);
    void notifySeasonTicketAddFinished(qint32 result);
    void notifySeasonTicketRemoveFinished(qint32 result);
    void notifySeasonTicketEditFinished(qint32 result);
    void notifySeasonTicketListFinished(qint32 result);
    void notifyAvailableTicketStateChangedFinished(qint32 result);
    void notifyAvailableTicketListFinsished(qint32 result);
    void notifyChangedGameFinished(qint32 result);
    void notifyChangedMeetingInfoFinished(qint32 result);
    void notifyChangedAwayTripInfoFinished(qint32 result);
    void notifyLoadMeetingInfoFinished(qint32 result);
    void notifyLoadAwayTripInfoFinished(qint32 result);
    void notifyAcceptMeetingFinished(qint32 result);
    void notifyAcceptAwayTripFinished(qint32 result);
    void notifyChangeNewsDataFinished(qint32 result);
    void notifyFanclubNewsListFinished(qint32 result);
    void notifyGetFanclubNewsItemFinished(qint32 result);
    void notifyDeleteFanclubNewsItemFinished(qint32 result);
    void notifyGetUserEvents(qint32 result);
    void notifyStatisticsCommandFinished(qint32 result);

public slots:
    void slotConnectionRequestFinished(qint32 result, const QString msg);

    void slotCommandFinished(quint32 command, qint32 result);


private:
    QMutex m_notifyMutex;
};

#endif // USERINTERFACE_H
