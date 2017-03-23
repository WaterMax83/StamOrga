#include <QtCore/QDataStream>
#include <QtCore/QtEndian>

#include "messageprotocol.h"
#include "../General/globaltiming.h"

MessageProtocol::MessageProtocol()
{
    this->m_Data.fill(0x00, MSG_HEADER_SIZE);
    this->m_pHead = (msg_Header *)this->m_Data.constData();
}

MessageProtocol::MessageProtocol(QByteArray &data)
{
    if (data.length() < (int)MSG_HEADER_SIZE)
        return;

    this->m_Data.append(data);
    this->m_pHead = (msg_Header *)this->m_Data.constData();

}

MessageProtocol::MessageProtocol(quint32 index, QByteArray &data) : MessageProtocol()
{
    this->m_Data.append(data);

    int length = data.length();

    int tmp = length % sizeof(quint32);
    if (tmp > 0)
    {
        length += (sizeof(quint32) - tmp);
        this->m_Data.resize(MSG_HEADER_SIZE + length);
    }

    this->m_pHead = (msg_Header *)this->m_Data.constData();

    this->m_pHead->index = index;
    this->m_pHead->length = length;
    this->m_pHead->timestamp = CalcTimeStamp();
}

MessageProtocol::MessageProtocol(quint32 index, quint32 data) : MessageProtocol()
{
    QDataStream wStream(&this->m_Data, QIODevice::WriteOnly);

    wStream.device()->seek(this->m_Data.length());

    wStream << qToLittleEndian(data);

    this->m_pHead = (msg_Header *)this->m_Data.constData();

    this->m_pHead->index = index;
    this->m_pHead->length = sizeof(data);
    this->m_pHead->timestamp = CalcTimeStamp();
}



