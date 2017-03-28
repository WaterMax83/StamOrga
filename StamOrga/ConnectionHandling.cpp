
#include <QDebug>

#include "connectionhandling.h"
#include "../Common/Network/messageprotocol.h"
#include "../Common/General/globalfunctions.h"

ConnectionHandling::ConnectionHandling(QObject *parent) : QObject(parent)
{

}

bool ConnectionHandling::StartMainConnection()
{
    if (this->m_ctrlMainCon.IsRunning())
    {
        qWarning() << QString("Main Connect is already running");
        return false;
    }

    this->m_pMainCon = new MainConnection(this->m_pGlobalData);
    connect(this->m_pMainCon, &MainConnection::connectionRequestFinished, this, &ConnectionHandling::mainConReqFin);
    this->m_ctrlMainCon.Start(this->m_pMainCon, false);

    this->m_pGlobalData->saveGlobalUserSettings();

    return true;
}

void ConnectionHandling::mainConReqFin(qint32 result, const QString &msg)
{
    if (result > ERROR_CODE_NO_ERROR)
    {
        this->m_pGlobalData->setConDataPort((quint16)result);
        this->m_pDataCon = new DataConnection(this->m_pGlobalData);
        connect(this->m_pDataCon, &DataConnection::notifyLoginRequest, this, &ConnectionHandling::dataConLoginFinished);
        this->m_ctrlDataCon.Start(this->m_pDataCon, false);
    }
    else
    {
        qWarning() << "Error main connecting: " << msg;
        emit this->notifyConnectionFinished(false);
    }
    this->m_ctrlMainCon.Stop();
}


void ConnectionHandling::dataConLoginFinished(qint32 result)
{
    if (result == ERROR_CODE_SUCCESS) {
        qInfo().noquote() << "Logged in succesfully";
        emit this->notifyConnectionFinished(true);
    }
    else {
        qWarning().noquote() << QString("Error Login: %1").arg(getErrorCodeString(result));
        emit this->notifyConnectionFinished(false);
    }
}
