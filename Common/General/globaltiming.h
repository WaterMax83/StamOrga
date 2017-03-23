#ifndef GLOBALTIMING_H
#define GLOBALTIMING_H


#include <QtCore/QDateTime>

#define MSEC_PER_HOUR   3600000


inline uint CalcMSecEpochToUsHour(quint64 msec64)
{
    uint msec32 = msec64 % MSEC_PER_HOUR;

    return msec32 * 1000;
}

quint64 CalcTimeStamp()
{
    return CalcMSecEpochToUsHour(QDateTime::currentDateTime().toMSecsSinceEpoch());
}

#endif // GLOBALTIMING_H
