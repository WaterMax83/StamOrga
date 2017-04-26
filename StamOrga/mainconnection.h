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

#ifndef MAINCONNECTION_H
#define MAINCONNECTION_H

#include <QtCore/QObject>
#include <QtCore/QTimer>
#include <QtNetwork/QUdpSocket>

#include "../Common/General/backgroundworker.h"
#include "../Common/Network/messagebuffer.h"
#include "../Data/globaldata.h"

class MainConnection : public BackgroundWorker
{
    Q_OBJECT
public:
    MainConnection(GlobalData* pData);
    ~MainConnection();

    int DoBackgroundWork() override;

    QString m_workerName = "MainConnection";

signals:
    void connectionRequestFinished(qint32 result, const QString& msg);

private slots:
    void connectionTimeoutFired();
    void readyReadMasterPort();

private:
    GlobalData*   m_pGlobalData;
    MessageBuffer m_messageBuffer;

    QTimer*      m_pConTimeout;
    QUdpSocket*  m_pMasterUdpSocket = NULL;
    QHostAddress m_hMasterReceiver;

    void checkNewOncomingData();
};

#endif // MAINCONNECTION_H
