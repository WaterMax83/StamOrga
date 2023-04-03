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
#include <QtCore/QThread>

#include "../Common/General/globalfunctions.h"
#include "SmtpClient-for-Qt/src/SmtpMime"
#include "csmtpmanager.h"

cSmtpManager g_SmtpManager;

cSmtpManager::cSmtpManager(QObject* parent)
    : BackgroundWorker(parent)
{
    this->m_bDeactivate = false;
}


qint32 cSmtpManager::initialize()
{
    this->SetWorkerName("SmtpManager");

    this->m_initialized = true;

    return ERROR_CODE_SUCCESS;
}

int cSmtpManager::DoBackgroundWork()
{
    connect(this, &cSmtpManager::signalSendNewEmails, this, &cSmtpManager::slotSendNewEmails);

    return ERROR_CODE_SUCCESS;
}

qint32 cSmtpManager::setServerEmail(const QString email)
{
    if (!this->m_initialized)
        return ERROR_CODE_NOT_INITIALIZED;

    QMutexLocker lock(&this->m_mutex);

    this->m_serverEmail = email;

    return ERROR_CODE_SUCCESS;
}

QString cSmtpManager::getServerEmail()
{
    if (!this->m_initialized)
        return "";

    QMutexLocker lock(&this->m_mutex);

    return this->m_serverEmail;
}

qint32 cSmtpManager::setServerPassword(const QString password)
{
    if (!this->m_initialized)
        return ERROR_CODE_NOT_INITIALIZED;

    QMutexLocker lock(&this->m_mutex);

    this->m_serverPassword = password;

    return ERROR_CODE_SUCCESS;
}

QString cSmtpManager::getServerPassword()
{
    if (!this->m_initialized)
        return "";

    QMutexLocker lock(&this->m_mutex);

    return this->m_serverPassword;
}

qint32 cSmtpManager::addDestinationEAddr(const QString& email)
{
    if (!this->m_initialized)
        return ERROR_CODE_NOT_INITIALIZED;

    QMutexLocker lock(&this->m_mutex);

    if (this->m_destinationAdress.contains(email.toLower()))
        return ERROR_CODE_ALREADY_EXIST;

    this->m_destinationAdress.append(email.toLower());

    return ERROR_CODE_SUCCESS;
}

qint32 cSmtpManager::removeDestinationEAddr(const QString& email)
{
    if (!this->m_initialized)
        return ERROR_CODE_NOT_INITIALIZED;

    QMutexLocker lock(&this->m_mutex);

    if (!this->m_destinationAdress.contains(email.toLower()))
        return ERROR_CODE_NOT_FOUND;

    this->m_destinationAdress.removeOne(email.toLower());

    return ERROR_CODE_SUCCESS;
}

qint32 cSmtpManager::clearDestinationEAddresses()
{
    if (!this->m_initialized)
        return ERROR_CODE_NOT_INITIALIZED;

    QMutexLocker lock(&this->m_mutex);

    this->m_destinationAdress.clear();

    return ERROR_CODE_SUCCESS;
}

QStringList cSmtpManager::getDestinationEAddresses()
{
    if (!this->m_initialized)
        return QStringList();

    QMutexLocker lock(&this->m_mutex);

    return this->m_destinationAdress;
}

qint32 cSmtpManager::getDoesDestEAddressExist(const QString& addr)
{
    if (!this->m_initialized)
        return -1;

    QMutexLocker lock(&this->m_mutex);

    if (this->m_destinationAdress.contains(addr.toLower()))
        return 1;
    return 0;
}

qint32 cSmtpManager::sendNewEmail(const QString header, const QString body)
{
    QMutexLocker lock(&this->m_mutex);

    if (this->m_serverEmail.isEmpty() || this->m_serverPassword.isEmpty()) {
        qWarning().noquote() << "No address and password to send email to";
        return ERROR_CODE_NOT_FOUND;
    }

    SmtpMail* pMail = new SmtpMail();
    pMail->m_header = header;
    pMail->m_body   = body;

    emit this->signalSendNewEmails(pMail);

    return ERROR_CODE_SUCCESS;
}


void cSmtpManager::slotSendNewEmails(SmtpMail* pMail)
{
    QMutexLocker lock(&this->m_mutex);

    if (this->m_bDeactivate) {
        qInfo() << "Do not send email because it is deactivated";
        delete pMail;
        return;
    }

    SmtpClient client("mail.gmx.net", 465, SmtpClient::ConnectionType::SslConnection);

//    client.setUser(this->m_serverEmail);
//    client.setPassword(this->m_serverPassword);

    MimeMessage message;

    EmailAddress sender(this->m_serverEmail, "StamOrga");
    message.setSender(sender);

    EmailAddress to(this->m_serverEmail, "StamOrga");
    message.addTo(to);
    for (int i = 0; i < this->m_destinationAdress.size(); i++) {
        EmailAddress bcc(this->m_destinationAdress.at(i));
        message.addBcc(bcc);
    }
    message.setSubject(pMail->m_header);

    // Now add some text to the email.
    // First we create a MimeText object.
    MimeText text;

    QString strTxt = pMail->m_body;
    strTxt.append("\n\nViele Grüße\nStamOrga");
    text.setText(strTxt);

    // Now add it to the mail
    message.addPart(&text);

    // Now we can send the mail
    client.connectToHost();
    if (!client.waitForReadyConnected()) {
        qWarning() << "SmptManager::Failed to connect to host.";
        delete pMail;
        return;
    }
    client.login(this->m_serverEmail, this->m_serverPassword);
    if (!client.waitForAuthenticated()) {
        qWarning() << "SmptManager::Failed to authenticate.";
        delete pMail;
        return;
    }
    client.sendMail(message);
    if (!client.waitForMailSent()) {
        qWarning() << "SmptManager::Failed to send mail.";
        delete pMail;
        return;
    }


    client.quit();

        qInfo().noquote() << QString("Sending email with success to %1 recipients").arg(this->m_destinationAdress.size());


    delete pMail;
}
