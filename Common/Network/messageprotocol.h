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

struct msg_Header {
    quint32 m_timestamp;
    quint32 m_length;
    quint32 m_index;
    quint32 m_version;
};


#define MSG_HEADER_SIZE sizeof(msg_Header)

// clang-format off
#define MSG_HEADER_VERSION_START        0x1
#define MSG_HEADER_VERSION_PASSWORD     0x2
#define MSG_HEADER_VERSION_GAME_LIST    0x3
#define MSG_HEADER_VERSION_MESSAGE_ID   0x4     // V1.0.3
#define MSG_HEADER_ADD_FANCLUB          0x5     // 20.09.17 for V1.0.4
#define MSG_HEADER_ADD_AWAYTRIP_INFO    0x6     // 25.01.18 for V1.0.6
#define MSG_HEADER_JSON_USERPROPS       0x7     // 27.02.18 for V1.0.7
#define MSG_HEADER_ADD_FANCLUB_MEETING  0x8     // 07.08.18 for V1.1.2
// clang-format on

#define MSG_HEADER_VERSION MSG_HEADER_ADD_FANCLUB_MEETING

#define MAX_DATAGRAMM_SIZE 512

class MessageProtocol
{
public:
    MessageProtocol();
    MessageProtocol(QByteArray& data);
    MessageProtocol(const quint32 index);
    MessageProtocol(const quint32 index, QByteArray& data);
    MessageProtocol(const quint32 index, quint32 data);
    MessageProtocol(const quint32 index, qint32 data);
    MessageProtocol(const quint32 index, char* data, const quint32 size);

    quint32 getTimeStamp() { return qFromLittleEndian(this->m_pHead->m_timestamp); }
    quint32 getIndex() { return qFromLittleEndian(this->m_pHead->m_index); }
    quint32 getDataLength() { return qFromLittleEndian(this->m_pHead->m_length); }
    quint32 getVersion() { return qFromLittleEndian(this->m_pHead->m_version); }

    quint32     getNetworkSize() { return this->m_Data.size(); }
    const char* getNetworkProtocol() { return this->m_Data.constData(); }

    const char* getPointerToData()
    {
        if (this->getDataLength() == 0)
            return nullptr;
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

    msg_Header* m_pHead;
};

#endif // MESSAGEPROTOCOL_H
