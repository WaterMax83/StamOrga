#ifndef GLOBALDATA_H
#define GLOBALDATA_H

#include "../Data/listeduser.h"
#include "../Data/games.h"

class GlobalData
{
public:
    GlobalData();

    ListedUser m_UserList;

    Games   m_GamesList;

};

#endif // GLOBALDATA_H
