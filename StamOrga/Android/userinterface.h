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
#include "../connectionhandling.h"

class UserInterface : public QObject
{
    Q_OBJECT

    Q_PROPERTY(GlobalData* globalData READ globalData WRITE setGlobalData)
public:
    explicit UserInterface(QObject* parent = 0);

    Q_INVOKABLE qint32 startMainConnection(QString name, QString passw);

    Q_INVOKABLE qint32 startListGettingGames();

    Q_INVOKABLE qint32 startUpdateUserPassword(QString newPassw);

    Q_INVOKABLE qint32 startUpdateReadableName(QString name);

    Q_INVOKABLE qint32 startAddSeasonTicket(QString name, quint32 discount);

    Q_INVOKABLE qint32 startRemoveSeasonTicket(quint32 index);

    Q_INVOKABLE qint32 startNewPlaceSeasonTicket(quint32 index, QString place);

    Q_INVOKABLE qint32 startListSeasonTickets();

    Q_INVOKABLE qint32 startRequestAvailableTickets(quint32 gameIndex);

    Q_INVOKABLE qint32 startFreeAvailableTicket(quint32 ticketIndex, quint32 gameIndex);

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
#ifdef Q_OS_ANDROID
        return true;
#else
        return false;
#endif
    }

    Q_INVOKABLE QString getErrorCodeToString(qint32 code)
    {
        return getErrorCodeString(code);
    }

    GlobalData* globalData()
    {
        return this->m_pConHandle->getGlobalData();
    }
    void setGlobalData(GlobalData* pData)
    {
        this->m_pConHandle->setGlobalData(pData);
    }

signals:
    void notifyConnectionFinished(qint32 result);
    void notifyVersionRequestFinished(qint32 result, QString msg);
    void notifyUserPropertiesFinished(qint32 result);
    void notifyUpdatePasswordRequestFinished(qint32 result, QString newPassWord);
    void notifyUpdateReadableNameRequest(qint32 result);
    void notifyGamesListFinished(qint32 result);
    void notifySeasonTicketAddFinished(qint32 result);
    void notifySeasonTicketRemoveFinished(qint32 result);
    void notifySeasonTicketNewPlaceFinished(qint32 result);
    void notifySeasonTicketListFinished(qint32 result);
    void notifyAvailableTicketFreeFinsished(qint32 result);
    void notifyAvailableTicketListFinsished(qint32 result);

public slots:
    void slConnectionRequestFinished(qint32 result);
    void slVersionRequestFinished(qint32 result, QString msg);
    void slUserPropertiesFinished(qint32 result);
    void slUpdatePasswordRequestFinished(qint32 result, QString newPassWord);
    void slUpdateReadableNameRequestFinished(qint32 result);
    void slGettingGamesListFinished(qint32 result);
    void slSeasonTicketAddFinished(qint32 result);
    void slSeasonTicketRemoveFinished(qint32 result);
    void slSeasonTicketNewPlaceFinished(qint32 result);
    void slSeasonTicketListFinished(qint32 result);
    void slAvailableTicketFreeFinished(qint32 result);
    void slAvailableTicketListFinished(qint32 result);


private:
    ConnectionHandling* m_pConHandle;
};

#endif // USERINTERFACE_H
