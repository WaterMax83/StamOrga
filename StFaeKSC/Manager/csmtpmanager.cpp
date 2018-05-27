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

qint32 cSmtpManager::addDestinationEmail(const QString email)
{
    if (!this->m_initialized)
        return ERROR_CODE_NOT_INITIALIZED;

    QMutexLocker lock(&this->m_mutex);

    if (this->m_destinationAdress.contains(email))
        return ERROR_CODE_ALREADY_EXIST;

    this->m_destinationAdress.append(email);

    return ERROR_CODE_SUCCESS;
}

qint32 cSmtpManager::clearDestinationEmails()
{
    if (!this->m_initialized)
        return ERROR_CODE_NOT_INITIALIZED;

    QMutexLocker lock(&this->m_mutex);

    this->m_destinationAdress.clear();

    return ERROR_CODE_SUCCESS;
}

QStringList cSmtpManager::getDestinationEmails()
{
    if (!this->m_initialized)
        return QStringList();

    QMutexLocker lock(&this->m_mutex);

    return this->m_destinationAdress;
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

    QThread::sleep(1);

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

    client.setUser(this->m_serverEmail);
    client.setPassword(this->m_serverPassword);

    MimeMessage message;

    message.setSender(new EmailAddress(this->m_serverEmail, "StamOrga"));
    message.addTo(new EmailAddress(this->m_serverEmail, "StamOrga"));
    for (int i = 0; i < this->m_destinationAdress.size(); i++)
        message.addBcc(new EmailAddress(this->m_destinationAdress.at(i)));
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
    qint32 successCnt = 0;
    bool   rCode      = client.connectToHost();
    if (rCode) {
        successCnt++;
        rCode = client.login();
    }
    if (rCode) {
        successCnt++;
        rCode = client.sendMail(message);
    }
    client.quit();

    if (rCode)
        qInfo().noquote() << QString("Sending email with success to %1 recipients").arg(this->m_destinationAdress.size());
    else
        qWarning().noquote() << QString("Error sending email with step %1").arg(successCnt);

    delete pMail;
}