#ifndef SEASONTICKET_H
#define SEASONTICKET_H


#include <QtCore/QSettings>
#include <QtCore/QString>
#include <QtCore/QMutex>


struct TicketInfo {
    QString user;
    QString name;
    qint64 timestamp;
    quint32 discount;
    QString place;
    quint32 index;

//    static bool compareDateTimeFunction(GamesPlay p1, GamesPlay p2)
//    {
//        if (p1.datetime > p2.datetime)
//            return false;
//        return true;
//    }
};

#define TICKET_GROUP             "TICKETS"
#define TICKET_ARRAY             "season"
#define TICKET_USER              "user"
#define TICKET_NAME               "name"
#define TICKET_INDEX               "index"
#define TICKET_DISCOUNT           "discount"
#define TICKET_TIMESTAMP       "timestamp"
#define TICKET_PLACE            "place"

class SeasonTicket
{
public:
    SeasonTicket();
    ~SeasonTicket();


    int addNewSeasonTicket(QString user, QString name, quint32 discount);
//    int removeUser(const QString &name);
    int showAllSeasonTickets();

    quint16 getNumberOfTickets() { return this->m_lTicketInfo.size(); }

    TicketInfo *ticketExists(QString name);
    bool ticketExists(quint32 index);

//    quint16 startRequestGetGamesPlay()
//    {
//        this->m_mGamesListMutex.lock();
//        return this->getNumberOfGames();
//    }

//    GamesPlay *getRequestGamesPlay(int index)
//    {
//        if (index < this->m_lGamesPlay.size())
//            return &this->m_lGamesPlay[index];
//        return NULL;
//    }

//    void stopRequestGetGamesPlay()
//    {
//        this->m_mGamesListMutex.unlock();
//    }

//    void sortGamesListByTime();
//    bool userCheckPassword(QString name, QString passw);
//    bool userChangePassword(QString name, QString passw);
//    bool userChangeProperties(QString name, quint32 props);
//    quint32 getUserProperties(QString name);

private:
    QSettings           *m_pTicketsSettings = NULL;
    QList<TicketInfo>    m_lTicketInfo;
    QMutex              m_mTicketIniMutex;
    QMutex              m_mTicketInfoMutex;

    QList<TicketInfo>    m_lAddTicketInfoProblems;

    void saveActualTicketList();

    bool addNewTicketInfo(QString user, QString name, qint64 datetime, quint32 discount, QString place, quint32 index, bool checkTicket = true);
    void addNewTicketInfo(QString user, QString name, qint64 datetime, quint32 discount, QString place, quint32 index, QList<TicketInfo> *pList);
//    quint32 getUserLoginIndex(const QString &name);
    quint32 getNextTicketIndex();

    bool updateTicketInfoValue(TicketInfo *pTicket, QString key, QVariant value);
};

#endif // SEASONTICKET_H
