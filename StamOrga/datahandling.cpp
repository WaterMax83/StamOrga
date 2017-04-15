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

qint32 DataHandling::getHandleUserPropsResponse(MessageProtocol *msg, QString *props)
{
    if (msg->getDataLength() < 12)
        return ERROR_CODE_WRONG_SIZE;

    const char *pData = msg->getPointerToData();
    qint32 rValue = qFromBigEndian(*((qint32 *)pData));
    *props = QString::number(qFromBigEndian(*((quint32 *)(pData + 4))));

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
	quint16 version = qFromBigEndian(*(qint16 *)(pData + offset));
	if (version != 0x1) {
		qWarning().noquote() << QString("Unknown game version %1").arg(version);
		return -1;
	}
	offset += 2;
	
    quint16 totalPacks = qFromBigEndian(*(quint16 *)(pData + offset));
    offset += 2;

    this->m_pGlobalData->startUpdateGamesPlay();
    while(offset < totalSize && totalPacks > 0) {
        GamePlay *play = new GamePlay();
        quint16 size = qFromBigEndian(*(qint16 *)(pData + offset));
        offset += 2;

        if (size <= 8) {
            qWarning().noquote() << QString("Size is to small %1").arg(size);
            delete play;
            break;
        }

        play->setIndex(*(qint8 *)(pData + offset));
        offset += 1;
        play->setCompetition(*(qint8 *)(pData + offset));
        offset += 1;

        /* On Android there are problems reading from qint64 pointers???? SIGBUS*/
//        play->setTimeStamp(qFromBigEndian(*(qint64 *)(pData + offset)));
        quint32 tmp = qFromBigEndian(*(qint32 *)(pData + offset));
        qint64 timeStamp = qint64(tmp) << 32;
        tmp = qFromBigEndian(*(quint32 *)(pData + offset + 4));
        timeStamp |= qint64(tmp);

        play->setTimeStamp(timeStamp);
        offset += 8;

        QString playString(QByteArray(pData + offset, size - 10));
        offset += (size - 10);
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

qint32 DataHandling::getHandleSeasonTicketListResponse(MessageProtocol *msg)
{
    if (msg->getDataLength() < 8)
        return ERROR_CODE_WRONG_SIZE;

    const char *pData = msg->getPointerToData();
    qint32 rValue = qFromBigEndian(*((qint32 *)pData));

    if (rValue != ERROR_CODE_SUCCESS)
        return rValue;

    quint32 totalSize = msg->getDataLength();
    quint32 offset = 4;
    quint16 version = qFromBigEndian(*(qint16 *)(pData + offset));
    if (version != 0x1) {
        qWarning().noquote() << QString("Unknown season ticket version %1").arg(version);
        return -1;
    }
    offset += 2;

    quint16 totalPacks = qFromBigEndian(*(quint16 *)(pData + offset));
    offset += 2;

    qDebug() << QString("Getting %1 number of tickets").arg(totalPacks);

    this->m_pGlobalData->startUpdateSeasonTickets();
    while(offset < totalSize && totalPacks > 0) {
        SeasonTicket *sTicket = new SeasonTicket();
        quint16 size = qFromBigEndian(*(qint16 *)(pData + offset));
        offset += 2;

        if (size <= 8) {
            qWarning().noquote() << QString("Size is to small %1").arg(size);
            delete sTicket;
            break;
        }

        sTicket->setDiscount(*(qint8 *)(pData + offset));
        offset += 1;
        quint8 IsOwnTicket = (*(qint8 *)(pData + offset));
        offset += 1;

        QString ticketString(QByteArray(pData + offset, size - 2));
        offset += (size - 2);
        QStringList lsticketString = ticketString.split(";");

        if (lsticketString.size() > 0)
            sTicket->setName(lsticketString.value(0));
        if (lsticketString.size() > 1)
            sTicket->setPlace(lsticketString.value(1));

        this->m_pGlobalData->addNewSeasonTicket(sTicket);
        totalPacks--;
    }

    this->m_pGlobalData->saveCurrentSeasonTickets();

    return rValue;
}

