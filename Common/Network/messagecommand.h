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

#ifndef MESSAGECOMMAND_H
#define MESSAGECOMMAND_H


enum OP_CODE_CMD_REQ {
    REQ_CONNECT_USER         = 0x00010001,
    REQ_LOGIN_USER           = 0x00020001,
    REQ_GET_USER_PROPS       = 0x00040001,
    REQ_USER_CHANGE_LOGIN    = 0x00080001,
    REQ_USER_CHANGE_READNAME = 0x00100001,
    REQ_GET_VERSION          = 0x00030002,
    REQ_GET_GAMES_LIST       = 0x00010004,
    REQ_GET_TICKETS_LIST     = 0x00010008,
    REQ_ADD_TICKET           = 0x00020008,
    REQ_REMOVE_TICKET        = 0x00040008,
    REQ_NEW_TICKET_PLACE     = 0x00080008,

    REQ_FREE_SEASON_TICKET = 0x00200001,
};

enum OP_CODE_CMD_RES {
    ACK_CONNECT_USER         = 0x10010001,
    ACK_LOGIN_USER           = 0x10020001,
    ACK_GET_USER_PROPS       = 0x10040001,
    ACK_USER_CHANGE_LOGIN    = 0x10080001,
    ACK_USER_CHANGE_READNAME = 0x10100001,
    ACK_GET_VERSION          = 0x10030002,
    ACK_GET_GAMES_LIST       = 0x10010004,
    ACK_GET_TICKETS_LIST     = 0x10010008,
    ACK_ADD_TICKET           = 0x10020008,
    ACK_REMOVE_TICKET        = 0x10040008,
    ACK_NEW_TICKET_PLACE     = 0x10080008,

    ACK_FREE_SEASON_TICKET = 0x10200001,

    ACK_NOT_LOGGED_IN = 0x1F00FFFF,
};

class MessageCommand
{
public:
    MessageCommand();
};

#endif // MESSAGECOMMAND_H
