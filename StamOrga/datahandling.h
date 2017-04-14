#ifndef DATAHANDLING_H
#define DATAHANDLING_H

#include <QtCore/QString>

#include "../Data/globaldata.h"
#include "../Common/Network/messageprotocol.h"

class DataHandling
{
public:
    DataHandling(GlobalData *pData);

    qint32 getHandleLoginResponse(MessageProtocol *msg);
    qint32 getHandleVersionResponse(MessageProtocol *msg, QString *version);
    qint32 getHandleUserPropsResponse(MessageProtocol *msg, quint32 *props);
    qint32 getHandleGamesListResponse(MessageProtocol *msg);
    qint32 getHandleSeasonTicketListResponse(MessageProtocol *msg);

private:
    GlobalData  *m_pGlobalData;
};

#endif // DATAHANDLING_H
