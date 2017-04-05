
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

qint32 ConnectionHandling::startMainConnection(QString name, QString passw)
{
    if (this->isMainConnectionActive())
    {
        if (name == this->m_pGlobalData->userName()) {
            if (this->m_pGlobalData->bIsConnected() && passw == this->m_pGlobalData->passWord()) {
                emit this->sNotifyConnectionFinished(ERROR_CODE_SUCCESS);
                qDebug() << "Saving to log in again, should already be succesfull";
                return ERROR_CODE_NO_ERROR;
            }
            this->m_pGlobalData->setPassWord(passw);
            this->startDataConnection();
            QThread::msleep(1);
            this->sendLoginRequest();
            this->m_pGlobalData->saveGlobalUserSettings();
            return ERROR_CODE_SUCCESS;
        }
        this->m_ctrlMainCon.Stop();
        this->stopDataConnection();
        QThread::msleep(2);
    } else
        this->stopDataConnection();

    this->m_pGlobalData->setUserName(name);
    this->m_pGlobalData->setPassWord(passw);

    this->m_pMainCon = new MainConnection(this->m_pGlobalData);
    connect(this->m_pMainCon, &MainConnection::connectionRequestFinished, this, &ConnectionHandling::slMainConReqFin);
    this->m_ctrlMainCon.Start(this->m_pMainCon, false);

    this->m_pGlobalData->saveGlobalUserSettings();

    return ERROR_CODE_SUCCESS;
}

qint32 ConnectionHandling::startGettingVersionInfo()
{
    this->sendVersionRequest();
    return ERROR_CODE_SUCCESS;
}

qint32 ConnectionHandling::startGettingUserProps()
{
    this->sendUserPropertiesRequest();
    return ERROR_CODE_SUCCESS;
}

bool ConnectionHandling::startUpdatePassword(QString newPassWord)
{
    this->sendUpdatePasswordRequest(newPassWord);
    return true;
}

qint32 ConnectionHandling::startGettingGamesList()
{
    this->sendGamesListRequest();
    return ERROR_CODE_SUCCESS;
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
        this->m_ctrlMainCon.Stop();
        this->stopDataConnection();
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
        this->m_pGlobalData->setbIsConnected(true);
        this->m_pTimerLoginReset->start();
    } else {
        qWarning().noquote() << QString("Error Login: %1").arg(getErrorCodeString(result));
        this->checkTimeoutResult(result);
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
    this->checkTimeoutResult(result);
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

    if (result == ERROR_CODE_SUCCESS)
        this->m_pGlobalData->uUserProperties = value;
    emit this->sNotifyUserPropertiesRequest(result, value);
    this->checkTimeoutResult(result);
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

void ConnectionHandling::slDataConUpdPassFinished(qint32 result, QString newPassWord)
{
    disconnect(this->m_pDataCon, &DataConnection::notifyUpdPassRequest,
            this, &ConnectionHandling::slDataConUpdPassFinished);

    if (result == ERROR_CODE_SUCCESS) {
        this->m_pGlobalData->setPassWord(newPassWord);
        this->m_pGlobalData->saveGlobalUserSettings();
    }

    emit this->sNotifyUpdatePasswordRequest(result, newPassWord);
    this->checkTimeoutResult(result);
}

/*
 * Functions for getting Games list
 */
void ConnectionHandling::sendGamesListRequest()
{
    this->startDataConnection();
    connect(this->m_pDataCon, &DataConnection::notifyGamesListRequest,
            this, &ConnectionHandling::slDataConGamesListFinished);

    emit this->sStartSendGamesListRequest();
}

void ConnectionHandling::slDataConGamesListFinished(qint32 result)
{
    disconnect(this->m_pDataCon, &DataConnection::notifyGamesListRequest,
            this, &ConnectionHandling::slDataConGamesListFinished);

    emit this->sNotifyGamesListRequest(result);
    this->checkTimeoutResult(result);
}


void ConnectionHandling::checkTimeoutResult(qint32 result)
{
    if (result == ERROR_CODE_TIMEOUT) {
        this->m_ctrlMainCon.Stop();
        this->stopDataConnection();
    } else if (this->m_pGlobalData->bIsConnected())
        this->m_pTimerLoginReset->start();              // restart Timer
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
    connect(this, &ConnectionHandling::sStartSendGamesListRequest,
            this->m_pDataCon, &DataConnection::startSendGamesListRequest);

    this->m_ctrlDataCon.Start(this->m_pDataCon, false);
}

void ConnectionHandling::stopDataConnection()
{
    this->m_pGlobalData->setbIsConnected(false);

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
    disconnect(this, &ConnectionHandling::sStartSendGamesListRequest,
            this->m_pDataCon, &DataConnection::startSendGamesListRequest);

    this->m_ctrlDataCon.Stop();
}

void ConnectionHandling::slTimerConResetFired()
{
    this->m_ctrlMainCon.Stop();
}

void ConnectionHandling::slTimerConLoginFired()
{
    this->m_pGlobalData->setbIsConnected(false);
}

ConnectionHandling::~ConnectionHandling()
{
    if (this->isDataConnectionActive())
        this->stopDataConnection();

    if (this->isMainConnectionActive())
        this->m_ctrlMainCon.Stop();
}
