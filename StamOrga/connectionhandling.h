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

#ifndef CONNECTIONHANDLING_H
#define CONNECTIONHANDLING_H

#include <QtCore/QObject>
#include <QtCore/QTimer>

#include "../Common/General/backgroundcontroller.h"
#include "../Data/globaldata.h"
#include "dataconnection.h"
#include "mainconnection.h"


class ConnectionHandling : public QObject
{
    Q_OBJECT
public:
    explicit ConnectionHandling(QObject* parent = 0);
    ~ConnectionHandling();

    qint32 startMainConnection(QString name, QString passw);
    qint32 startGettingVersionInfo();
    qint32 startGettingUserProps();
    bool startUpdatePassword(QString newPassWord);
    qint32 startUpdateReadableName(QString name);
    qint32 startListGettingGames();
    qint32 startRemoveSeasonTicket(quint32 index);
    qint32 startNewPlaceSeasonTicket(quint32 index, QString place);
    qint32 startAddSeasonTicket(QString name, quint32 discount);
    qint32 startListSeasonTickets();
    qint32 startChangeSeasonTicketState(quint32 tickedIndex, quint32 gameIndex, quint32 state, QString name);
    qint32 startListAvailableTicket(quint32 gameIndex);


    void setGlobalData(GlobalData* pData)
    {
        if (pData != NULL)
            this->m_pGlobalData = pData;
    }

    GlobalData* getGlobalData()
    {
        return this->m_pGlobalData;
    }

    //    ConnectionInfo *GetConnectionInfo() { return &this->m_conInfo; }

signals:
    void sNotifyConnectionFinished(qint32 result);
    void sNotifyVersionRequest(qint32 result, QString msg);
    //    void sNotifyUserPropertiesRequest(qint32 result);
    void sNotifyUpdatePasswordRequest(qint32 result, QString newPassWord);
    //    void sNotifyUpdateReadableNameRequest(qint32 result);
    //    void sNotifyGamesListRequest(qint32 result);
    //    void sNotifySeasonTicketRemoveRequest(quint32 result);
    //    void sNotifySeasonTicketAddRequest(quint32 result);
    //    void sNotifySeasonTicketListRequest(quint32 result);
    //    void sNotifySeasonTicketNewPlace(quint32 result);
    //    void sNotityAvailableTicketStateChangeRequest(qint32 result);
    //    void sNotityAvailableTicketListRequest(qint32 result);

    void sNotifyCommandFinished(quint32 command, qint32 result);

    void sStartSendMainConRequest(QString name);

    void sStartSendNewRequest(DataConRequest request);

public slots:

private slots:
    void slMainConReqFin(qint32 result, const QString& msg);

    void slDataConLastRequestFinished(DataConRequest request);

    //    void slTimerConResetFired();
    //    void slTimerConLoginFired();

private:
    BackgroundController m_ctrlMainCon;
    MainConnection*      m_pMainCon = NULL;

    BackgroundController m_ctrlDataCon;
    DataConnection*      m_pDataCon = NULL;

    GlobalData* m_pGlobalData = NULL;

    //    QTimer* m_pTimerConReset;
    //    QTimer* m_pTimerLoginReset;
    qint64 m_lastSuccessTimeStamp;

    QList<DataConRequest> m_lErrorMainCon;

    void sendLoginRequest(QString password);
    void sendNewRequest(DataConRequest request);

    void checkTimeoutResult(qint32 result);

    void startDataConnection();
    void stopDataConnection();
    bool isDataConnectionActive() { return this->m_ctrlDataCon.IsRunning(); }
    bool isMainConnectionActive() { return this->m_ctrlMainCon.IsRunning(); }
};

#endif // CONNECTIONHANDLING_H
