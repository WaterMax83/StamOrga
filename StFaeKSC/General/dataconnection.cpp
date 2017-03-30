#include <QtCore/QDataStream>

#include "dataconnection.h"
#include "../Common/General/globalfunctions.h"
#include "../Common/General/config.h"
#include "../Common/Network/messagecommand.h"

DataConnection::DataConnection(GlobalData *pGData, QObject *parent) : QObject(parent)
{
    this->m_pGlobalData = pGData;
}


MessageProtocol *DataConnection::requestCheckUserLogin(MessageProtocol *msg)
{
    MessageProtocol *ack = NULL;
    QString passw(QByteArray(msg->getPointerToData(), msg->getDataLength()));
    if (this->m_pGlobalData->m_UserList.userCheckPassword(this->m_pUserConData->userName, passw)) {
        ack = new MessageProtocol(OP_CODE_CMD_RES::ACK_LOGIN_USER, ERROR_CODE_SUCCESS);
        this->m_pUserConData->bIsConnected = true;
        qInfo().noquote() << QString("User %1 logged in").arg(this->m_pUserConData->userName);
    }
    else {
        ack = new MessageProtocol(OP_CODE_CMD_RES::ACK_LOGIN_USER, ERROR_CODE_WRONG_PASSWORD);
        this->m_pUserConData->bIsConnected = false;
    }
    return ack;
}


MessageProtocol *DataConnection::requestGetProgramVersion(MessageProtocol *msg)
{
    if (msg->getDataLength() <= 4) {
        qWarning() << QString("Getting no version data from %1").arg(this->m_pUserConData->userName);
        return NULL;
    }

    QString remVersion(QByteArray(msg->getPointerToData() + sizeof(quint32), msg->getDataLength() - sizeof(quint32)));
    qInfo().noquote() << QString("Version from %1 = %2").arg(this->m_pUserConData->userName).arg(remVersion);
    QByteArray ownVersion;
    QDataStream wVersion(&ownVersion, QIODevice::WriteOnly);
    wVersion.setByteOrder(QDataStream::BigEndian);
    wVersion << (quint32)STAM_ORGA_VERSION_I;

    ownVersion.append(QString(STAM_ORGA_VERSION_S));
    return new MessageProtocol(OP_CODE_CMD_RES::ACK_GET_VERSION, ownVersion);
}
