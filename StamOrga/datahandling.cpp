#include <QtCore/QtEndian>

#include "datahandling.h"
#include "../Data/gameplay.h"
#include "../Common/General/globalfunctions.h"
#include "../Common/General/config.h"

DataHandling::DataHandling(GlobalData *pData)
{
    this->m_pGlobalData = pData;
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

    quint16 size = qFromBigEndian(*((quint16 *)(pData + 4)));
    if (size > msg->getDataLength())
        return ERROR_CODE_WRONG_SIZE;

    QString remVersion(QByteArray(pData + 6, size));

    if (uVersion > STAM_ORGA_VERSION_I) {
        version->append(QString("Deine Version: %2\nAktuelle Version: %1").arg(remVersion, STAM_ORGA_VERSION_S));
        return ERROR_CODE_NEW_VERSION;
    }
    version->append(remVersion);
    return ERROR_CODE_NO_ERROR;
}

qint32 DataHandling::getHandleUserPropsResponse(MessageProtocol *msg, quint32 *props)
{
    if (msg->getDataLength() < 12)
        return ERROR_CODE_WRONG_SIZE;

    const char *pData = msg->getPointerToData();
    qint32 rValue = qFromBigEndian(*((qint32 *)pData));
    *props = qFromBigEndian(*((quint32 *)(pData + 4)));

    quint16 readableNameSize = qFromBigEndian(*((quint16 *)(pData + 8)));
    QString readableName(QByteArray(pData + 10, readableNameSize));

    this->m_pGlobalData->setReadableName(readableName);

    return rValue;
}

qint32 DataHandling::getHandleGamesListResponse(MessageProtocol *msg)
{
    if (msg->getDataLength() < 8)
        return ERROR_CODE_WRONG_SIZE;

    const char *pData = msg->getPointerToData();
    qint32 rValue = qFromBigEndian(*((qint32 *)pData));

    if (rValue != ERROR_CODE_SUCCESS)
        return rValue;

    quint32 totalSize = msg->getDataLength();
    quint32 offset = 4;
    quint16 totalPacks = qFromBigEndian(*(quint16 *)(pData + offset));

    offset += 2;

    this->m_pGlobalData->startUpdateGamesPlay();
    while(offset < totalSize && totalPacks > 0) {
        GamePlay *play = new GamePlay();
        quint16 size = qFromBigEndian(*(qint16 *)(pData + offset));
        offset += 2;

        if (size <= 8) {
            qWarning().noquote() << QString("Size is to small %1").arg(size);
            break;
        }

        quint16 version = qFromBigEndian(*(qint16 *)(pData + offset));
        if (version != 0x1) {
            qWarning().noquote() << QString("Unknown game version %1").arg(version);
            break;
        }
        offset += 2;
        play->setIndex(*(qint8 *)(pData + offset));
        offset += 1;
        play->setCompetition(*(qint8 *)(pData + offset));
        offset += 1;


        play->setTimeStamp(qFromBigEndian(*(qint64 *)(pData + offset)));
        offset += 8;

        QString playString(QByteArray(pData + offset, size - 8));
        offset += (size - 12);
        QStringList lplayString = playString.split(";");

        if (lplayString.size() > 0)
            play->setHome(lplayString.value(0));
        if (lplayString.size() > 1)
            play->setAway(lplayString.value(1));
        if (lplayString.size() > 2)
            play->setScore(lplayString.value(2));

        this->m_pGlobalData->addNewGamePlay(play);
        totalPacks--;
    }

    this->m_pGlobalData->saveActualGamesList();

    return rValue;
}

