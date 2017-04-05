#include <QtCore/QDataStream>

#include "dataconnection.h"
#include "../Common/General/config.h"
#include "../Common/General/globalfunctions.h"
#include "../Common/Network/messageprotocol.h"
#include "../Common/Network/messagecommand.h"

DataConnection::DataConnection(GlobalData *pData) : BackgroundWorker()
{
    this->SetWorkerName("DataConnection");
    this->m_pGlobalData = pData;
    this->m_bRequestLoginAgain = false;
}


int DataConnection::DoBackgroundWork()
{
    /* Have to do it here or it will not be in the correct thread */
    this->m_pConTimeout = new QTimer();
    this->m_pConTimeout->setSingleShot(true);
    this->m_pConTimeout->setInterval(3000);
    connect(this->m_pConTimeout, &QTimer::timeout, this, &DataConnection::connectionTimeoutFired);

    this->m_pDataHandle = new DataHandling(this->m_pGlobalData);

    this->m_pDataUdpSocket = new QUdpSocket();
    if (!this->m_pDataUdpSocket->bind())
    {
        qCritical().noquote() << QString("Could not bin socket for dataconnection");
//        emit this->connectionRequestFinished(0, this->m_pDataUdpSocket->errorString());
        return -1;
    }
    connect(this->m_pDataUdpSocket, &QUdpSocket::readyRead, this, &DataConnection::readyReadDataPort);

    this->m_hDataReceiver = QHostAddress(this->m_pGlobalData->ipAddr());

    return 0;
}

/*
 * Function/slot to call when new data is ready from udp socket
 */
void DataConnection::readyReadDataPort()
{
    while(this->m_pDataUdpSocket->hasPendingDatagrams()) {
        QHostAddress sender;
        quint16 port;
        QByteArray datagram;
        datagram.resize(this->m_pDataUdpSocket->pendingDatagramSize());

        if (this->m_pDataUdpSocket->readDatagram(datagram.data(), datagram.size(), &sender, &port)) {
            if (this->m_pGlobalData->conDataPort() == port
                && this->m_hDataReceiver.toIPv4Address() == sender.toIPv4Address()) {

                this->m_messageBuffer.StoreNewData(datagram);
            }
        }
    }

    this->checkNewOncomingData();
}

/*
 * Function to check which data was received
 */
void DataConnection::checkNewOncomingData()
{
    MessageProtocol *msg;
    while((msg = this->m_messageBuffer.GetNextMessage()) != NULL) {

        switch (msg->getIndex()) {
        case OP_CODE_CMD_RES::ACK_LOGIN_USER:
            if (this->m_bRequestLoginAgain) {
                this->sendActualRequestsAgain();
                this->m_bRequestLoginAgain = false;
            }
            else
                emit this->notifyLoginRequest(this->m_pDataHandle->getHandleLoginResponse(msg));
            break;

        case OP_CODE_CMD_RES::ACK_GET_VERSION:
        {
            QString version;
            qint32 uVersion = this->m_pDataHandle->getHandleVersionResponse(msg, &version);
            emit this->notifyVersionRequest(uVersion, version);
            break;
        }
        case OP_CODE_CMD_RES::ACK_GET_USER_PROPS:
        {
            quint32 props;
            qint32 rValue = this->m_pDataHandle->getHandleUserPropsResponse(msg, &props);
            emit this->notifyUserPropsRequest(rValue, props);
            break;
        }
        case OP_CODE_CMD_RES::ACK_USER_CHANGE_LOGIN:
        {
            qint32 result = msg->getIntData();
            QVariant passw = this->getActualRequestData(msg->getIndex() & 0x00FFFFFF);
            emit this->notifyUpdPassRequest(result, passw.toString());
            break;
        }
        case OP_CODE_CMD_RES::ACK_NOT_LOGGED_IN:
            if (!this->m_bRequestLoginAgain) {
                this->m_bRequestLoginAgain = true;
                this->startSendLoginRequest();
            }
            break;

        case OP_CODE_CMD_RES::ACK_GET_GAMES_LIST:
        {
            qint32 result = this->m_pDataHandle->getHandleGamesListResponse(msg);
            emit this->notifyGamesListRequest(result);
            break;
        }
        default:
            delete msg;
            continue;
        }
        this->removeActualRequest(msg->getIndex() & 0x00FFFFFF);
        if (this->m_lActualRequest.size() == 0)
            this->m_pConTimeout->stop();
        delete msg;
    }
}

void DataConnection::startSendLoginRequest()
{
    QByteArray aPassw;
    aPassw.append(this->m_pGlobalData->passWord());
    MessageProtocol msg(OP_CODE_CMD_REQ::REQ_LOGIN_USER, aPassw);
    if (this->sendMessageRequest(&msg) < 0)
        emit this->notifyLoginRequest(ERROR_CODE_ERR_SEND);
}

void DataConnection::startSendVersionRequest()
{
    QByteArray version;
    QDataStream wVersion(&version, QIODevice::WriteOnly);
    wVersion.setByteOrder(QDataStream::BigEndian);
    wVersion << (quint32)STAM_ORGA_VERSION_I;

    version.append(QString(STAM_ORGA_VERSION_S));
    MessageProtocol msg(OP_CODE_CMD_REQ::REQ_GET_VERSION, version);
    if (this->sendMessageRequest(&msg) < 0)
        emit this->notifyVersionRequest(ERROR_CODE_ERR_SEND, "");
}

void DataConnection::startSendUserPropsRequest()
{
    MessageProtocol msg(OP_CODE_CMD_REQ::REQ_GET_USER_PROPS);
    if (this->sendMessageRequest(&msg) < 0)
        emit this->notifyUserPropsRequest(ERROR_CODE_ERR_SEND, 0);
}

void DataConnection::startSendUpdPassRequest(QString newPassWord)
{
    QByteArray passReq;
    QDataStream wPassReq(&passReq, QIODevice::WriteOnly);
    wPassReq.setByteOrder(QDataStream::BigEndian);

    wPassReq << (qint16)this->m_pGlobalData->passWord().size();
    passReq.append(this->m_pGlobalData->passWord());
    wPassReq.device()->seek(passReq.length());
    wPassReq << (qint16)newPassWord.size();
    passReq.append(newPassWord);

    QVariant tmp(newPassWord);
    MessageProtocol msg(OP_CODE_CMD_REQ::REQ_USER_CHANGE_LOGIN, passReq);
    if (this->sendMessageRequest(&msg, &tmp) < 0)
        emit this->notifyUpdPassRequest(ERROR_CODE_ERR_SEND, "");
}

void DataConnection::startSendGamesListRequest()
{
    MessageProtocol msg(OP_CODE_CMD_REQ::REQ_GET_GAMES_LIST);
    if (this->sendMessageRequest(&msg) < 0)
        emit this->notifyGamesListRequest(ERROR_CODE_ERR_SEND);
}

void DataConnection::connectionTimeoutFired()
{
    qDebug() << "Timeout from Data UdpServer";
    while(this->m_lActualRequest.size() > 0) {
        switch(this->m_lActualRequest.last().request) {
        case OP_CODE_CMD_REQ::REQ_LOGIN_USER:
            emit this->notifyLoginRequest(ERROR_CODE_NO_ANSWER);
            break;

        case OP_CODE_CMD_REQ::REQ_GET_VERSION:
            emit this->notifyVersionRequest(ERROR_CODE_NO_ANSWER, "");
            break;

        case OP_CODE_CMD_REQ::REQ_GET_USER_PROPS:
            emit this->notifyUserPropsRequest(ERROR_CODE_NO_ANSWER, 0);
            break;

        case OP_CODE_CMD_REQ::REQ_GET_GAMES_LIST:
            emit this->notifyGamesListRequest(ERROR_CODE_NO_ANSWER);
        }
        this->m_lActualRequest.removeLast();
    }
    this->m_bRequestLoginAgain = false;
}

qint32 DataConnection::sendMessageRequest(MessageProtocol *msg, QVariant *data)
{
    if (this->m_pDataUdpSocket == NULL)
        return -1;

    const char *pData = msg->getNetworkProtocol();
    qint32 rValue = this->m_pDataUdpSocket->writeDatagram(pData,
                                                          msg->getNetworkSize(),
                                                          this->m_hDataReceiver,
                                                          this->m_pGlobalData->conDataPort());
    this->m_pConTimeout->start();

    /* Only add when not sending request again */
    if (!this->m_bRequestLoginAgain) {
        ActualRequest actReq;
        actReq.request = msg->getIndex();
        if (data != NULL)
            actReq.data = *data;
        this->m_lActualRequest.append(actReq);
    }

    return rValue;
}

void DataConnection::removeActualRequest(quint32 req)
{
    for (int i=0; i<this->m_lActualRequest.size(); i++) {
        if (this->m_lActualRequest[i].request == req)
            this->m_lActualRequest.removeAt(i);
    }
}
QVariant DataConnection::getActualRequestData(quint32 req)
{
    for (int i=0; i<this->m_lActualRequest.size(); i++) {
        if (this->m_lActualRequest[i].request == req)
            return this->m_lActualRequest[i].data;
    }
    return QVariant(0);
}

void DataConnection::sendActualRequestsAgain()
{
    for (int i=0; i<this->m_lActualRequest.size(); i++) {
        switch (this->m_lActualRequest[i].request) {
        case OP_CODE_CMD_REQ::REQ_GET_VERSION:
            this->startSendVersionRequest();
            break;

        case OP_CODE_CMD_REQ::REQ_GET_USER_PROPS:
            this->startSendUserPropsRequest();
            break;

        case OP_CODE_CMD_REQ::REQ_USER_CHANGE_LOGIN:
            this->startSendUpdPassRequest(this->m_lActualRequest[i].data.toString());
            break;

        case OP_CODE_CMD_REQ::REQ_GET_GAMES_LIST:
            this->startSendGamesListRequest();
            break;

        default:
            continue;
        }
    }
}

DataConnection::~DataConnection()
{
    if (this->m_pDataUdpSocket != NULL)
        delete this->m_pDataUdpSocket;

    if (this->m_pDataHandle != NULL)
        delete this->m_pDataHandle;
}
