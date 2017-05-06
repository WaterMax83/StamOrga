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


#include <QtCore/QtEndian>

#include "messagebuffer.h"
#include "messageprotocol.h"


#define MIN_PAYLOAD_SIZE sizeof(msg_Header)
//#define MAX_PAYLOAD_SIZE    512


MessageBuffer::MessageBuffer()
{
}

void MessageBuffer::StoreNewData(QByteArray& data)
{
    if (data.length() == 0)
        return;

    this->m_DataBuffer.append(data);
}

MessageProtocol* MessageBuffer::GetNextMessage()
{
    if (this->m_DataBuffer.length() < 12)
        return NULL;

    msg_Header* pHead = (msg_Header*)this->m_DataBuffer.constData();

    uint payLoadLength = qFromBigEndian(pHead->length);

    int tmp = payLoadLength % sizeof(quint32);
    if (tmp > 0)
        payLoadLength += (sizeof(quint32) - tmp);

    payLoadLength += MIN_PAYLOAD_SIZE; // MinSize

    //    if (payLoadLength > MAX_PAYLOAD_SIZE) {
    //        this->m_DataBuffer.clear();
    //        return NULL;
    //    }

    if (payLoadLength > (uint)this->m_DataBuffer.length()) // not yet received everything
        return NULL;

    QByteArray       packet = this->m_DataBuffer.left(payLoadLength);
    MessageProtocol* msg    = new MessageProtocol(packet);

    this->m_DataBuffer.remove(0, payLoadLength);

    return msg;
}
