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
#include "../Common/General/cgendisposer.h"
//#include "../Data/globaldata.h"
#include "ccontcpdata.h"
#include "ccontcpmain.h"


class cConManager : public cGenDisposer
{
    Q_OBJECT
public:
    explicit cConManager(QObject* parent = 0);
    ~cConManager();

    qint32 initialize();

    qint32 startMainConnection(QString name, QString passw);
    //    qint32 startGettingUserEvents();
    //    qint32 startSettingUserEvents(qint64 eventID, qint32 status);
    //    bool startUpdatePassword(QString newPassWord);
    //    qint32 startUpdateReadableName(QString name);
    //    qint32 startListGettingGames();
    //    qint32 startListGettingGamesInfo();
    //    qint32 startSetFixedGameTime(const quint32 gameIndex, const quint32 fixedTime);
    //    qint32 startRemoveSeasonTicket(quint32 index);
    //    qint32 startEditSeasonTicket(quint32 index, QString name, QString place, quint32 discount);
    //    qint32 startAddSeasonTicket(QString name, quint32 discount);
    //    qint32 startListSeasonTickets();
    //    qint32 startChangeSeasonTicketState(quint32 tickedIndex, quint32 gameIndex, quint32 state, QString name);
    //    qint32 startListAvailableTicket(quint32 gameIndex);
    //    qint32 startChangeGame(const quint32 index, const quint32 sIndex, const QString competition,
    //                           const QString home, const QString away, const QString date, const QString score);
    //    qint32 startSaveMeetingInfo(const quint32 gameIndex, const QString when, const QString where, const QString info,
    //                                const quint32 type);
    //    qint32 startLoadMeetingInfo(const quint32 gameIndex, const quint32 type);
    //    qint32 startAcceptMeetingInfo(const quint32 gameIndex, const quint32 accept,
    //                                  const QString name, const quint32 type,
    //                                  const quint32 acceptIndex = 0);

    //    qint32 startChangeFanclubNews(const quint32 newsIndex, const QString header,
    //                                  const QString info);
    //    qint32 startListFanclubNews();
    //    qint32 startGetFanclubNewsItem(const quint32 newsIndex);
    //    qint32 startDeleteFanclubNewsItem(const quint32 newsIndex);
    //    qint32 startStatisticsCommand(const QByteArray& command);


    //    void setGlobalData(GlobalData* pData)
    //    {
    //        if (pData != NULL)
    //            this->m_pGlobalData = pData;
    //    }

    //    GlobalData* getGlobalData()
    //    {
    //        return this->m_pGlobalData;
    //    }

    //    ConnectionInfo *GetConnectionInfo() { return &this->m_conInfo; }

    void sendLoginRequest();
    void sendNewRequest(TcpDataConRequest* request);

signals:
    void signalNotifyConnectionFinished(const qint32 result, const QString msg);
    //    void sNotifyVersionRequest(qint32 result, QString msg);
    //    //    void sNotifyUserPropertiesRequest(qint32 result);
    //    void sNotifyUpdatePasswordRequest(qint32 result, QString newPassWord);

    void signalNotifyCommandFinished(quint32 command, qint32 result);

    //    void sStartSendMainConRequest(QString name);

    //    void sSendNewBindingPortRequest();

    void signalStartSendNewRequest(TcpDataConRequest* request);

public slots:

private slots:
    void slMainConReqFin(qint32 result, const QString msg, const QString salt, const QString random);
    void slotDataConnnectionFinished(qint32 result, const QString msg);

    void slotDataConLastRequestFinished(TcpDataConRequest* request);

private:
    BackgroundController m_ctrlMainCon;
    cConTcpMain*         m_pMainCon = NULL;

    BackgroundController m_ctrlDataCon;
    cConTcpData*         m_pDataCon = NULL;

    bool m_bIsConnecting = false;

    //    GlobalData* m_pGlobalData = NULL;

    //    qint64 m_lastSuccessTimeStamp;

    QList<TcpDataConRequest*> m_lRequestConError;


    void checkTimeoutResult(qint32 result);

    QString m_mainConRequestUserName;
    QString m_mainConRequestPassWord;
    QString m_mainConRequestSalt;
    QString m_mainConRequestRandom;
    quint16 m_mainConRequestDataPort;

    void startDataConnection();
    void stopDataConnection();
    bool isDataConnectionActive() { return this->m_ctrlDataCon.IsRunning(); }
    bool isMainConnectionActive() { return this->m_ctrlMainCon.IsRunning(); }
};

extern cConManager g_ConManager;

#endif // CONNECTIONHANDLING_H
