#ifndef MESSAGECOMMAND_H
#define MESSAGECOMMAND_H


enum OP_CODE_CMD_REQ
{
    REQ_CONNECT_USER        = 0x00010001,
    REQ_LOGIN_USER          = 0x00020001,
    REQ_GET_USER_PROPS      = 0x00040001,
    REQ_USER_CHANGE_LOGIN   = 0x00080001,
    REQ_USER_CHANGE_READNAME= 0x00100001,
    REQ_GET_VERSION         = 0x00030002,
    REQ_GET_GAMES_LIST      = 0x00010004,
};

enum OP_CODE_CMD_RES
{
    ACK_CONNECT_USER        = 0x10010001,
    ACK_LOGIN_USER          = 0x10020001,
    ACK_GET_USER_PROPS      = 0x10040001,
    ACK_USER_CHANGE_LOGIN   = 0x10080001,
    ACK_USER_CHANGE_READNAME= 0x10100001,
    ACK_GET_VERSION         = 0x10030002,
    ACK_GET_GAMES_LIST      = 0x10010004,
    ACK_NOT_LOGGED_IN       = 0x1F00FFFF,
};

class MessageCommand
{
public:
    MessageCommand();
};

#endif // MESSAGECOMMAND_H
