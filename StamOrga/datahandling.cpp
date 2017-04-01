#include <QtCore/QtEndian>

#include "datahandling.h"
#include "../Common/General/globalfunctions.h"
#include "../Common/General/config.h"

DataHandling::DataHandling()
{

}



qint32 DataHandling::getHandleLoginResponse(MessageProtocol *msg)
{
    if (msg->getDataLength() != 4)
        return ERROR_CODE_WRONG_SIZE;
    return msg->getIntData();
}

qint32 DataHandling::getHandleVersionResponse(MessageProtocol *msg, QString *version)
{
    if (msg->getDataLength() <= 4)
        return ERROR_CODE_WRONG_SIZE;

    const char *pData = msg->getPointerToData();
    quint32 uVersion = qFromBigEndian(*((quint32 *)pData));
    QString remVersion(QByteArray(pData + sizeof(quint32), msg->getDataLength() - sizeof(quint32)));

    if (uVersion > STAM_ORGA_VERSION_I) {
        version->append(QString("Deine Version: %2\nAktuelle Version: %1").arg(remVersion, STAM_ORGA_VERSION_S));
        return ERROR_CODE_NEW_VERSION;
    }
    version->append(remVersion);
    return ERROR_CODE_NO_ERROR;
}

qint32 DataHandling::getHandleUserPropsResponse(MessageProtocol *msg, quint32 *props)
{
    if (msg->getDataLength() != 8)
        return ERROR_CODE_WRONG_SIZE;

    const char *pData = msg->getPointerToData();
    qint32 rValue = qFromBigEndian(*((qint32 *)pData));
    *props = qFromBigEndian(*((quint32 *)(pData + 4)));

    return rValue;
}

