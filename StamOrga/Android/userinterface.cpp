#include <QtCore/QDebug>

#include "userinterface.h"

UserInterface::UserInterface(QObject *parent) : QObject(parent)
{
    this->m_pConHandle = new ConnectionHandling();
    connect(this->m_pConHandle, &ConnectionHandling::sNotifyConnectionFinished,
            this, &UserInterface::slConnectionRequestFinished);
    connect(this->m_pConHandle, &ConnectionHandling::sNotifyVersionRequest,
            this, &UserInterface::slVersionRequestFinished);
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

qint32 UserInterface::startRemoveSeasonTicket(QString name)
{
    return this->m_pConHandle->startSeasonTicketRemove(name);
}

qint32 UserInterface::startGettingSeasonTicketList()
{
    return this->m_pConHandle->startGettingSeasonTicketList();
}

void UserInterface::slConnectionRequestFinished(qint32 result)
{
    //this->ui->btnSendData->setEnabled(true);
    emit this->notifyConnectionFinished(result);
    this->m_pConHandle->startGettingVersionInfo();
    this->m_pConHandle->startGettingUserProps();
}

void UserInterface::slVersionRequestFinished(qint32 result, QString msg)
{
    emit this->notifyVersionRequestFinished(result, msg);
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

void UserInterface::slSeasonTicketListFinished(qint32 result)
{
    emit this->notifySeasonTicketListFinished(result);
}
