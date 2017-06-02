/*
*	This file is part of StamOrga
*   Copyright (C) 2017 Markus Schneider
*
*	This program is free software; you can redistribute it and/or modify
*   it under the terms of the GNU General Public License as published by
*   the Free Software Foundation; either version 3 of the License, or
*   (at your option) any later version.
*
*	StamOrga is distributed in the hope that it will be useful,
*    but WITHOUT ANY WARRANTY; without even the implied warranty of
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*    GNU General Public License for more details.

*    You should have received a copy of the GNU General Public License
*    along with StamOrga.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef MESSAGEPROTOCOL_H
#define MESSAGEPROTOCOL_H

#include <QtCore/QByteArray>
#include <QtCore/QtEndian>

#include "../../Common/General/globalfunctions.h"

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
    MessageProtocol(quint32 index);
    MessageProtocol(quint32 index, QByteArray &data);
    MessageProtocol(quint32 index, quint32 data);
    MessageProtocol(quint32 index, qint32 data);

//    void SetTimeStamp(quint32 time) { this->m_pHead->timestamp = time; }
//    void SetIndex(quint32 index) { this->m_pHead->index = index; }

    quint32 getTimeStamp() { return qFromLittleEndian(this->m_pHead->timestamp); }
    quint32 getIndex() { return qFromLittleEndian(this->m_pHead->index); }
    quint32 getDataLength() { return qFromLittleEndian(this->m_pHead->length); }

    quint32 getNetworkSize() { return this->m_Data.size(); }
    const char *getNetworkProtocol() { return this->m_Data.constData();}

    const char *getPointerToData()
    {
        if (this->getDataLength() == 0)
            return NULL;
        return getNetworkProtocol() + MSG_HEADER_SIZE;
    }

    qint32 getIntData()
    {
        if (this->getDataLength() != 4)
            return ERROR_CODE_WRONG_SIZE;
        return qFromLittleEndian(*(qint32*)this->getPointerToData());
    }

private:
    QByteArray m_Data;

    msg_Header *m_pHead;
};

#endif // MESSAGEPROTOCOL_H
