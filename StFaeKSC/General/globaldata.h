#ifndef GLOBALDATA_H
#define GLOBALDATA_H

#include "../Data/games.h"
#include "../Data/listeduser.h"
#include "../Data/seasonticket.h"

class GlobalData
{
public:
    GlobalData();

    ListedUser   m_UserList;
    Games        m_GamesList;
    SeasonTicket m_SeasonTicket;
};

#endif // GLOBALDATA_H
