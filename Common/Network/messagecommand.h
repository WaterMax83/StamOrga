#ifndef MESSAGECOMMAND_H
#define MESSAGECOMMAND_H


enum OP_CODE_CMD_REQ
{
    REQ_CONNECT_USER = 0x00010001,
};

enum OP_CODE_CMD_RES
{
    ACK_CONNECT_USER = 0x10010001,
};

class MessageCommand
{
public:
    MessageCommand();
};

#endif // MESSAGECOMMAND_H
