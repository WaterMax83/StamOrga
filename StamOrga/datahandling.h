#ifndef DATAHANDLING_H
#define DATAHANDLING_H

#include <QtCore/QString>

#include "../Common/Network/messageprotocol.h"

class DataHandling
{
public:
    DataHandling();

    qint32 getHandleLoginResponse(MessageProtocol *msg);
    qint32 getHandleVersionResponse(MessageProtocol *msg, QString *version);
};

#endif // DATAHANDLING_H
