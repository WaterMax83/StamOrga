#include <QtCore/QDateTime>

#include "../Common/General/globalfunctions.h"
#include "seasonticket.h"

SeasonTicket::SeasonTicket()
{
    QString ticketSetFilePath = getUserHomeConfigPath() + "/Settings/SeasonTicket.ini";

    if (!checkFilePathExistAndCreate(ticketSetFilePath)) {
        CONSOLE_CRITICAL(QString("Could not create File for SeasonTicket setting"));
        return;
    }

    this->m_pConfigSettings = new QSettings(ticketSetFilePath, QSettings::IniFormat);

    /* Check wheter we have to save data after reading again */
    bool bProblems = false;
    {
        QMutexLocker locker(&this->m_mConfigIniMutex);

        this->m_pConfigSettings->beginGroup(GROUP_LIST_ITEM);
        int sizeOfTicket = this->m_pConfigSettings->beginReadArray(CONFIG_LIST_ARRAY);

        for (int i = 0; i < sizeOfTicket; i++) {
            this->m_pConfigSettings->setArrayIndex(i);
            QString ticketName      = this->m_pConfigSettings->value(ITEM_NAME, "").toString();
            qint64  timestamp = this->m_pConfigSettings->value(ITEM_TIMESTAMP, 0x0).toULongLong();
            quint32 index     = this->m_pConfigSettings->value(ITEM_INDEX, 0).toInt();

            QString user      = this->m_pConfigSettings->value(TICKET_USER, "").toString();
            quint32 userIndex = this->m_pConfigSettings->value(TICKET_USER_INDEX, 0).toUInt();            
            quint8  discount  = quint8(this->m_pConfigSettings->value(TICKET_DISCOUNT, 0).toUInt());
            QString place     = this->m_pConfigSettings->value(TICKET_PLACE, "").toString();

            if (!this->addNewTicketInfo(user, userIndex, ticketName, timestamp, discount, place, index))
                bProblems = true;
        }
        this->m_pConfigSettings->endArray();
        this->m_pConfigSettings->endGroup();
    }


    for (int i = 0; i < this->m_lAddItemProblems.size(); i++) {
        bProblems                               = true;
        this->m_lAddItemProblems[i].m_index = this->getNextInternalIndex();
        this->addNewTicketInfo(this->m_lAddItemProblems[i].user, this->m_lAddItemProblems[i].userIndex,
                               this->m_lAddItemProblems[i].m_itemName,
                               this->m_lAddItemProblems[i].m_timestamp, this->m_lAddItemProblems[i].discount,
                               this->m_lAddItemProblems[i].place, this->m_lAddItemProblems[i].m_index);
    }

    if (bProblems)
        this->saveCurrentInteralList();

    //    this->sortGamesListByTime();
}

int SeasonTicket::addNewSeasonTicket(QString user, quint32 userIndex, QString ticketName, quint8 discount)
{
    TicketInfo* pTicket;
    if ((pTicket = this->ticketExists(ticketName)) != NULL) {
        qInfo() << QString("SeasonTicket \"%1\" already exists, cannot add").arg(ticketName);
        return ERROR_CODE_ALREADY_EXIST;
    }

    int newIndex = this->getNextInternalIndex();

    QMutexLocker locker(&this->m_mConfigIniMutex);

    qint64 timestamp = QDateTime::currentDateTime().toMSecsSinceEpoch();


    this->m_pConfigSettings->beginGroup(GROUP_LIST_ITEM);
    this->m_pConfigSettings->beginWriteArray(CONFIG_LIST_ARRAY);

    this->m_pConfigSettings->setValue(ITEM_NAME, ticketName);
    this->m_pConfigSettings->setValue(ITEM_TIMESTAMP, timestamp);
    this->m_pConfigSettings->setValue(ITEM_INDEX, newIndex);

    this->m_pConfigSettings->setArrayIndex(this->getNumberOfInternalList());
    this->m_pConfigSettings->setValue(TICKET_USER, user);
    this->m_pConfigSettings->setValue(TICKET_USER_INDEX, userIndex);
    this->m_pConfigSettings->setValue(TICKET_DISCOUNT, discount);
    this->m_pConfigSettings->setValue(TICKET_PLACE, ticketName);

    this->m_pConfigSettings->endArray();
    this->m_pConfigSettings->endGroup();
    this->m_pConfigSettings->sync();

    this->addNewTicketInfo(user, userIndex, ticketName, timestamp, discount, ticketName, newIndex, false);

//    qInfo() << (QString("Added new ticket: %1").arg(ticketName));
    return newIndex;
}

int SeasonTicket::removeTicket(const QString& ticketName)
{
    int index = this->getTicketInfoIndex(ticketName);
    if (index < 0 || index > this->m_lInteralList.size() - 1) {
        qWarning() << (QString("Could not find season ticket \"%1\" to remove").arg(ticketName));
        return ERROR_CODE_COMMON;
    }

    QMutexLocker locker(&this->m_mInternalInfoMutex);

    this->m_lInteralList.removeAt(index);

    this->saveCurrentInteralList();

//    qInfo() << (QString("removed Ticket \"%1\"").arg(ticketName));
    return ERROR_CODE_SUCCESS;
}

int SeasonTicket::showAllSeasonTickets()
{
    QMutexLocker locker(&this->m_mInternalInfoMutex);

    foreach (TicketInfo ticket, this->m_lInteralList) {
        QString date = QDateTime::fromMSecsSinceEpoch(ticket.m_timestamp).toString("dd.MM.yyyy hh:mm");
        QString output;
        output = QString("%1: %2 - %3 - %4 - %5").arg(ticket.m_itemName).arg(ticket.discount).arg(ticket.place, date, ticket.user);
        std::cout << output.toStdString() << std::endl;
    }
    return 0;
}

void SeasonTicket::saveCurrentInteralList()
{
    QMutexLocker locker(&this->m_mConfigIniMutex);

    this->m_pConfigSettings->beginGroup(GROUP_LIST_ITEM);
    this->m_pConfigSettings->remove(""); // clear all elements

    this->m_pConfigSettings->beginWriteArray(CONFIG_LIST_ARRAY);
    for (int i = 0; i < this->m_lInteralList.size(); i++) {
        this->m_pConfigSettings->setArrayIndex(i);

        this->m_pConfigSettings->setValue(ITEM_NAME, this->m_lInteralList[i].m_itemName);
        this->m_pConfigSettings->setValue(ITEM_TIMESTAMP, this->m_lInteralList[i].m_timestamp);
        this->m_pConfigSettings->setValue(ITEM_INDEX, this->m_lInteralList[i].m_index);

        this->m_pConfigSettings->setValue(TICKET_USER, this->m_lInteralList[i].user);
        this->m_pConfigSettings->setValue(TICKET_USER_INDEX, this->m_lInteralList[i].userIndex);
        this->m_pConfigSettings->setValue(TICKET_PLACE, this->m_lInteralList[i].place);
        this->m_pConfigSettings->setValue(TICKET_DISCOUNT, this->m_lInteralList[i].discount);
    }

    this->m_pConfigSettings->endArray();
    this->m_pConfigSettings->endGroup();

    qDebug().noquote() << QString("saved actual SeasonTicket List with %1 entries").arg(this->m_lInteralList.size());
}

TicketInfo* SeasonTicket::ticketExists(QString ticketName)
{
    QMutexLocker locker(&this->m_mInternalInfoMutex);

    for (int i = 0; i < this->m_lInteralList.size(); i++) {
        if (this->m_lInteralList[i].m_itemName == ticketName)
            return &this->m_lInteralList[i];
    }
    return NULL;
}

bool SeasonTicket::ticketExists(quint32 index)
{
    QMutexLocker locker(&this->m_mInternalInfoMutex);

    foreach (TicketInfo ticket, this->m_lInteralList) {
        if (ticket.m_index == index)
            return true;
    }
    return false;
}




bool SeasonTicket::addNewTicketInfo(QString user, quint32 userIndex, QString ticketName, qint64 datetime, quint8 discount, QString place, quint32 index, bool checkTicket)
{
    if (checkTicket) {
        if (index == 0 || this->ticketExists(index)) {
            qWarning().noquote() << QString("Ticket \"%1\" with index \"%2\" already exists, saving with new index").arg(ticketName).arg(index);
            this->addNewTicketInfo(user, userIndex, ticketName, datetime, discount, place, index, &this->m_lAddItemProblems);
            return false;
        }
    }

    this->addNewTicketInfo(user, userIndex, ticketName, datetime, discount, place, index, &this->m_lInteralList);
    return true;
}

void SeasonTicket::addNewTicketInfo(QString user, quint32 userIndex, QString ticketName, qint64 datetime, quint8 discount, QString place, quint32 index, QList<TicketInfo>* pList)
{
    QMutexLocker locker(&this->m_mInternalInfoMutex);

    TicketInfo ticket;
    ticket.user      = user;
    ticket.userIndex = userIndex;
    ticket.m_itemName      = ticketName;
    ticket.m_timestamp = datetime;
    ticket.discount  = discount;
    ticket.place     = place;
    ticket.m_index     = index;
    pList->append(ticket);
}

quint32 SeasonTicket::getTicketInfoIndex(const QString& ticketName)
{
    QMutexLocker locker(&this->m_mInternalInfoMutex);

    for (int i = 0; i < this->m_lInteralList.size(); i++) {
        if (this->m_lInteralList[i].m_itemName == ticketName)
            return i;
    }
    return -1;
}

bool SeasonTicket::updateTicketInfoValue(TicketInfo* pTicket, QString key, QVariant value)
{
    bool         rValue = false;
    QMutexLocker locker(&this->m_mConfigIniMutex);

    this->m_pConfigSettings->beginGroup(GROUP_LIST_ITEM);
    int arrayCount = this->m_pConfigSettings->beginReadArray(CONFIG_LIST_ARRAY);
    for (int i = 0; i < arrayCount; i++) {
        this->m_pConfigSettings->setArrayIndex(i);
        quint32 actIndex = this->m_pConfigSettings->value(ITEM_INDEX, 0).toInt();
        if (pTicket->m_index == actIndex) {

            this->m_pConfigSettings->setValue(key, value);
            qInfo().noquote() << QString("Change %1 of ticket %2 to %3").arg(key, pTicket->m_itemName).arg(value.toString());
            rValue = true;
            break;
        }
    }
    this->m_pConfigSettings->endArray();
    this->m_pConfigSettings->endGroup();
    return rValue;
}




SeasonTicket::~SeasonTicket()
{
    if (this->m_pConfigSettings != NULL)
        delete this->m_pConfigSettings;
}
