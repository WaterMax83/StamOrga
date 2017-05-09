#ifndef RESERVEDTICKETLIST_H
#define RESERVEDTICKETLIST_H


#include <QtCore/QMutex>
#include <QtCore/QSettings>
#include <QtCore/QString>

#include "configlist.h"

struct FreeTicketInfo {
    quint32 m_ticketID;
    qint64 m_timeStamp;
    quint32 m_userID;
};

class ReservedTicketList
{
public:
    ReservedTicketList();

    void initialize(quint32 year);

private:
    QList<FreeTicketInfo*> m_lInteralList;
    QList<FreeTicketInfo*> m_lAddItemProblems;

    QSettings* m_pConfigSettings = NULL;
    QMutex     m_mConfigIniMutex;
    QMutex     m_mInternalInfoMutex;
};

#endif // RESERVEDTICKETLIST_H
