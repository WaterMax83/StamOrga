#ifndef DATACONNECTION_H
#define DATACONNECTION_H

#include <QObject>

#include "globaldata.h"
#include "../Network/connectiondata.h"
#include "../Common/Network/messageprotocol.h"

class DataConnection : public QObject
{
    Q_OBJECT
public:
    explicit DataConnection(GlobalData *pGData, QObject *parent = 0);

    MessageProtocol *requestCheckUserLogin(MessageProtocol *msg, UserConData *pUserConData);
    MessageProtocol *requestGetProgramVersion(MessageProtocol *msg);

signals:

public slots:

private:
    GlobalData      *m_pGlobalData;
};

#endif // DATACONNECTION_H
