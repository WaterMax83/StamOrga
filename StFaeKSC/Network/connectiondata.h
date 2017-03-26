#ifndef CONNECTIONDATA_H
#define CONNECTIONDATA_H

#include <QtNetwork/QUdpSocket>

struct UserConData {
    QHostAddress            sender;
    quint16                 srcPort;
    quint16                 dataPort;
    QString                 userName;
};

#endif // CONNECTIONDATA_H
