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

#include <QtCore/QDataStream>
#include <QtCore/QtEndian>

#include "../General/globaltiming.h"
#include "messageprotocol.h"

MessageProtocol::MessageProtocol()
{
    this->m_Data.fill(0x00, MSG_HEADER_SIZE);
    this->m_pHead = (msg_Header*)this->m_Data.constData();
}

MessageProtocol::MessageProtocol(QByteArray& data)
{
    if (data.length() < (int)MSG_HEADER_SIZE)
        return;

    this->m_Data.append(data);
    this->m_pHead = (msg_Header*)this->m_Data.constData();
}

MessageProtocol::MessageProtocol(quint32 index)
    : MessageProtocol()
{
    this->m_pHead = (msg_Header*)this->m_Data.constData();

    this->m_pHead->index     = qToBigEndian(index);
    this->m_pHead->length    = 0;
    this->m_pHead->timestamp = qToBigEndian(CalcTimeStamp());
}

MessageProtocol::MessageProtocol(quint32 index, QByteArray& data)
    : MessageProtocol()
{
    int length = data.length();

    int tmp = length % sizeof(quint32);
    qDebug() << QString("MessageProtocol: %1 %2").arg(length).arg(tmp);
    if (tmp > 0) {
        this->m_Data.fill(0x0, MSG_HEADER_SIZE + (sizeof(quint32) - tmp));
        //        length += (sizeof(quint32) - tmp);
    }
    this->m_Data.insert(MSG_HEADER_SIZE, data);
    qDebug() << QString("MessageProtocol2: %1 %2").arg(length).arg(this->m_Data.size());

    this->m_pHead = (msg_Header*)this->m_Data.constData();

    this->m_pHead->index     = qToBigEndian(index);
    this->m_pHead->length    = qToBigEndian(length);
    this->m_pHead->timestamp = qToBigEndian(CalcTimeStamp());
}

MessageProtocol::MessageProtocol(quint32 index, quint32 data)
    : MessageProtocol()
{
    QDataStream wStream(&this->m_Data, QIODevice::WriteOnly);
    wStream.setByteOrder(QDataStream::BigEndian);

    wStream.device()->seek(this->m_Data.length());

    wStream << data;

    this->m_pHead = (msg_Header*)this->m_Data.constData();

    quint32 size             = sizeof(data);
    this->m_pHead->index     = qToBigEndian(index);
    this->m_pHead->length    = qToBigEndian(size);
    this->m_pHead->timestamp = qToBigEndian(CalcTimeStamp());
}

MessageProtocol::MessageProtocol(quint32 index, qint32 data)
    : MessageProtocol()
{
    QDataStream wStream(&this->m_Data, QIODevice::WriteOnly);
    wStream.setByteOrder(QDataStream::BigEndian);

    wStream.device()->seek(this->m_Data.length());

    wStream << data;

    this->m_pHead = (msg_Header*)this->m_Data.constData();

    quint32 size             = sizeof(data);
    this->m_pHead->index     = qToBigEndian(index);
    this->m_pHead->length    = qToBigEndian(size);
    this->m_pHead->timestamp = qToBigEndian(CalcTimeStamp());
}
