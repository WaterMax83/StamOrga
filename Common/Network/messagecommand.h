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
    REQ_LOGIN_USER           = 0x00010002,
    REQ_GET_USER_PROPS       = 0x00010004,
    REQ_USER_CHANGE_LOGIN    = 0x00010008,
    REQ_USER_CHANGE_READNAME = 0x00010010,
    REQ_GET_VERSION          = 0x00010011,
    REQ_GET_GAMES_LIST       = 0x00020001,
    REQ_GET_TICKETS_LIST     = 0x00030001,
    REQ_ADD_TICKET           = 0x00030002,
    REQ_REMOVE_TICKET        = 0x00030004,
    REQ_NEW_TICKET_PLACE     = 0x00030008,

    REQ_STATE_CHANGE_SEASON_TICKET = 0x00040001,
    REQ_GET_AVAILABLE_TICKETS      = 0x00040002,

    REQ_CHANGE_GAME = 0x00080001,
};

enum OP_CODE_CMD_RES {
    ACK_CONNECT_USER         = 0x10010001,
    ACK_LOGIN_USER           = 0x10010002,
    ACK_GET_USER_PROPS       = 0x10010004,
    ACK_USER_CHANGE_LOGIN    = 0x10010008,
    ACK_USER_CHANGE_READNAME = 0x10010010,
    ACK_GET_VERSION          = 0x10010011,
    ACK_GET_GAMES_LIST       = 0x10020001,
    ACK_GET_TICKETS_LIST     = 0x10030001,
    ACK_ADD_TICKET           = 0x10030002,
    ACK_REMOVE_TICKET        = 0x10030004,
    ACK_NEW_TICKET_PLACE     = 0x10030008,

    ACK_STATE_CHANGE_SEASON_TICKET = 0x10040001,
    ACK_GET_AVAILABLE_TICKETS      = 0x10040002,

    ACK_CHANGE_GAME = 0x10080001,

    ACK_NOT_LOGGED_IN = 0x1F00FFFF,
};

class MessageCommand
{
public:
    MessageCommand();
};

#endif // MESSAGECOMMAND_H
