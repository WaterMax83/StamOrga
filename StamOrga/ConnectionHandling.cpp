
#include <QDebug>

#include "connectionhandling.h"
#include "../Common/Network/messageprotocol.h"
#include "../Common/General/globalfunctions.h"

ConnectionHandling::ConnectionHandling(QObject *parent) : QObject(parent)
{

}

bool ConnectionHandling::startMainConnection()
{
    if (this->m_ctrlMainCon.IsRunning())
    {
        qWarning() << QString("Main Connect is already running");
        return false;
    }

    this->m_pMainCon = new MainConnection(this->m_pGlobalData);
    connect(this->m_pMainCon, &MainConnection::connectionRequestFinished, this, &ConnectionHandling::slMainConReqFin);
    this->m_ctrlMainCon.Start(this->m_pMainCon, false);

    this->m_pGlobalData->saveGlobalUserSettings();

    return true;
}

bool ConnectionHandling::startGettingInfo()
{
    qDebug() << "Started getting Infos";

    this->sendVersionRequest();
    return true;
}

/*
 * Answer function after connection with username
 */
void ConnectionHandling::slMainConReqFin(qint32 result, const QString &msg)
{
    if (result > ERROR_CODE_NO_ERROR)
    {
        this->m_pGlobalData->setConDataPort((quint16)result);
        this->startDataConnection();
        QThread::msleep(2);
        this->sendLoginRequest();
    }
    else
    {
        qWarning() << "Error main connecting: " << msg;
        emit this->sNotifyConnectionFinished(result);
    }
    this->m_ctrlMainCon.Stop();
}

/*
 * Functions for login
 */
void ConnectionHandling::sendLoginRequest()
{
    this->startDataConnection();                        // call it every time, if it is already started it just returns
    connect(this->m_pDataCon, &DataConnection::notifyLoginRequest,
            this, &ConnectionHandling::slDataConLoginFinished);

//    qDebug() << "Break 2";
    emit this->sStartSendLoginRequest();
}

/*
 * Answer function after login in with password
 */
void ConnectionHandling::slDataConLoginFinished(qint32 result)
{
    disconnect(this->m_pDataCon, &DataConnection::notifyLoginRequest,
            this, &ConnectionHandling::slDataConLoginFinished);

    if (result == ERROR_CODE_SUCCESS) {
        qInfo().noquote() << "Logged in succesfully";
    }
    else {
        qWarning().noquote() << QString("Error Login: %1").arg(getErrorCodeString(result));
        this->m_ctrlDataCon.Stop();
    }

    emit this->sNotifyConnectionFinished(result);
}

/*
 * Funktions for version request
 */
void ConnectionHandling::sendVersionRequest()
{
    this->startDataConnection();                        // call it every time, if it is already started it just returns
    connect(this->m_pDataCon, &DataConnection::notifyVersionRequest,
            this, &ConnectionHandling::slDataConVersionFinished);

    emit this->sStartSendVersionRequest();
}

void ConnectionHandling::slDataConVersionFinished(qint32 result, QString msg)
{
    disconnect(this->m_pDataCon, &DataConnection::notifyVersionRequest,
            this, &ConnectionHandling::slDataConVersionFinished);
    qDebug() << "version request finished: "  << msg;

    emit this->sNotifyVersionRequest(result, msg);
}


void ConnectionHandling::startDataConnection()
{
    if (this->isDataConnectionActive())
        return;

    this->m_pDataCon = new DataConnection(this->m_pGlobalData);

    connect(this, &ConnectionHandling::sStartSendVersionRequest,
            this->m_pDataCon, &DataConnection::startSendVersionRequest);
    connect(this, &ConnectionHandling::sStartSendLoginRequest,
            this->m_pDataCon, &DataConnection::startSendLoginRequest);

    this->m_ctrlDataCon.Start(this->m_pDataCon, false);
}

void ConnectionHandling::stopDataConnection()
{
    if (!this->isDataConnectionActive())
        return;

    disconnect(this, &ConnectionHandling::sStartSendVersionRequest,
            this->m_pDataCon, &DataConnection::startSendVersionRequest);
    disconnect(this, &ConnectionHandling::sStartSendLoginRequest,
            this->m_pDataCon, &DataConnection::startSendLoginRequest);

    this->m_ctrlDataCon.Stop();
}
