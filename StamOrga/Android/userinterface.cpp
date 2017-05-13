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

#include <QtCore/QDebug>

#include "userinterface.h"

UserInterface::UserInterface(QObject* parent)
    : QObject(parent)
{
    this->m_pConHandle = new ConnectionHandling();
    connect(this->m_pConHandle, &ConnectionHandling::sNotifyConnectionFinished,
            this, &UserInterface::slConnectionRequestFinished);
    connect(this->m_pConHandle, &ConnectionHandling::sNotifyVersionRequest,
            this, &UserInterface::slVersionRequestFinished);
    connect(this->m_pConHandle, &ConnectionHandling::sNotifyUserPropertiesRequest,
            this, &UserInterface::slUserPropertiesFinished);
    connect(this->m_pConHandle, &ConnectionHandling::sNotifyUpdatePasswordRequest,
            this, &UserInterface::slUpdatePasswordRequestFinished);
    connect(this->m_pConHandle, &ConnectionHandling::sNotifyUpdateReadableNameRequest,
            this, &UserInterface::slUpdateReadableNameRequestFinished);
    connect(this->m_pConHandle, &ConnectionHandling::sNotifyGamesListRequest,
            this, &UserInterface::slGettingGamesListFinished);
    connect(this->m_pConHandle, &ConnectionHandling::sNotifySeasonTicketAddRequest,
            this, &UserInterface::slSeasonTicketAddFinished);
    connect(this->m_pConHandle, &ConnectionHandling::sNotifySeasonTicketRemoveRequest,
            this, &UserInterface::slSeasonTicketRemoveFinished);
    connect(this->m_pConHandle, &ConnectionHandling::sNotifySeasonTicketNewPlace,
            this, &UserInterface::slSeasonTicketNewPlaceFinished);
    connect(this->m_pConHandle, &ConnectionHandling::sNotifySeasonTicketListRequest,
            this, &UserInterface::slSeasonTicketListFinished);
}

qint32 UserInterface::startMainConnection(QString name, QString passw)
{
    return this->m_pConHandle->startMainConnection(name, passw);
}

qint32 UserInterface::startUpdateUserPassword(QString newPassw)
{
    return this->m_pConHandle->startUpdatePassword(newPassw);
}

qint32 UserInterface::startUpdateReadableName(QString name)
{
    return this->m_pConHandle->startUpdateReadableName(name);
}

qint32 UserInterface::startGettingGamesList()
{
    return this->m_pConHandle->startGettingGamesList();
}

qint32 UserInterface::startAddSeasonTicket(QString name, quint32 discount)
{
    return this->m_pConHandle->startSeasonTicketAdd(name, discount);
}

qint32 UserInterface::startRemoveSeasonTicket(quint32 index)
{
    return this->m_pConHandle->startSeasonTicketRemove(index);
}

qint32 UserInterface::startNewPlaceSeasonTicket(quint32 index, QString place)
{
    return this->m_pConHandle->startSeasonTicketNewPlace(index, place);
}

qint32 UserInterface::startGettingSeasonTicketList()
{
    return this->m_pConHandle->startGettingSeasonTicketList();
}

void UserInterface::slConnectionRequestFinished(qint32 result)
{
    //this->ui->btnSendData->setEnabled(true);
    emit this->notifyConnectionFinished(result);
	if (result > ERROR_CODE_NO_ERROR) {
		this->m_pConHandle->startGettingVersionInfo();
		this->m_pConHandle->startGettingUserProps();
	}
}

void UserInterface::slVersionRequestFinished(qint32 result, QString msg)
{
    emit this->notifyVersionRequestFinished(result, msg);
}


void UserInterface::slUserPropertiesFinished(qint32 result)
{
    emit this->notifyUserPropertiesFinished(result);
}

void UserInterface::slUpdatePasswordRequestFinished(qint32 result, QString newPassWord)
{
    emit this->notifyUpdatePasswordRequestFinished(result, newPassWord);
}

void UserInterface::slGettingGamesListFinished(qint32 result)
{
    emit this->notifyGamesListFinished(result);
}

void UserInterface::slUpdateReadableNameRequestFinished(qint32 result)
{
    emit this->notifyUpdateReadableNameRequest(result);
}

void UserInterface::slSeasonTicketAddFinished(qint32 result)
{
    emit this->notifySeasonTicketAddFinished(result);
}

void UserInterface::slSeasonTicketRemoveFinished(qint32 result)
{
    emit this->notifySeasonTicketRemoveFinished(result);
}

void UserInterface::slSeasonTicketNewPlaceFinished(qint32 result)
{
    emit this->notifySeasonTicketNewPlaceFinished(result);
}

void UserInterface::slSeasonTicketListFinished(qint32 result)
{
    emit this->notifySeasonTicketListFinished(result);
}
