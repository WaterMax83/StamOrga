#ifndef GLOBALTIMING_H
#define GLOBALTIMING_H


#include <QtCore/QDateTime>

#define MSEC_PER_HOUR   3600000


#define CON_RESET_TIMEOUT_MSEC        (15 * 60 * 1000)
#define CON_LOGIN_TIMEOUT_MSEC        (2 * 60 * 1000)


inline uint CalcMSecEpochToUsHour(quint64 msec64)
{
    uint msec32 = msec64 % MSEC_PER_HOUR;

    return msec32 * 1000;
}

inline quint64 CalcTimeStamp()
{
    return CalcMSecEpochToUsHour(QDateTime::currentDateTime().toMSecsSinceEpoch());
}

#endif // GLOBALTIMING_H
