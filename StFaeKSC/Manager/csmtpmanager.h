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

#ifndef CSMTPMANAGER_H
#define CSMTPMANAGER_H

#include <QObject>
#include <QtCore/QMutex>
#include <QtCore/QWaitCondition>

#include "../Common/General/backgroundworker.h"

struct SmtpMail {
    QString m_header;
    QString m_body;
};

class cSmtpManager : public BackgroundWorker
{
    Q_OBJECT
public:
    explicit cSmtpManager(QObject* parent = 0);

    qint32 initialize();

    qint32 setServerEmail(const QString email);
    QString getServerEmail();
    qint32 setServerPassword(const QString password);
    QString getServerPassword();

    qint32 addDestinationEmail(const QString email);
    qint32      clearDestinationEmails();
    QStringList getDestinationEmails();

    qint32 sendNewEmail(const QString header, const QString body);

protected:
    int DoBackgroundWork();

signals:
    void signalSendNewEmails(SmtpMail* pMail);

private slots:
    void slotSendNewEmails(SmtpMail* pMail);

private:
    QMutex m_mutex;

    QString     m_serverEmail;
    QString     m_serverPassword;
    QStringList m_destinationAdress;
};

extern cSmtpManager g_SmtpManager;

#endif // CSMTPMANAGER_H
