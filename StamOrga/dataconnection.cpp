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
    this->m_pConTimeout->setInterval(10000);
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

        DataConRequest request = this->getActualRequest(msg->getIndex() & 0x00FFFFFF);
        if (request.m_request == 0)
            continue;

        switch (msg->getIndex()) {
        case OP_CODE_CMD_RES::ACK_LOGIN_USER:
            if (this->m_bRequestLoginAgain) {
                this->sendActualRequestsAgain();
                this->m_bRequestLoginAgain = false;
                continue;
            }
            else {
                request.m_result = this->m_pDataHandle->getHandleLoginResponse(msg);
            }

            break;

        case OP_CODE_CMD_RES::ACK_GET_VERSION:
            request.m_result = this->m_pDataHandle->getHandleVersionResponse(msg, &request.m_returnData);
            break;

        case OP_CODE_CMD_RES::ACK_GET_USER_PROPS:
            request.m_result = this->m_pDataHandle->getHandleUserPropsResponse(msg,& request.m_returnData);
            break;

        case OP_CODE_CMD_RES::ACK_USER_CHANGE_LOGIN:
            request.m_result = msg->getIntData();
            if (request.m_lData.size() > 0)
                request.m_returnData = request.m_lData.at(0);
            break;

        case OP_CODE_CMD_RES::ACK_USER_CHANGE_READNAME:
            request.m_result = msg->getIntData();
            if (request.m_lData.size() > 0)
                request.m_returnData = request.m_lData.at(0);
            break;

        case OP_CODE_CMD_RES::ACK_NOT_LOGGED_IN:
            if (!this->m_bRequestLoginAgain) {
                this->m_bRequestLoginAgain = true;
                DataConRequest conReq;
                conReq.m_request = OP_CODE_CMD_REQ::REQ_LOGIN_USER;
                this->startSendLoginRequest(conReq);
            }
            break;

        case OP_CODE_CMD_RES::ACK_GET_GAMES_LIST:
            request.m_result = this->m_pDataHandle->getHandleGamesListResponse(msg);
            break;

        case OP_CODE_CMD_RES::ACK_ADD_TICKET:
            request.m_result = msg->getIntData();;
            break;

        case OP_CODE_CMD_RES::ACK_REMOVE_TICKET:
            request.m_result = msg->getIntData();
            break;

        case OP_CODE_CMD_RES::ACK_GET_TICKETS_LIST:
            request.m_result = this->m_pDataHandle->getHandleSeasonTicketListResponse(msg);
            break;

        default:
            delete msg;
            continue;
        }
        emit this->notifyLastRequestFinished(request);
        this->removeActualRequest(request.m_request);
        if (this->m_lActualRequest.size() == 0)
            this->m_pConTimeout->stop();
        delete msg;
    }
}

void DataConnection::startSendLoginRequest(DataConRequest request)
{
    QString passWord = this->m_pGlobalData->passWord();
    QByteArray aPassw;
    QDataStream wPassword(&aPassw, QIODevice::WriteOnly);
    wPassword.setByteOrder(QDataStream::BigEndian);
    wPassword << quint16(passWord.toUtf8().size());
    aPassw.append(passWord);
    MessageProtocol msg(OP_CODE_CMD_REQ::REQ_LOGIN_USER, aPassw);
    this->sendMessageRequest(&msg, request);
}

void DataConnection::startSendVersionRequest(DataConRequest request)
{
    QByteArray version;
    QDataStream wVersion(&version, QIODevice::WriteOnly);
    wVersion.setByteOrder(QDataStream::BigEndian);
    wVersion << (quint32)STAM_ORGA_VERSION_I;
    wVersion << quint16(QString(STAM_ORGA_VERSION_S).toUtf8().size());

    version.append(QString(STAM_ORGA_VERSION_S));
    MessageProtocol msg(OP_CODE_CMD_REQ::REQ_GET_VERSION, version);
    this->sendMessageRequest(&msg, request);
}

void DataConnection::startSendUserPropsRequest(DataConRequest request)
{
    MessageProtocol msg(OP_CODE_CMD_REQ::REQ_GET_USER_PROPS);
    this->sendMessageRequest(&msg, request);
}

void DataConnection::startSendUpdPassRequest(DataConRequest request)
{
    QByteArray passReq;
    QDataStream wPassReq(&passReq, QIODevice::WriteOnly);
    wPassReq.setByteOrder(QDataStream::BigEndian);

    QString newPassWord = request.m_lData.at(0);

    wPassReq << (qint16)this->m_pGlobalData->passWord().size();
    passReq.append(this->m_pGlobalData->passWord());
    wPassReq.device()->seek(passReq.length());
    wPassReq << (qint16)newPassWord.size();
    passReq.append(newPassWord);

    MessageProtocol msg(OP_CODE_CMD_REQ::REQ_USER_CHANGE_LOGIN, passReq);
    this->sendMessageRequest(&msg, request);
}

void DataConnection::startSendReadableNameRequest(DataConRequest request)
{
    QString name = request.m_lData.at(0);
    QByteArray readName;
    QDataStream wReadName(&readName, QIODevice::WriteOnly);
    wReadName.setByteOrder(QDataStream::BigEndian);
    wReadName << quint16(name.toUtf8().size());
    readName.append(name);

    MessageProtocol msg(OP_CODE_CMD_REQ::REQ_USER_CHANGE_READNAME, readName);
    this->sendMessageRequest(&msg, request);
}

void DataConnection::startSendGamesListRequest(DataConRequest request)
{
    MessageProtocol msg(OP_CODE_CMD_REQ::REQ_GET_GAMES_LIST);
    this->sendMessageRequest(&msg, request);
}

void DataConnection::startSendAddSeasonTicket(DataConRequest request)
{
    QString name = request.m_lData.at(1);
    QByteArray seasonTicket;
    QDataStream wSeasonTicket(&seasonTicket, QIODevice::WriteOnly);
    wSeasonTicket.setByteOrder(QDataStream::BigEndian);
    wSeasonTicket << request.m_lData.at(1).toUInt();
    wSeasonTicket << quint16(name.toUtf8().size());
    seasonTicket.append(name);

    MessageProtocol msg(OP_CODE_CMD_REQ::REQ_ADD_TICKET, seasonTicket);
    this->sendMessageRequest(&msg, request);
}

void DataConnection::startSendRemoveSeasonTicket(DataConRequest request)
{
    QString name = request.m_lData.at(0);
    QByteArray seasonTicket;
    QDataStream wSeasonTicket(&seasonTicket, QIODevice::WriteOnly);
    wSeasonTicket.setByteOrder(QDataStream::BigEndian);
    wSeasonTicket << quint16(name.toUtf8().size());
    seasonTicket.append(name);

    MessageProtocol msg(OP_CODE_CMD_REQ::REQ_REMOVE_TICKET, seasonTicket);
    this->sendMessageRequest(&msg, request);
}

void DataConnection::startSendSeasonTicketListRequest(DataConRequest request)
{
    MessageProtocol msg(OP_CODE_CMD_REQ::REQ_GET_TICKETS_LIST);
    this->sendMessageRequest(&msg, request);
}

void DataConnection::connectionTimeoutFired()
{
    qDebug() << "Timeout from Data UdpServer";
    while(this->m_lActualRequest.size() > 0) {
        DataConRequest request = this->m_lActualRequest.last();
        request.m_result = ERROR_CODE_TIMEOUT;
        emit this->notifyLastRequestFinished(request);
        this->m_lActualRequest.removeLast();
    }
    this->m_bRequestLoginAgain = false;
}

qint32 DataConnection::sendMessageRequest(MessageProtocol *msg, DataConRequest request)
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
        this->m_lActualRequest.append(request);
    }

    if (rValue < 0) {
        request.m_result = ERROR_CODE_ERR_SEND;
        emit this->notifyLastRequestFinished(request);
    }

    return rValue;
}

void DataConnection::removeActualRequest(quint32 req)
{
    for (int i=0; i<this->m_lActualRequest.size(); i++) {
        if (this->m_lActualRequest[i].m_request == req) {
//            if (this->m_lActualRequest[i].m_lData != NULL)
//                delete this->m_lActualRequest[i].m_lData;
            this->m_lActualRequest.removeAt(i);
        }
    }
}


DataConRequest DataConnection::getActualRequest(quint32 req)
{
    for (int i=0; i<this->m_lActualRequest.size(); i++) {
        if (this->m_lActualRequest[i].m_request == req) {
            return this->m_lActualRequest[i];
        }
    }
    return DataConRequest(0);
}

QString DataConnection::getActualRequestData(quint32 req, qint32 index)
{
    for (int i=0; i<this->m_lActualRequest.size(); i++) {
        if (this->m_lActualRequest[i].m_request == req) {
            if (this->m_lActualRequest[i].m_lData.size() > index)
                return this->m_lActualRequest[i].m_lData.at(index);
        }
    }
    return NULL;
}

void DataConnection::sendActualRequestsAgain()
{
    for (int i=0; i<this->m_lActualRequest.size(); i++) {
        this->startSendNewRequest(this->m_lActualRequest[i]);
    }
}

void DataConnection::startSendNewRequest(DataConRequest request)
{
    switch (request.m_request) {
    case OP_CODE_CMD_REQ::REQ_LOGIN_USER:
        this->startSendLoginRequest(request);
        break;

    case OP_CODE_CMD_REQ::REQ_GET_VERSION:
        this->startSendVersionRequest(request);
        break;

    case OP_CODE_CMD_REQ::REQ_GET_USER_PROPS:
        this->startSendUserPropsRequest(request);
        break;

    case OP_CODE_CMD_REQ::REQ_USER_CHANGE_LOGIN:
        if (request.m_lData.size() > 0)
            this->startSendUpdPassRequest(request);
        break;

    case OP_CODE_CMD_REQ::REQ_USER_CHANGE_READNAME:
        if (request.m_lData.size() > 0)
            this->startSendReadableNameRequest(request);
        break;

    case OP_CODE_CMD_REQ::REQ_GET_GAMES_LIST:
        this->startSendGamesListRequest(request);
        break;

    case OP_CODE_CMD_REQ::REQ_ADD_TICKET:
        if (request.m_lData.size() > 1)
            this->startSendAddSeasonTicket(request);
        break;

    case OP_CODE_CMD_REQ::REQ_REMOVE_TICKET:
        if (request.m_lData.size() > 0)
            this->startSendRemoveSeasonTicket(request);
        break;

    case OP_CODE_CMD_REQ::REQ_GET_TICKETS_LIST:
        this->startSendSeasonTicketListRequest(request);
        break;

    default:
        return;
    }

}

DataConnection::~DataConnection()
{
    if (this->m_pDataUdpSocket != NULL)
        delete this->m_pDataUdpSocket;

    if (this->m_pDataHandle != NULL)
        delete this->m_pDataHandle;
}
