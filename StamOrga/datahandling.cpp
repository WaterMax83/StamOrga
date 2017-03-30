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
    quint32 uVersion = *((quint32 *)pData);
    QString remVersion(QByteArray(pData + sizeof(quint32), msg->getDataLength() - sizeof(quint32)));

    if (uVersion > STAM_ORGA_VERSION_I) {
        version->append(QString("There is a new version available: %1, your version is %2").arg(remVersion, STAM_ORGA_VERSION_S));
        return ERROR_CODE_NEW_VERSION;
    }
    version->append(remVersion);
    return ERROR_CODE_NO_ERROR;
}
