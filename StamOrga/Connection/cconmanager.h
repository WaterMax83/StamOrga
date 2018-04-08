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
    //    qint32 startListAvailableTicket(quint32 gameIndex);
    //    qint32 startSaveMeetingInfo(const quint32 gameIndex, const QString when, const QString where, const QString info,
    //                                const quint32 type);
    //    qint32 startAcceptMeetingInfo(const quint32 gameIndex, const quint32 accept,
    //                                  const QString name, const quint32 type,
    //                                  const quint32 acceptIndex = 0);

    //    ConnectionInfo *GetConnectionInfo() { return &this->m_conInfo; }

    void sendLoginRequest();
    void sendNewRequest(TcpDataConRequest* request);

signals:
    void signalNotifyConnectionFinished(const qint32 result, const QString msg);
    void signalNotifyCommandFinished(quint32 command, qint32 result);
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
