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

#ifndef DATACONNECTION_H
#define DATACONNECTION_H

#include <QtCore/QCryptographicHash>
#include <QtCore/QList>
#include <QtCore/QTimer>
#include <QtNetwork/QUdpSocket>

#include "../Common/General/backgroundworker.h"
#include "../Common/Network/messagebuffer.h"
#include "../Data/globaldata.h"
#include "datahandling.h"

struct DataConRequest {
    quint32        m_request;
    QList<QString> m_lData;
    qint32         m_result;
    QString        m_returnData;

    DataConRequest(quint32 req)
    {
        m_request = req;
    }
    DataConRequest() {}
};

class DataConnection : public BackgroundWorker
{
    Q_OBJECT
public:
    DataConnection(GlobalData* pData);
    ~DataConnection();

    int DoBackgroundWork() override;

    QString m_workerName = "DataConnection";

    void setRandomLoginValue(QString val) { this->m_randomLoginValue = val; }


signals:
    void notifyLastRequestFinished(DataConRequest request);

public slots:
    void startSendNewRequest(DataConRequest request);

private slots:
    void slotConnectionTimeoutFired();
    void slotReadyReadDataPort();
    void slotSocketDataError(QAbstractSocket::SocketError socketError);

private:
    GlobalData*   m_pGlobalData;
    MessageBuffer m_messageBuffer;
    DataHandling* m_pDataHandle;
    QString       m_randomLoginValue;

    QTimer*      m_pConTimeout;
    QUdpSocket*  m_pDataUdpSocket = NULL;
    QHostAddress m_hDataReceiver;

    void startSendLoginRequest(DataConRequest request);
    void startSendVersionRequest(DataConRequest request);
    void startSendUserPropsRequest(DataConRequest request);
    void startSendUpdPassRequest(DataConRequest request);
    void startSendReadableNameRequest(DataConRequest request);
    void startSendGamesListRequest(DataConRequest request);
    void startSendGamesInfoListRequest(DataConRequest request);
    void startSendSetGameTimeFixedRequest(DataConRequest request);
    void startSendAddSeasonTicket(DataConRequest request);
    void startSendRemoveSeasonTicket(DataConRequest request);
    void startSendSeasonTicketListRequest(DataConRequest request);
    void startSendEditSeasonTicket(DataConRequest request);
    void startSendChangeTicketState(DataConRequest request);
    void startSendAvailableTicketListRequest(DataConRequest request);
    void startSendChangeGameRequest(DataConRequest request);
    void startSendChangeMeetingInfo(DataConRequest request);
    void startSendGetMeetingInfo(DataConRequest request);
    void startSendAcceptMeeting(DataConRequest request);
    void startSendChangeNewsData(DataConRequest request);
    void startSendGetNewsDataList(DataConRequest request);


    void   checkNewOncomingData();
    qint32 sendMessageRequest(MessageProtocol* msg, DataConRequest request);
    void removeActualRequest(quint32 req);
    DataConRequest getActualRequest(quint32 req);
    QString getActualRequestData(quint32 req, qint32 index);
    QString createHashValue(const QString first, const QString second);

    bool m_bRequestLoginAgain;
    void sendActualRequestsAgain(qint32 result);

    QList<DataConRequest> m_lActualRequest;
    QCryptographicHash*   m_hash;
};

#endif // DATACONNECTION_H
