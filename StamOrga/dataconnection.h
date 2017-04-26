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


signals:
    void notifyLastRequestFinished(DataConRequest request);

public slots:
    void startSendNewRequest(DataConRequest request);

private slots:
    void connectionTimeoutFired();
    void readyReadDataPort();

private:
    GlobalData*   m_pGlobalData;
    MessageBuffer m_messageBuffer;
    DataHandling* m_pDataHandle;

    QTimer*      m_pConTimeout;
    QUdpSocket*  m_pDataUdpSocket = NULL;
    QHostAddress m_hDataReceiver;

    void startSendLoginRequest(DataConRequest request);
    void startSendVersionRequest(DataConRequest request);
    void startSendUserPropsRequest(DataConRequest request);
    void startSendUpdPassRequest(DataConRequest request);
    void startSendReadableNameRequest(DataConRequest request);
    void startSendGamesListRequest(DataConRequest request);
    void startSendAddSeasonTicket(DataConRequest request);
    void startSendRemoveSeasonTicket(DataConRequest request);
    void startSendSeasonTicketListRequest(DataConRequest request);
    void startSendNewPlaceTicket(DataConRequest request);


    void   checkNewOncomingData();
    qint32 sendMessageRequest(MessageProtocol* msg, DataConRequest request);
    void removeActualRequest(quint32 req);
    DataConRequest getActualRequest(quint32 req);
    QString getActualRequestData(quint32 req, qint32 index);

    bool m_bRequestLoginAgain;
    void sendActualRequestsAgain();

    QList<DataConRequest> m_lActualRequest;
};

#endif // DATACONNECTION_H
