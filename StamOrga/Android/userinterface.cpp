#include <QtCore/QDebug>

#include "userinterface.h"

UserInterface::UserInterface(QObject *parent) : QObject(parent)
{
    this->m_pMainCon = new ConnectionHandling();
    connect(this->m_pMainCon, &ConnectionHandling::notifyConnectionFinished, this, &UserInterface::connectionFinished);
}

void UserInterface::StartSendingData()
{
    qDebug() << "Started sending Data";

    this->m_pMainCon->setGlobalData(this->m_pGlobalData);

    if (this->m_pMainCon->StartMainConnection())
    {
//        this->ui->btnSendData->setEnabled(false);
    }
}

void UserInterface::connectionFinished(bool result)
{
    //this->ui->btnSendData->setEnabled(true);
    emit this->notifyConnectionFinished(result);
}
