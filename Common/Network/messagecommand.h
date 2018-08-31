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
    REQ_GET_USER_EVENTS      = 0x00010012, /* only for udp */
    REQ_SEND_CONSOLE_CMD     = 0x00010012,
    REQ_SET_USER_EVENTS      = 0x00010014,

    REQ_GET_GAMES_LIST      = 0x00020001,
    REQ_GET_GAMES_INFO_LIST = 0x00020002,
    REQ_SET_FIXED_GAME_TIME = 0x00020004,
    REQ_CHANGE_GAME_TCP     = 0x00020008,
    REQ_GET_GAME_EVENTS     = 0x00020016,

    REQ_GET_TICKETS_LIST = 0x00030001,
    REQ_ADD_TICKET       = 0x00030002,
    REQ_REMOVE_TICKET    = 0x00030004,
    REQ_NEW_TICKET_PLACE = 0x00030008,
    REQ_CHANGE_TICKET    = 0x00030010,

    REQ_STATE_CHANGE_SEASON_TICKET = 0x00040001,
    REQ_GET_AVAILABLE_TICKETS      = 0x00040002,

    REQ_CHANGE_NEWS_DATA   = 0x00050001,
    REQ_GET_NEWS_DATA_LIST = 0x00050002,
    REQ_GET_NEWS_DATA_ITEM = 0x00050004,
    REQ_DEL_NEWS_DATA_ITEM = 0x00050008,

    REQ_CMD_STATISTIC = 0x00060001,
    REQ_CMD_CONTROL   = 0x00060002,
    REQ_CMD_MEDIA     = 0x00060004,

    REQ_CHANGE_GAME_UDP = 0x00080001,

    REQ_CHANGE_MEETING_INFO  = 0x00100001,
    REQ_GET_MEETING_INFO     = 0x00100002,
    REQ_ACCEPT_MEETING       = 0x00100004,
    REQ_SEND_COMMENT_MEET    = 0x00100008,
    REQ_CHANGE_AWAYTRIP_INFO = 0x00100101,
    REQ_GET_AWAYTRIP_INFO    = 0x00100102,
    REQ_ACCEPT_AWAYTRIP      = 0x00100104,
    REQ_SEND_COMMENT_TRIP    = 0x00100108,
};

enum OP_CODE_CMD_RES {
    ACK_CONNECT_USER         = 0x10010001,
    ACK_LOGIN_USER           = 0x10010002,
    ACK_GET_USER_PROPS       = 0x10010004,
    ACK_USER_CHANGE_LOGIN    = 0x10010008,
    ACK_USER_CHANGE_READNAME = 0x10010010,
    ACK_GET_VERSION          = 0x10010011,
    ACK_GET_USER_EVENTS      = 0x00010012, /* only for upd */
    ACK_SEND_CONSOLE_CMD     = 0x10010012,
    ACK_SET_USER_EVENTS      = 0x00010014,

    ACK_GET_GAMES_LIST      = 0x10020001,
    ACK_GET_GAMES_INFO_LIST = 0x10020002,
    ACK_SET_FIXED_GAME_TIME = 0x10020004,
    ACK_CHANGE_GAME_TCP     = 0x00020008,
    ACK_GET_GAME_EVENTS     = 0x10020016,

    ACK_GET_TICKETS_LIST = 0x10030001,
    ACK_ADD_TICKET       = 0x10030002,
    ACK_REMOVE_TICKET    = 0x10030004,
    ACK_NEW_TICKET_PLACE = 0x10030008,
    ACK_CHANGE_TICKET    = 0x10030010,

    ACK_STATE_CHANGE_SEASON_TICKET = 0x10040001,
    ACK_GET_AVAILABLE_TICKETS      = 0x10040002,

    ACK_CHANGE_NEWS_DATA   = 0x10050001,
    ACK_GET_NEWS_DATA_LIST = 0x10050002,
    ACK_GET_NEWS_DATA_ITEM = 0x10050004,
    ACK_DEL_NEWS_DATA_ITEM = 0x10050008,

    ACK_CMD_STATISTIC = 0x10060001,
    ACK_CMD_CONTROL   = 0x10060002,
    ACK_CMD_MEDIA     = 0x10060004,

    ACK_CHANGE_GAME_UDP = 0x10080001,

    ACK_CHANGE_MEETING_INFO  = 0x10100001,
    ACK_GET_MEETING_INFO     = 0x10100002,
    ACK_ACCEPT_MEETING       = 0x10100004,
    ACK_SEND_COMMENT_MEET    = 0x10100008,
    ACK_CHANGE_AWAYTRIP_INFO = 0x10100101,
    ACK_GET_AWAYTRIP_INFO    = 0x10100102,
    ACK_ACCEPT_AWAYTRIP      = 0x10100104,
    ACK_SEND_COMMENT_TRIP    = 0x10100108,

    ACK_NOT_LOGGED_IN = 0x1F00FFFF,
};

class MessageCommand
{
public:
    MessageCommand();
};

#endif // MESSAGECOMMAND_H
