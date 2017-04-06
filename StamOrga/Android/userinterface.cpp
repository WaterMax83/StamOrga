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
}

qint32 UserInterface::startMainConnection(QString name, QString passw)
{
    this->m_pConHandle->setGlobalData(this->m_pGlobalData);

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
