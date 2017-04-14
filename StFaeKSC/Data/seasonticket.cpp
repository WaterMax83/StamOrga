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

    this->m_pTicketsSettings = new QSettings(ticketSetFilePath, QSettings::IniFormat);

    /* Check wheter we have to save data after reading again */
    bool bProblems = false;
    {
        QMutexLocker locker(&this->m_mTicketIniMutex);

        this->m_pTicketsSettings->beginGroup(TICKET_GROUP);
        int sizeOfTicket = this->m_pTicketsSettings->beginReadArray(TICKET_ARRAY);

        for (int i = 0; i < sizeOfTicket; i++) {
            this->m_pTicketsSettings->setArrayIndex(i);
            QString user      = this->m_pTicketsSettings->value(TICKET_USER, "").toString();
            quint32 userIndex = this->m_pTicketsSettings->value(TICKET_USER_INDEX, 0).toUInt();
            QString name      = this->m_pTicketsSettings->value(TICKET_NAME, "").toString();
            qint64  timestamp = this->m_pTicketsSettings->value(TICKET_TIMESTAMP, 0x0).toULongLong();
            quint8  discount  = quint8(this->m_pTicketsSettings->value(TICKET_DISCOUNT, 0).toUInt());
            QString place     = this->m_pTicketsSettings->value(TICKET_PLACE, "").toString();
            quint32 index     = this->m_pTicketsSettings->value(TICKET_INDEX, 0).toInt();
            if (!this->addNewTicketInfo(user, userIndex, name, timestamp, discount, place, index))
                bProblems = true;
        }
        this->m_pTicketsSettings->endArray();
        this->m_pTicketsSettings->endGroup();
    }


    for (int i = 0; i < this->m_lAddTicketInfoProblems.size(); i++) {
        bProblems                               = true;
        this->m_lAddTicketInfoProblems[i].index = this->getNextTicketIndex();
        this->addNewTicketInfo(this->m_lAddTicketInfoProblems[i].user, this->m_lAddTicketInfoProblems[i].userIndex,
                               this->m_lAddTicketInfoProblems[i].name,
                               this->m_lAddTicketInfoProblems[i].timestamp, this->m_lAddTicketInfoProblems[i].discount,
                               this->m_lAddTicketInfoProblems[i].place, this->m_lAddTicketInfoProblems[i].index);
    }

    if (bProblems)
        this->saveActualTicketList();

    //    this->sortGamesListByTime();
}

int SeasonTicket::addNewSeasonTicket(QString user, quint32 userIndex, QString name, quint8 discount)
{
    //    if (sIndex == 0 || comp == 0) {
    //        qWarning().noquote() << "Could not add game because saisonIndex or competition were zero";
    //        return ERROR_CODE_COMMON;
    //    }

    TicketInfo* pTicket;
    if ((pTicket = this->ticketExists(name)) != NULL) {
        qInfo() << QString("SeasonTicket \"%1\" already exists, cannot add").arg(name);
        return ERROR_CODE_ALREADY_EXIST;
    }

    QMutexLocker locker(&this->m_mTicketIniMutex);

    qint64 timestamp = QDateTime::currentDateTime().toMSecsSinceEpoch();

    int newIndex = this->getNextTicketIndex();
    this->m_pTicketsSettings->beginGroup(TICKET_GROUP);
    this->m_pTicketsSettings->beginWriteArray(TICKET_ARRAY);

    this->m_pTicketsSettings->setArrayIndex(this->getNumberOfTickets());
    this->m_pTicketsSettings->setValue(TICKET_USER, user);
    this->m_pTicketsSettings->setValue(TICKET_USER_INDEX, userIndex);
    this->m_pTicketsSettings->setValue(TICKET_NAME, name);
    this->m_pTicketsSettings->setValue(TICKET_TIMESTAMP, timestamp);
    this->m_pTicketsSettings->setValue(TICKET_DISCOUNT, discount);
    this->m_pTicketsSettings->setValue(TICKET_PLACE, name);
    this->m_pTicketsSettings->setValue(TICKET_INDEX, newIndex);
    this->m_pTicketsSettings->endArray();
    this->m_pTicketsSettings->endGroup();
    this->m_pTicketsSettings->sync();

    this->addNewTicketInfo(user, userIndex, name, timestamp, discount, name, newIndex, false);

//    qInfo() << (QString("Added new ticket: %1").arg(name));
    return newIndex;
}

int SeasonTicket::removeTicket(const QString& name)
{
    int index = this->getTicketInfoIndex(name);
    if (index < 0 || index > this->m_lTicketInfo.size() - 1) {
        qWarning() << (QString("Could not find season ticket \"%1\" to remove").arg(name));
        return ERROR_CODE_COMMON;
    }

    QMutexLocker locker(&this->m_mTicketInfoMutex);

    this->m_lTicketInfo.removeAt(index);

    this->saveActualTicketList();

//    qInfo() << (QString("removed Ticket \"%1\"").arg(name));
    return ERROR_CODE_SUCCESS;
}

int SeasonTicket::showAllSeasonTickets()
{
    QMutexLocker locker(&this->m_mTicketInfoMutex);

    foreach (TicketInfo ticket, this->m_lTicketInfo) {
        QString date = QDateTime::fromMSecsSinceEpoch(ticket.timestamp).toString("dd.MM.yyyy hh:mm");
        QString output;
        output = QString("%1: %2 - %3 - %4 - %5").arg(ticket.name).arg(ticket.discount).arg(ticket.place, date, ticket.user);
        std::cout << output.toStdString() << std::endl;
    }
    return 0;
}

void SeasonTicket::saveActualTicketList()
{
    QMutexLocker locker(&this->m_mTicketIniMutex);

    this->m_pTicketsSettings->beginGroup(TICKET_GROUP);
    this->m_pTicketsSettings->remove(""); // clear all elements

    this->m_pTicketsSettings->beginWriteArray(TICKET_ARRAY);
    for (int i = 0; i < this->m_lTicketInfo.size(); i++) {
        this->m_pTicketsSettings->setArrayIndex(i);
        this->m_pTicketsSettings->setValue(TICKET_USER, this->m_lTicketInfo[i].user);
        this->m_pTicketsSettings->setValue(TICKET_USER_INDEX, this->m_lTicketInfo[i].userIndex);
        this->m_pTicketsSettings->setValue(TICKET_NAME, this->m_lTicketInfo[i].name);
        this->m_pTicketsSettings->setValue(TICKET_TIMESTAMP, this->m_lTicketInfo[i].timestamp);
        this->m_pTicketsSettings->setValue(TICKET_PLACE, this->m_lTicketInfo[i].place);
        this->m_pTicketsSettings->setValue(TICKET_DISCOUNT, this->m_lTicketInfo[i].discount);
        this->m_pTicketsSettings->setValue(TICKET_INDEX, this->m_lTicketInfo[i].index);
    }

    this->m_pTicketsSettings->endArray();
    this->m_pTicketsSettings->endGroup();

    qDebug().noquote() << QString("saved actual SeasonTicket List with %1 entries").arg(this->m_lTicketInfo.size());
}

TicketInfo* SeasonTicket::ticketExists(QString name)
{
    QMutexLocker locker(&this->m_mTicketInfoMutex);

    for (int i = 0; i < this->m_lTicketInfo.size(); i++) {
        if (this->m_lTicketInfo[i].name == name)
            return &this->m_lTicketInfo[i];
    }
    return NULL;
}

bool SeasonTicket::ticketExists(quint32 index)
{
    QMutexLocker locker(&this->m_mTicketInfoMutex);

    foreach (TicketInfo ticket, this->m_lTicketInfo) {
        if (ticket.index == index)
            return true;
    }
    return false;
}

//bool ListedUser::userCheckPassword(QString name, QString passw)
//{
//    QMutexLocker locker(&this->m_mUserListMutex);

//    if (name.length() < MIN_SIZE_USERNAME)
//        return false;

//    foreach (UserLogin login, this->m_lUserLogin) {
//        if (login.userName == name) {
//            if (login.password == passw)
//                return true;
//            return false;
//        }
//    }
//    return false;
//}

//bool ListedUser::userChangePassword(QString name, QString passw)
//{
//    QMutexLocker locker(&this->m_mUserListMutex);

//    if (name.length() < MIN_SIZE_USERNAME)
//        return false;

//    for (int i=0; i<this->m_lUserLogin.size(); i++) {
//        if (this->m_lUserLogin[i].userName == name) {
//            if (this->updateUserLoginValue(&this->m_lUserLogin[i], LOGIN_PASSWORD, QVariant(passw))) {
//                this->m_lUserLogin[i].password = passw;
//                return true;
//            }
//        }
//    }
//    return false;
//}

//bool ListedUser::userChangeProperties(QString name, quint32 props)
//{
//    QMutexLocker locker(&this->m_mUserListMutex);

//    if (name.length() < MIN_SIZE_USERNAME)
//        return false;

//    for (int i=0; i<this->m_lUserLogin.size(); i++) {
//        if (this->m_lUserLogin[i].userName == name) {
//            if (this->updateUserLoginValue(&this->m_lUserLogin[i], LOGIN_PROPERTIES, QVariant(props))) {
//                this->m_lUserLogin[i].properties = props;
//                return true;
//            } else
//                return false;
//        }
//    }
//    return false;
//}

//quint32 ListedUser::getUserProperties(QString name)
//{
//    QMutexLocker locker(&this->m_mUserListMutex);

//    foreach (UserLogin login, this->m_lUserLogin) {
//        if (login.userName == name)
//            return login.properties;
//    }
//    return 0;
//}


bool SeasonTicket::addNewTicketInfo(QString user, quint32 userIndex, QString name, qint64 datetime, quint8 discount, QString place, quint32 index, bool checkTicket)
{
    if (checkTicket) {
        if (index == 0 || this->ticketExists(index)) {
            qWarning().noquote() << QString("Ticket \"%1\" with index \"%2\" already exists, saving with new index").arg(name).arg(index);
            this->addNewTicketInfo(user, userIndex, name, datetime, discount, place, index, &this->m_lAddTicketInfoProblems);
            return false;
        }
    }

    this->addNewTicketInfo(user, userIndex, name, datetime, discount, place, index, &this->m_lTicketInfo);
    return true;
}

void SeasonTicket::addNewTicketInfo(QString user, quint32 userIndex, QString name, qint64 datetime, quint8 discount, QString place, quint32 index, QList<TicketInfo>* pList)
{
    QMutexLocker locker(&this->m_mTicketInfoMutex);

    TicketInfo ticket;
    ticket.user      = user;
    ticket.userIndex = userIndex;
    ticket.name      = name;
    ticket.timestamp = datetime;
    ticket.discount  = discount;
    ticket.place     = place;
    ticket.index     = index;
    pList->append(ticket);
}

quint32 SeasonTicket::getTicketInfoIndex(const QString& name)
{
    QMutexLocker locker(&this->m_mTicketInfoMutex);

    for (int i = 0; i < this->m_lTicketInfo.size(); i++) {
        if (this->m_lTicketInfo[i].name == name)
            return i;
    }
    return -1;
}

quint32 SeasonTicket::getNextTicketIndex()
{
    QMutexLocker locker(&this->m_mTicketInfoMutex);

    quint32 index = 0;
    for (int i = 0; i < this->m_lTicketInfo.size(); i++) {
        if (this->m_lTicketInfo[i].index > index)
            index = this->m_lTicketInfo[i].index;
    }
    return index + 1;
}

bool SeasonTicket::updateTicketInfoValue(TicketInfo* pTicket, QString key, QVariant value)
{
    bool         rValue = false;
    QMutexLocker locker(&this->m_mTicketIniMutex);

    this->m_pTicketsSettings->beginGroup(TICKET_GROUP);
    int arrayCount = this->m_pTicketsSettings->beginReadArray(TICKET_ARRAY);
    for (int i = 0; i < arrayCount; i++) {
        this->m_pTicketsSettings->setArrayIndex(i);
        quint32 actIndex = this->m_pTicketsSettings->value(TICKET_INDEX, 0).toInt();
        if (pTicket->index == actIndex) {

            this->m_pTicketsSettings->setValue(key, value);
            qInfo().noquote() << QString("Change %1 of ticket %2 to %3").arg(key, pTicket->name).arg(value.toString());
            rValue = true;
            break;
        }
    }
    this->m_pTicketsSettings->endArray();
    this->m_pTicketsSettings->endGroup();
    return rValue;
}

//void SeasonTicket::sortGamesListByTime()
//{
//    QMutexLocker locker(&this->m_mTicketInfoMutex);

//    std::sort(this->m_lTicketInfo.begin(), this->m_lTicketInfo.end(), TicketInfo::compareDateTimeFunction);
//}


SeasonTicket::~SeasonTicket()
{
    if (this->m_pTicketsSettings != NULL)
        delete this->m_pTicketsSettings;
}
