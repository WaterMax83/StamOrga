
#include <QDebug>

#include "connectionhandling.h"
#include "../Common/Network/messageprotocol.h"
#include "../Common/General/globalfunctions.h"
#include "../Common/General/globaltiming.h"

#define TIMER_DIFF_MSEC      10 * 1000

ConnectionHandling::ConnectionHandling(QObject *parent) : QObject(parent)
{
    this->m_pTimerConReset = new QTimer();
    this->m_pTimerConReset->setSingleShot(true);
    this->m_pTimerConReset->setInterval(CON_RESET_TIMEOUT_MSEC - TIMER_DIFF_MSEC);
    connect(this->m_pTimerConReset, &QTimer::timeout, this, &ConnectionHandling::slTimerConResetFired);

    this->m_pTimerLoginReset = new QTimer();
    this->m_pTimerLoginReset->setSingleShot(true);
    this->m_pTimerLoginReset->setInterval(CON_RESET_TIMEOUT_MSEC - TIMER_DIFF_MSEC);
    connect(this->m_pTimerLoginReset, &QTimer::timeout, this, &ConnectionHandling::slTimerConLoginFired);
}

bool ConnectionHandling::startMainConnection(QString name, QString passw)
{
    if (this->m_ctrlMainCon.IsRunning())
    {
        if (name == this->m_pGlobalData->userName()) {
            if (this->m_pGlobalData->bIsConnected && passw == this->m_pGlobalData->passWord()) {
                emit this->sNotifyConnectionFinished(ERROR_CODE_SUCCESS);
                qDebug() << "Saving to log in again, should already be succesfull";
                return true;
            }
            this->m_pGlobalData->setPassWord(passw);
            this->startDataConnection();
            QThread::msleep(1);
            this->sendLoginRequest();
            this->m_pGlobalData->saveGlobalUserSettings();
            return true;
        }
        this->m_ctrlMainCon.Stop();
        this->stopDataConnection();
        QThread::msleep(2);
    }
    this->m_pGlobalData->setUserName(name);
    this->m_pGlobalData->setPassWord(passw);

    this->m_pMainCon = new MainConnection(this->m_pGlobalData);
    connect(this->m_pMainCon, &MainConnection::connectionRequestFinished, this, &ConnectionHandling::slMainConReqFin);
    this->m_ctrlMainCon.Start(this->m_pMainCon, false);

    this->m_pGlobalData->saveGlobalUserSettings();

    return true;
}

bool ConnectionHandling::startGettingInfo()
{
    this->sendVersionRequest();
    this->sendUserPropertiesRequest();
    return true;
}

bool ConnectionHandling::startUpdatePassword(QString newPassWord)
{
    this->sendUpdatePasswordRequest(newPassWord);
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
        this->m_pTimerConReset->start();
    }
    else
    {
        qWarning() << "Error main connecting: " << msg;
        emit this->sNotifyConnectionFinished(result);
    }
}

/*
 * Functions for login
 */
void ConnectionHandling::sendLoginRequest()
{
    this->startDataConnection();                        // call it every time, if it is already started it just returns
    connect(this->m_pDataCon, &DataConnection::notifyLoginRequest,
            this, &ConnectionHandling::slDataConLoginFinished);

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
        this->m_pGlobalData->bIsConnected = true;
        this->m_pTimerLoginReset->start();
    }
    else {
        qWarning().noquote() << QString("Error Login: %1").arg(getErrorCodeString(result));
        this->stopDataConnection();
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
    this->m_pTimerLoginReset->start();
}

/*
 * Functions for user properties
 */
void ConnectionHandling::sendUserPropertiesRequest()
{
    this->startDataConnection();                        // call it every time, if it is already started it just returns
    connect(this->m_pDataCon, &DataConnection::notifyUserPropsRequest,
            this, &ConnectionHandling::slDataConUserPropsFinished);

    emit this->sStartSendUserPropertiesRequest();
}

void ConnectionHandling::slDataConUserPropsFinished(qint32 result, quint32 value)
{
    disconnect(this->m_pDataCon, &DataConnection::notifyUserPropsRequest,
            this, &ConnectionHandling::slDataConUserPropsFinished);

    emit this->sNotifyUserPropertiesRequest(result, value);
    this->m_pTimerLoginReset->start();
}

/*
 * Functions for updating passwords
 */
void ConnectionHandling::sendUpdatePasswordRequest(QString newPassWord)
{
    this->startDataConnection();                        // call it every time, if it is already started it just returns
    connect(this->m_pDataCon, &DataConnection::notifyUpdPassRequest,
            this, &ConnectionHandling::slDataConUpdPassFinished);

    emit this->sStartSendUpdatePasswordRequest(newPassWord);
}

void ConnectionHandling::slDataConUpdPassFinished(qint32 result)
{
    this->startDataConnection();                        // call it every time, if it is already started it just returns
    connect(this->m_pDataCon, &DataConnection::notifyUpdPassRequest,
            this, &ConnectionHandling::slDataConUpdPassFinished);

    emit this->sNotifyUpdatePasswordRequest(result);
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
    connect(this, &ConnectionHandling::sStartSendUserPropertiesRequest,
            this->m_pDataCon, &DataConnection::startSendUserPropsRequest);
    connect(this, &ConnectionHandling::sStartSendUpdatePasswordRequest,
            this->m_pDataCon, &DataConnection::startSendUpdPassRequest);

    this->m_ctrlDataCon.Start(this->m_pDataCon, false);
}

void ConnectionHandling::stopDataConnection()
{
    this->m_pGlobalData->bIsConnected = false;

    if (!this->isDataConnectionActive())
        return;

    disconnect(this, &ConnectionHandling::sStartSendVersionRequest,
            this->m_pDataCon, &DataConnection::startSendVersionRequest);
    disconnect(this, &ConnectionHandling::sStartSendLoginRequest,
            this->m_pDataCon, &DataConnection::startSendLoginRequest);
    disconnect(this, &ConnectionHandling::sStartSendUserPropertiesRequest,
            this->m_pDataCon, &DataConnection::startSendUserPropsRequest);
    disconnect(this, &ConnectionHandling::sStartSendUpdatePasswordRequest,
            this->m_pDataCon, &DataConnection::startSendUpdPassRequest);

    this->m_ctrlDataCon.Stop();
}

void ConnectionHandling::slTimerConResetFired()
{
    this->m_ctrlMainCon.Stop();
}

void ConnectionHandling::slTimerConLoginFired()
{
    this->m_pGlobalData->bIsConnected = false;
}

ConnectionHandling::~ConnectionHandling()
{
    if (this->m_ctrlDataCon.IsRunning())
        this->stopDataConnection();

    if (this->m_ctrlMainCon.IsRunning())
        this->m_ctrlMainCon.Stop();
}
