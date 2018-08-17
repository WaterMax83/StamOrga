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

#include <QtNetwork/QSslCertificate>
#include <QtNetwork/QSslCipher>
#include <QtNetwork/QSslConfiguration>
#include <QtNetwork/QSslKey>
#include <QtNetwork/QSslSocket>

#include "../../Common/General/globalfunctions.h"
#include "cconsslserver.h"

cConSslUsage    cConSslServer::m_staticIsSslInit = SSL_UNKNOWN;
QSslKey         cConSslServer::m_sslKey;
QSslCertificate cConSslServer::m_localCert;
QSslCertificate cConSslServer::m_caCert;

cConSslServer::cConSslServer(cConSslUsage ssl, QObject* parent)
    : QTcpServer(parent)
{
    if (ssl == USE_SSL) {
        if (m_staticIsSslInit == SSL_UNKNOWN) {
            m_staticIsSslInit = NO_SSL;

            QString sslPath = getUserHomeConfigPath() + "/SSL";
            QFile   keyFile(sslPath + "/server.key");
            QFile   localCertFile(sslPath + "/server.crt");
            QFile   caCertFile(sslPath + "/ca.crt");

            if (keyFile.open(QIODevice::ReadOnly) && localCertFile.open(QIODevice::ReadOnly) && caCertFile.open(QIODevice::ReadOnly)) {
                m_sslKey          = QSslKey(keyFile.readAll(), QSsl::Rsa);
                m_localCert       = QSslCertificate(localCertFile.readAll());
                m_caCert          = QSslCertificate(caCertFile.readAll());
                m_staticIsSslInit = USE_SSL;

                keyFile.close();
                localCertFile.close();
                caCertFile.close();
            }
        }

        if (m_staticIsSslInit == NO_SSL) {
            qWarning().noquote() << "Could not use SSL for server";
            ssl = NO_SSL;
        }
    }

    this->m_ssl = ssl;
}


void cConSslServer::incomingConnection(qintptr socketDescriptor)
{
    QTcpSocket* pSocket;
    if (this->m_ssl == cConSslUsage::USE_SSL) {
        pSocket = new QSslSocket(this);

        pSocket->setSocketDescriptor(socketDescriptor);

        ((QSslSocket*)pSocket)->setPrivateKey(m_sslKey);
        ((QSslSocket*)pSocket)->setLocalCertificate(m_localCert);
        ((QSslSocket*)pSocket)->addCaCertificate(m_caCert);
        ((QSslSocket*)pSocket)->setProtocol(QSsl::TlsV1_2);

        ((QSslSocket*)pSocket)->startServerEncryption();
    } else {
        pSocket = new QTcpSocket();
        pSocket->setSocketDescriptor(socketDescriptor);
    }
    this->addPendingConnection(pSocket);
}
