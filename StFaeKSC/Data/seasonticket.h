#ifndef SEASONTICKET_H
#define SEASONTICKET_H


#include <QtCore/QMutex>
#include <QtCore/QSettings>
#include <QtCore/QString>

#include "configlist.h"

struct TicketInfo : public ConfigItem
{
    QString user;
    quint32 userIndex;
    quint8  discount;
    QString place;

};


#define TICKET_USER "user"
#define TICKET_USER_INDEX "userIndex"
#define TICKET_DISCOUNT "discount"
#define TICKET_PLACE "place"

#define TICKET_INDEX_GROUP    "IndexCount"
#define TICKET_MAX_COUNT      "CurrentCount"

class SeasonTicket : public ConfigList
{
public:
    SeasonTicket();
    ~SeasonTicket();


    int addNewSeasonTicket(QString user, quint32 userIndex, QString ticketName, quint8 discount);
    int removeTicket(const QString& ticketName);
    int showAllSeasonTickets();

    quint32 getNumberOfInternalList() { return this->m_lInteralList.size(); }

    TicketInfo* ticketExists(QString ticketName);
    bool ticketExists(quint32 index);


    TicketInfo* getRequestConfigItem(int index)
    {
        if (index < this->m_lInteralList.size())
            return &this->m_lInteralList[index];
        return NULL;
    }

private:
    QList<TicketInfo> m_lInteralList;
    QList<TicketInfo> m_lAddItemProblems;

    void saveCurrentInteralList() override;

    bool addNewTicketInfo(QString user, quint32 userIndex, QString ticketName, qint64 timestamp, quint8 discount, QString place, quint32 index, bool checkTicket = true);
    void addNewTicketInfo(QString user, quint32 userIndex, QString ticketName, qint64 timestamp, quint8 discount, QString place, quint32 index, QList<TicketInfo>* pList);
    quint32 getTicketInfoIndex(const QString& ticketName);

    bool updateTicketInfoValue(TicketInfo* pTicket, QString key, QVariant value);
};

#endif // SEASONTICKET_H
