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
#include "../Common/Network/messagebuffer.h"
#include "../Common/Network/messageprotocol.h"
//#include "../Data/globaldata.h"
//#include "datahandling.h"

struct TcpDataConRequest {
    quint32    m_request;
    QByteArray m_lData;
    qint32     m_result;
    //    QString    m_sInfoData;

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

    int DoBackgroundWork() override;


    qint32 terminate();


signals:
    void notifyLastRequestFinished(TcpDataConRequest* request);
    void signalDataConnectionFinished(qint32 result, const QString msg);

public slots:
    void startSendNewRequest(TcpDataConRequest* request);

private slots:
    void slotConnectionTimeoutFired();
    void slotDataSocketConnected();
    void slotReadyReadDataPort();
    void slotSocketDataError(QAbstractSocket::SocketError socketError);

private:
    QHostAddress  m_hMasterReceiver;
    quint16       m_dataPort;
    MessageBuffer m_messageBuffer;

    QTimer*     m_pConTimeout;
    QTcpSocket* m_pDataTcpSocket = NULL;

    //    void startSendGetUserEventsRequest(DataConRequest request);
    //    void startSendSetUserEventsRequest(DataConRequest request);


    void               checkNewOncomingData();
    qint32             sendMessageRequest(MessageProtocol* msg, TcpDataConRequest* request);
    void               removeActualRequest(TcpDataConRequest* request);
    TcpDataConRequest* getActualRequest(quint32 req);

    bool m_bRequestLoginAgain;
    void sendActualRequestsAgain(qint32 result);

    QList<TcpDataConRequest*> m_lActualRequest;
};

#endif // CCONTCPDATA_H
