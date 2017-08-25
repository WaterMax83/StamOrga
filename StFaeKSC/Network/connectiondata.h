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

#ifndef CONNECTIONDATA_H
#define CONNECTIONDATA_H

#include <QtNetwork/QUdpSocket>

struct UserConData {
    QHostAddress m_sender;
    quint16      m_srcMasterPort;
    quint16      m_dstDataPort;
    quint16      m_srcDataPort;
    QString      m_userName;
    qint32       m_userID;
    QString      m_randomLogin;
    bool         m_bIsConnected;
};

#endif // CONNECTIONDATA_H
