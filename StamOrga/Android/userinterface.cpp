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
}

qint32 UserInterface::startSendingData(QString name, QString passw)
{
    this->m_pConHandle->setGlobalData(this->m_pGlobalData);

    return this->m_pConHandle->startMainConnection(name, passw);
}

qint32 UserInterface::startUpdateUserPassword(QString newPassw)
{
    if (this->m_pConHandle->startUpdatePassword(newPassw))
        return ERROR_CODE_SUCCESS;
    return ERROR_CODE_COMMON;
}

void UserInterface::slConnectionRequestFinished(qint32 result)
{
    //this->ui->btnSendData->setEnabled(true);
    emit this->notifyConnectionFinished(result);
    this->m_pConHandle->startGettingInfo();
}

void UserInterface::slVersionRequestFinished(qint32 result, QString msg)
{
    emit this->notifyVersionRequestFinished(result, msg);
}


void UserInterface::slUpdatePasswordRequestFinished(qint32 result, QString newPassWord)
{
    emit this->notifyUpdatePasswordRequestFinished(result, newPassWord);
}
