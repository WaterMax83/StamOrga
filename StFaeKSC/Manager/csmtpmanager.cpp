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

#include "csmtpmanager.h"
#include "../Common/General/globalfunctions.h"

cSmtpManager g_SmtpManager;

cSmtpManager::cSmtpManager(QObject *parent) : BackgroundWorker(parent)
{

}


qint32 cSmtpManager::initialize()
{
    this->SetWorkerName("SmtpManager");

    this->m_initialized = true;

    return ERROR_CODE_SUCCESS;
}

int cSmtpManager::DoBackgroundWork()
{
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

qint32 cSmtpManager::setServerPassword(const QString password)
{
    if (!this->m_initialized)
        return ERROR_CODE_NOT_INITIALIZED;

    QMutexLocker lock(&this->m_mutex);

    this->m_serverPassword = password;

    return ERROR_CODE_SUCCESS;
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
