#ifndef CONNECTIONDATA_H
#define CONNECTIONDATA_H

#include <QtNetwork/QUdpSocket>

struct UserConData {
    QHostAddress            sender;
    quint16                 srcMasterPort;
    quint16                 dstDataPort;
    quint16                 srcDataPort;
    QString                 userName;
    bool                    bIsConnected;
};

#endif // CONNECTIONDATA_H
