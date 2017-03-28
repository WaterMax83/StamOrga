#include "dataconnection.h"
#include "../Common/General/globalfunctions.h"
#include "../Common/Network/messagecommand.h"

DataConnection::DataConnection(GlobalData *pGData, QObject *parent) : QObject(parent)
{
    this->m_pGlobalData = pGData;
}


MessageProtocol *DataConnection::requestCheckUserLogin(MessageProtocol *msg, UserConData *pUserConData)
{
    MessageProtocol *ack = NULL;
    QString passw(QByteArray(msg->getPointerToData(), msg->getDataLength()));
    if (this->m_pGlobalData->m_UserList.userCheckPassword(pUserConData->userName, passw)) {
        ack = new MessageProtocol(OP_CODE_CMD_RES::ACK_LOGIN_USER, ERROR_CODE_SUCCESS);
        pUserConData->bIsConnected = true;
        qInfo().noquote() << QString("User %1 logged in").arg(pUserConData->userName);
    }
    else {
        ack = new MessageProtocol(OP_CODE_CMD_RES::ACK_LOGIN_USER, ERROR_CODE_WRONG_PASSWORD);
        pUserConData->bIsConnected = false;
    }
    return ack;
}


MessageProtocol *DataConnection::requestGetProgramVersion(MessageProtocol *msg)
{

}
