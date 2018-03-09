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

#ifndef CCONTCPDATA_H
#define CCONTCPDATA_H

#include <QtCore/QCryptographicHash>
#include <QtCore/QList>
#include <QtCore/QTimer>
#include <QtNetwork/QHostAddress>
#include <QtNetwork/QTcpSocket>

#include "../Common/General/backgroundworker.h"
//#include "../Common/Network/messagebuffer.h"
//#include "../Data/globaldata.h"
//#include "datahandling.h"

struct TcpDataConRequest {
    quint32    m_request;
    QByteArray m_lData;
    qint32     m_result;
    QString    m_returnData;

    TcpDataConRequest(quint32 req)
    {
        m_request = req;
    }
    TcpDataConRequest() {}
};

class cConTcpData : public BackgroundWorker
{
    Q_OBJECT
public:
    cConTcpData();
    ~cConTcpData();

    qint32 initialize(QString host, qint32 port);

    //    qint32 initialize() { return ERROR_CODE_SUCCESS; }

    int DoBackgroundWork() override;

    //    QString m_workerName = "cConTcpData";

    //    void setRandomLoginValue(QString val) { this->m_randomLoginValue = val; }

    qint32 terminate();


signals:
    //    void notifyLastRequestFinished(DataConRequest request);
    void signalDataConnectionFinished(qint32 result, const QString msg);

public slots:
    //    void startSendNewRequest(DataConRequest request);

private slots:
    void slotConnectionTimeoutFired();
    void slotDataSocketConnected();
    void slotReadyReadDataPort();
    void slotSocketDataError(QAbstractSocket::SocketError socketError);

private:
    QHostAddress m_hMasterReceiver;
    quint16      m_dataPort;
    //    GlobalData*   m_pGlobalData;
    //    MessageBuffer m_messageBuffer;
    //    DataHandling* m_pDataHandle;
    //    QString       m_randomLoginValue;

    QTimer*     m_pConTimeout;
    QTcpSocket* m_pDataTcpSocket = NULL;
    //    QHostAddress m_hDataReceiver;

    //    void startSendLoginRequest(DataConRequest request);
    //    void startSendVersionRequest(DataConRequest request);
    //    void startSendUserPropsRequest(DataConRequest request);
    //    void startSendGetUserEventsRequest(DataConRequest request);
    //    void startSendSetUserEventsRequest(DataConRequest request);
    //    void startSendUpdPassRequest(DataConRequest request);
    //    void startSendReadableNameRequest(DataConRequest request);
    //    void startSendGamesListRequest(DataConRequest request);
    //    void startSendGamesInfoListRequest(DataConRequest request);
    //    void startSendSetGameTimeFixedRequest(DataConRequest request);
    //    void startSendAddSeasonTicket(DataConRequest request);
    //    void startSendRemoveSeasonTicket(DataConRequest request);
    //    void startSendSeasonTicketListRequest(DataConRequest request);
    //    void startSendEditSeasonTicket(DataConRequest request);
    //    void startSendChangeTicketState(DataConRequest request);
    //    void startSendAvailableTicketListRequest(DataConRequest request);
    //    void startSendChangeGameRequest(DataConRequest request);
    //    void startSendChangeMeetingInfo(DataConRequest request);
    //    void startSendGetMeetingInfo(DataConRequest request);
    //    void startSendAcceptMeeting(DataConRequest request);
    //    void startSendChangeNewsData(DataConRequest request);
    //    void startSendGetNewsDataList(DataConRequest request);
    //    void startSendGetNewDataItem(DataConRequest request);
    //    void startSendDeleteNewDataItem(DataConRequest request);
    //    void startSendStatisticsCommand(DataConRequest request);


    //    void           checkNewOncomingData();
    //    qint32         sendMessageRequest(MessageProtocol* msg, DataConRequest request);
    //    void           removeActualRequest(quint32 req);
    //    DataConRequest getActualRequest(quint32 req);
    //    QString        getActualRequestData(quint32 req, qint32 index);
    //    QString        createHashValue(const QString first, const QString second);

    //    bool m_bRequestLoginAgain;
    //    void sendActualRequestsAgain(qint32 result);

    //    QList<DataConRequest> m_lActualRequest;
    //    QCryptographicHash*   m_hash;
};

#endif // CCONTCPDATA_H
