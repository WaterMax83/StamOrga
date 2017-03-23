
#include <QtCore/QtEndian>

#include "messagebuffer.h"
#include "messageprotocol.h"


#define MIN_PAYLOAD_SIZE    sizeof(msg_Header)
#define MAX_PAYLOAD_SIZE    512


MessageBuffer::MessageBuffer()
{

}

void MessageBuffer::StoreNewData(QByteArray &data)
{
    if (data.length() == 0)
        return;

    this->m_DataBuffer.append(data);
}

MessageProtocol *MessageBuffer::GetNextMessage()
{
    if (this->m_DataBuffer.length() < 12)
        return NULL;

    msg_Header *pHead = (msg_Header *)this->m_DataBuffer.constData();

    uint payLoadLength = qFromLittleEndian(pHead->length);

    int tmp = payLoadLength % sizeof(quint32);
    if (tmp > 0)
        payLoadLength += (sizeof(quint32) - tmp);

    payLoadLength += MIN_PAYLOAD_SIZE;    // MinSize

    if (payLoadLength > MAX_PAYLOAD_SIZE)
    {
        this->m_DataBuffer.clear();
        return NULL;
    }

    if (payLoadLength > (uint)this->m_DataBuffer.length())    // not yet received everything
        return NULL;

    QByteArray packet = this->m_DataBuffer.left(payLoadLength);
    MessageProtocol *msg = new MessageProtocol(packet);

    this->m_DataBuffer.remove(0, payLoadLength);

    return msg;
}

