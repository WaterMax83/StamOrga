
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
        emit this->notifyError("Main Connect is already running");
        return false;
    }

    this->m_pMainCon = new MainConnection(this->m_pGlobalData);
    connect(this->m_pMainCon, &MainConnection::connectionRequestFinished, this, &ConnectionHandling::MainConReqFin);
    this->m_ctrlMainCon.Start(this->m_pMainCon, false);

    this->m_pGlobalData->saveGlobalUserSettings();

    return true;
}

void ConnectionHandling::MainConReqFin(bool result, const QString &msg)
{
    if (result)
    {
        emit this->notifyInfo("Connection ok");
        qInfo() << "Connection ok";
    }
    else
    {
        emit this->notifyError("Error connecting: " + msg);
        qWarning() << "Error connecting: " << msg;
    }
    emit this->notifyConnectionFinished(result);
    this->m_ctrlMainCon.Stop();
}
