#ifndef MESSAGEBUFFER_H
#define MESSAGEBUFFER_H


#include <QtCore/QByteArray>

#include "messageprotocol.h"


class MessageBuffer
{
public:
    MessageBuffer();

    void StoreNewData(QByteArray &data);

    MessageProtocol *GetNextMessage();

private:
    QByteArray m_DataBuffer;
};

#endif // MESSAGEBUFFER_H
