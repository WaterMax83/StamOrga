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

#ifndef CTCPMAINCON_H
#define CTCPMAINCON_H

#include <QtCore/QObject>
#include <QtCore/QTimer>
#include <QtNetwork/QHostAddress>
#include <QtNetwork/QTcpSocket>

#include "../Common/General/backgroundworker.h"
#include "../Common/Network/messagebuffer.h"
//#include "../Data/globaldata.h"

class cConTcpMain : public BackgroundWorker
{
    Q_OBJECT
public:
    cConTcpMain();
    ~cConTcpMain();

    qint32 initialize(QString host);

    int DoBackgroundWork() override;

signals:
    void connectionRequestFinished(qint32 result, const QString msg, const QString salt, const QString random);

public slots:
    //    void slotSendNewMainConRequest(QString username);
    //    void slotNewBindingPortRequest();

private slots:
    void slotConnectionTimeoutFired();
    void slotMasterSocketConnected();
    void slotReadyReadMasterSocket();
    void slotMainSocketError(QAbstractSocket::SocketError socketError);


private:
    //    GlobalData*   m_pGlobalData;
    //    MessageBuffer m_messageBuffer;
    //    QString       m_userName;

    QTimer*      m_pConTimeout;
    QTcpSocket*  m_pMasterTcpSocket = NULL;
    QHostAddress m_hMasterReceiver;

    //    void checkNewOncomingData();
};

#endif // CTCPMAINCON_H
