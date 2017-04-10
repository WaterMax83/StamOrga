#ifndef SEASONTICKET_H
#define SEASONTICKET_H


#include <QtCore/QMutex>
#include <QtCore/QSettings>
#include <QtCore/QString>


struct TicketInfo {
    QString user;
    QString name;
    qint64  timestamp;
    quint8  discount;
    QString place;
    quint32 index;

    //    static bool compareDateTimeFunction(GamesPlay p1, GamesPlay p2)
    //    {
    //        if (p1.datetime > p2.datetime)
    //            return false;
    //        return true;
    //    }
};

#define TICKET_GROUP "TICKETS"
#define TICKET_ARRAY "season"
#define TICKET_USER "user"
#define TICKET_NAME "name"
#define TICKET_INDEX "index"
#define TICKET_DISCOUNT "discount"
#define TICKET_TIMESTAMP "timestamp"
#define TICKET_PLACE "place"

class SeasonTicket
{
public:
    SeasonTicket();
    ~SeasonTicket();


    int addNewSeasonTicket(QString user, QString name, quint8 discount);
    int removeTicket(const QString& name);
    int showAllSeasonTickets();

    quint16 getNumberOfTickets() { return this->m_lTicketInfo.size(); }

    TicketInfo* ticketExists(QString name);
    bool ticketExists(quint32 index);

    quint16 startRequestGetTicketInfoList()
    {
        this->m_mTicketInfoMutex.lock();
        return this->getNumberOfTickets();
    }

    TicketInfo* getRequestTicketInfo(int index)
    {
        if (index < this->m_lTicketInfo.size())
            return &this->m_lTicketInfo[index];
        return NULL;
    }

    void stopRequestGetTicketInfoList()
    {
        this->m_mTicketInfoMutex.unlock();
    }

    //    void sortGamesListByTime();
    //    bool userCheckPassword(QString name, QString passw);
    //    bool userChangePassword(QString name, QString passw);
    //    bool userChangeProperties(QString name, quint32 props);
    //    quint32 getUserProperties(QString name);

private:
    QSettings*        m_pTicketsSettings = NULL;
    QList<TicketInfo> m_lTicketInfo;
    QMutex            m_mTicketIniMutex;
    QMutex            m_mTicketInfoMutex;

    QList<TicketInfo> m_lAddTicketInfoProblems;

    void saveActualTicketList();

    bool addNewTicketInfo(QString user, QString name, qint64 datetime, quint8 discount, QString place, quint32 index, bool checkTicket = true);
    void addNewTicketInfo(QString user, QString name, qint64 datetime, quint8 discount, QString place, quint32 index, QList<TicketInfo>* pList);
    quint32 getTicketInfoIndex(const QString& name);
    quint32 getNextTicketIndex();

    bool updateTicketInfoValue(TicketInfo* pTicket, QString key, QVariant value);
};

#endif // SEASONTICKET_H
