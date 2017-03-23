#ifndef MESSAGEPROTOCOL_H
#define MESSAGEPROTOCOL_H

#include <QtCore/QByteArray>
#include <QtCore/QtEndian>

struct msg_Header
{
    quint32 timestamp;
    quint32 length;
    quint32 index;
};


#define MSG_HEADER_SIZE     sizeof(msg_Header)

class MessageProtocol
{
public:
    MessageProtocol();
    MessageProtocol(QByteArray &data);
    MessageProtocol(quint32 index, QByteArray &data);
    MessageProtocol(quint32 index, quint32 data);

//    void SetTimeStamp(quint32 time) { this->m_pHead->timestamp = time; }
//    void SetIndex(quint32 index) { this->m_pHead->index = index; }

    quint32 GetTimeStamp() { return qFromLittleEndian(this->m_pHead->timestamp); }
    quint32 GetIndex() { return qFromLittleEndian(this->m_pHead->index); }
    quint32 getDataLength() { return qFromLittleEndian(this->m_pHead->length); }

    quint32 getNetworkSize() { return this->m_Data.size(); }
    const char *getNetworkProtocol() { return this->m_Data.constData();}

    const char *getPointerToData()
    {
        if (this->getDataLength() == 0)
            return NULL;
        return getNetworkProtocol() + MSG_HEADER_SIZE;
    }


private:
    QByteArray m_Data;

    msg_Header *m_pHead;
};

#endif // MESSAGEPROTOCOL_H
