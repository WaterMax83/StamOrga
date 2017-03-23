#include <QtCore/QDebug>

#include "userinterface.h"
#include "../connectioninfo.h"

UserInterface::UserInterface(QObject *parent) : QObject(parent)
{
    this->m_pMainCon = new ConnectionHandling();
    connect(this->m_pMainCon, &ConnectionHandling::NotifyConnectionFinished, this, &UserInterface::ConnectionFinished);
}

void UserInterface::StartSendingData()
{
    qDebug() << "Started sending Data";

    ConnectionInfo *info = this->m_pMainCon->GetConnectionInfo();
    QMutexLocker locker(&info->m_infoMutex);

    info->SetUserName(this->m_UserName);
    info->SetHostAddress(this->m_ipAddr);
    info->SetPort(this->m_port);
    if (this->m_pMainCon->StartMainConnection())
    {
//        this->ui->btnSendData->setEnabled(false);
    }
}

void UserInterface::ConnectionFinished()
{
    //this->ui->btnSendData->setEnabled(true);
    emit this->notifyConnectionFinished();
}
