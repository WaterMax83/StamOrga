#include <QtCore/QDataStream>

#include "dataconnection.h"
#include "../Common/General/globalfunctions.h"
#include "../Common/General/config.h"
#include "../Common/Network/messagecommand.h"

DataConnection::DataConnection(GlobalData *pGData, QObject *parent) : QObject(parent)
{
    this->m_pGlobalData = pGData;
}

/*
 * 0                Header          12
 * 12   String      password        X
 */
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

/*
 * 0                Header          12
 */
MessageProtocol *DataConnection::requestGetUserProperties()
{
    QByteArray answer;
    QDataStream wAnswer(&answer, QIODevice::WriteOnly);
    wAnswer.setByteOrder(QDataStream::BigEndian);
    wAnswer << ERROR_CODE_SUCCESS;
    wAnswer << this->m_pGlobalData->m_UserList.getUserProperties(this->m_pUserConData->userName);

    MessageProtocol *ack = new MessageProtocol(OP_CODE_CMD_RES::ACK_GET_USER_PROPS, answer);

    return ack;
}

/*
 * 0                    Header          12
 * 12       quint16     size            2
 * 14       String      actual Passw    X
 * 14+X     quint16     size            2
 * 16+X     String      new Passw       Y
 */
MessageProtocol *DataConnection::requestUserChangeLogin(MessageProtocol *msg)
{
    MessageProtocol *ack = NULL;
    if (msg->getDataLength() <= 8) {
        qWarning() << QString("Getting no version data from %1").arg(this->m_pUserConData->userName);
        return NULL;
    }

    qint32 totalLength = (qint32)msg->getDataLength();
    const char *pData = msg->getPointerToData();

    quint16 actLength = qFromBigEndian(*((quint16 *)pData));
    if (actLength + 2 > totalLength)
        return new MessageProtocol(OP_CODE_CMD_RES::ACK_USER_CHANGE_LOGIN, ERROR_CODE_WRONG_SIZE);
    QString actPassw(QByteArray(pData + 2, actLength));

    quint16 newLength = qFromBigEndian(*((quint16 *)(pData + 2 + actLength)));
    if (newLength + actLength + 2 + 2 > totalLength)
        return new MessageProtocol(OP_CODE_CMD_RES::ACK_USER_CHANGE_LOGIN, ERROR_CODE_WRONG_SIZE);
    QString newPassw(QByteArray(pData + 2 + actLength + 2, newLength));

    if (!this->m_pGlobalData->m_UserList.userCheckPassword(this->m_pUserConData->userName, actPassw))
        return new MessageProtocol(OP_CODE_CMD_RES::ACK_USER_CHANGE_LOGIN, ERROR_CODE_WRONG_PASSWORD);

    if (this->m_pGlobalData->m_UserList.userChangePassword(this->m_pUserConData->userName, newPassw))
        ack = new MessageProtocol(OP_CODE_CMD_RES::ACK_USER_CHANGE_LOGIN, ERROR_CODE_SUCCESS);
    else
        ack = new MessageProtocol(OP_CODE_CMD_RES::ACK_USER_CHANGE_LOGIN, ERROR_CODE_COMMON);

    return ack;
}

/*
 * 0                Header          12
 * 12   quint32     version         4
 * 16   String      version         X
 */
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
