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

#ifndef CCONSSLSERVER_H
#define CCONSSLSERVER_H

#include <QObject>
#include <QtNetwork/QSslCertificate>
#include <QtNetwork/QSslKey>
#include <QtNetwork/QTcpServer>

enum cConSslUsage {
    SSL_UNKNOWN = -1,
    NO_SSL      = 0,
    USE_SSL     = 1
};

class cConSslServer : public QTcpServer
{
    Q_OBJECT
public:
    explicit cConSslServer(cConSslUsage ssl = cConSslUsage::NO_SSL, QObject* parent = nullptr);

    void incomingConnection(qintptr socketDescriptor) override;

    cConSslUsage getSslUsage(void) { return this->m_ssl; }

signals:

private:
    cConSslUsage m_ssl;

    static QSslKey         m_sslKey;
    static QSslCertificate m_caCert;
    static QSslCertificate m_localCert;
    static cConSslUsage    m_staticIsSslInit;
};

#endif // CCONSSLSERVER_H
