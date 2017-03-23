
#include <QDebug>

#include "connectionhandling.h"
#include "../Common/Network/messageprotocol.h"

ConnectionHandling::ConnectionHandling(QObject *parent) : QObject(parent)
{

}

bool ConnectionHandling::StartMainConnection()
{
    if (this->m_ctrlMainCon.IsRunning())
    {
        emit this->NotifyError("Main Connect is already running");
        return false;
    }

    this->m_pMainCon = new MainConnection(&this->m_conInfo);
    connect(this->m_pMainCon, &MainConnection::ConnectionRequestFinished, this, &ConnectionHandling::MainConReqFin);
    this->m_ctrlMainCon.Start(this->m_pMainCon, false);

    return true;
}

void ConnectionHandling::MainConReqFin(bool result, const QString &msg)
{
    if (result)
    {
        emit this->NotifyInfo("Connection ok");
        qInfo() << "Connection ok";
    }
    else
    {
        emit this->NotifyError("Error connecting: " + msg);
        emit this->NotifyConnectionFinished();
        qWarning() << "Error connecting: " << msg;
    }
    this->m_ctrlMainCon.Stop();
}
