
#include <QDebug>

#include "connectionhandling.h"
#include "../Common/Network/messageprotocol.h"
#include "../Common/General/globalfunctions.h"
#include "../Common/General/globaltiming.h"
#include "../Common/Network/messagecommand.h"

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
                qDebug() << "Did not log in again, should already be succesfull";
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
    this->startDataConnection();
    DataConRequest req(OP_CODE_CMD_REQ::REQ_GET_VERSION);
    emit this->sStartSendNewRequest(req);
    return ERROR_CODE_SUCCESS;
}

qint32 ConnectionHandling::startGettingUserProps()
{
    this->startDataConnection();
    DataConRequest req(OP_CODE_CMD_REQ::REQ_GET_USER_PROPS);
    emit this->sStartSendNewRequest(req);
    return ERROR_CODE_SUCCESS;
}

bool ConnectionHandling::startUpdatePassword(QString newPassWord)
{
    this->startDataConnection();
    DataConRequest req(OP_CODE_CMD_REQ::REQ_USER_CHANGE_LOGIN);
    req.m_lData.append(newPassWord);
    emit this->sStartSendNewRequest(req);
    return true;
}

qint32 ConnectionHandling::startUpdateReadableName(QString name)
{
    this->startDataConnection();
    DataConRequest req(OP_CODE_CMD_REQ::REQ_USER_CHANGE_READNAME);
    req.m_lData.append(name);
    emit this->sStartSendNewRequest(req);
    return ERROR_CODE_SUCCESS;
}

qint32 ConnectionHandling::startGettingGamesList()
{
//    this->sendGamesListRequest();

    this->startDataConnection();
    DataConRequest req(OP_CODE_CMD_REQ::REQ_GET_GAMES_LIST);
    emit this->sStartSendNewRequest(req);
    return ERROR_CODE_SUCCESS;
}

qint32 ConnectionHandling::startSeasonTicketRemove(QString name)
{
    this->startDataConnection();
    DataConRequest req(OP_CODE_CMD_REQ::REQ_REMOVE_TICKET);
    req.m_lData.append(name);
    emit this->sStartSendNewRequest(req);
    return ERROR_CODE_SUCCESS;
}

qint32 ConnectionHandling::startSeasonTicketAdd(QString name, quint32 discount)
{
    this->startDataConnection();
    DataConRequest req(OP_CODE_CMD_REQ::REQ_ADD_TICKET);
    req.m_lData.append(QString::number(discount));
    req.m_lData.append(name);
    emit this->sStartSendNewRequest(req);
    return ERROR_CODE_SUCCESS;
}

qint32 ConnectionHandling::startGettingSeasonTicketList()
{
    this->startDataConnection();
    DataConRequest req(OP_CODE_CMD_REQ::REQ_GET_TICKETS_LIST);
    emit this->sStartSendNewRequest(req);
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

    DataConRequest req;
    req.m_request = OP_CODE_CMD_REQ::REQ_LOGIN_USER;

    emit this->sStartSendNewRequest(req);
}


void ConnectionHandling::slDataConLastRequestFinished(DataConRequest request)
{
    switch(request.m_request) {
    case OP_CODE_CMD_REQ::REQ_LOGIN_USER:
            if (request.m_result == ERROR_CODE_SUCCESS) {
                qInfo().noquote() << "Logged in succesfully";
                this->m_pGlobalData->setbIsConnected(true);
            } else {
                qWarning().noquote() << QString("Error Login: %1").arg(getErrorCodeString(request.m_result));

            }

            emit this->sNotifyConnectionFinished(request.m_result);
        break;

    case OP_CODE_CMD_REQ::REQ_GET_VERSION:
        qDebug() << QString("Answer from Version Info");
        emit this->sNotifyVersionRequest(request.m_result, request.m_returnData);
        break;

    case OP_CODE_CMD_REQ::REQ_GET_USER_PROPS:
            if (request.m_result == ERROR_CODE_SUCCESS)
                this->m_pGlobalData->uUserProperties = request.m_returnData.toUInt();
            emit this->sNotifyUserPropertiesRequest(request.m_result, request.m_returnData.toUInt());
        break;

    case OP_CODE_CMD_REQ::REQ_USER_CHANGE_LOGIN:
            if (request.m_result == ERROR_CODE_SUCCESS) {
                this->m_pGlobalData->setPassWord(request.m_returnData);
                this->m_pGlobalData->saveGlobalUserSettings();
            }

            emit this->sNotifyUpdatePasswordRequest(request.m_result, request.m_returnData);
        break;

    case OP_CODE_CMD_REQ::REQ_USER_CHANGE_READNAME:
            if (request.m_result == ERROR_CODE_SUCCESS) {
                this->m_pGlobalData->setReadableName(request.m_returnData);
                this->m_pGlobalData->saveGlobalUserSettings();
            }

            emit this->sNotifyUpdateReadableNameRequest(request.m_result);
        break;

    case OP_CODE_CMD_REQ::REQ_GET_GAMES_LIST:
        emit this->sNotifyGamesListRequest(request.m_result);
        break;

    case OP_CODE_CMD_REQ::REQ_ADD_TICKET:
        emit this->sNotifySeasonTicketAddRequest(request.m_result);
        break;

    case OP_CODE_CMD_REQ::REQ_REMOVE_TICKET:
        emit this->sNotifySeasonTicketRemoveRequest(request.m_result);
        break;

    case OP_CODE_CMD_REQ::REQ_GET_TICKETS_LIST:
        emit this->sNotifySeasonTicketListRequest(request.m_result);
        break;

    }

    this->checkTimeoutResult(request.m_result);
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

    connect(this, &ConnectionHandling::sStartSendNewRequest,
            this->m_pDataCon, &DataConnection::startSendNewRequest);
    connect(this->m_pDataCon, &DataConnection::notifyLastRequestFinished,
            this, &ConnectionHandling::slDataConLastRequestFinished);

    this->m_ctrlDataCon.Start(this->m_pDataCon, false);
}

void ConnectionHandling::stopDataConnection()
{
    this->m_pGlobalData->setbIsConnected(false);

    if (!this->isDataConnectionActive())
        return;

    disconnect(this, &ConnectionHandling::sStartSendNewRequest,
            this->m_pDataCon, &DataConnection::startSendNewRequest);
    disconnect(this->m_pDataCon, &DataConnection::notifyLastRequestFinished,
            this, &ConnectionHandling::slDataConLastRequestFinished);

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
