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

#ifndef USERCOMMAND_H
#define USERCOMMAND_H

#include <QtCore/QDebug>
#include <QtCore/QStringList>

#include <iostream>

#include "../Common/General/logging.h"
#include "../Common/General/logging.h"
#include "../Data/readdatacsv.h"
#include "../General/globaldata.h"
#include "pushnotification.h"

class UserCommand
{

public:
    static QString runUserCommand(const QString& cmd, ListedUser* pUsers)
    {
        QString     rValue;
        QStringList list = cmd.split(' ');
        if (list.size() < 2 || list.value(0) != "user")
            return ShowUserCommandHelp();

        if (list.value(1) == "add" && list.size() == 3) {
            int result = pUsers->addNewUser(list.value(2));
            if (result > 0)
                rValue.append(QString("Added new user: %1\n").arg(list.value(2)));
            return rValue;
        } else if (list.value(1) == "remove" && list.size() == 3) {
            if (pUsers->removeItem(list.value(2)) == ERROR_CODE_SUCCESS)
                rValue.append(QString("Removed user %1\n").arg(list.value(2)));
            return rValue;
        } else if (list.value(1) == "change" && list.size() == 4) {
            if (pUsers->itemExists(list.value(2))) {
                if (pUsers->userChangePassword(list.value(2), list.value(3)))
                    rValue.append(QString("Changed password from user %1\n").arg(list.value(2)));
                else
                    rValue.append(QString("Error changing password %2 from user %1\n").arg(list.value(3), list.value(2)));
                return rValue;
            } else {
                rValue.append(QString("User %1 does not exist\n").arg(list.value(2)));
            }
        } else if (list.value(1) == "prop" && list.size() == 4) {
            if (pUsers->itemExists(list.value(2))) {
                bool    ok;
                quint32 prop = list.value(3).toInt(&ok, 16);
                if (ok && pUsers->userChangeProperties(list.value(2), prop))
                    rValue.append(QString("Changed property from user %1\n").arg(list.value(2)));
                else
                    rValue.append(QString("Error changing property %2 from user %1").arg(list.value(3), list.value(2)));
                return rValue;
            } else {
                rValue.append(QString("User %1 does not exist\n").arg(list.value(2)));
            }
        } else if (list.value(1) == "show" && list.size() == 2)
            return pUsers->showAllUsers();

        return ShowUserCommandHelp();
    }

    static QString runLoggingCommand(Logging* log, const QString& cmd)
    {
        QStringList list = cmd.split(' ');
        if (list.size() < 2 || list.value(0) != "log")
            return log->showLoggingInfo(5);

        bool ok;
        int  numb = list.value(1).toInt(&ok);
        if (ok)
            return log->showLoggingInfo(numb);
        return log->showLoggingInfo(5);
    }

    static QString runGameCommand(const QString& cmd, Games* pGames)
    {
        QStringList list = cmd.split(' ');
        if (list.size() < 2 || list.value(0) != "game")
            return ShowGamesCommandHelp();

        if (list.value(1) == "show" && list.size() == 2)
            return pGames->showAllGames(false);
        else if (list.value(1) == "show" && list.size() == 3 && list.value(2) == "update")
            return pGames->showAllGames(true);

        return ShowGamesCommandHelp();
    }

    static QString runTicketCommand(const QString& cmd, SeasonTicket* pTicket)
    {
        QStringList list = cmd.split(' ');
        if (list.size() < 2 || list.value(0) != "ticket")
            return ShowTicketsCommandHelp();

        if (list.value(1) == "show" && list.size() == 2)
            return pTicket->showAllSeasonTickets();

        return ShowTicketsCommandHelp();
    }

    static QString runReadCommand(const QString& cmd, GlobalData* pData)
    {
        QStringList list = cmd.split(' ');

        if (list.size() != 2 || list.value(0) != "read")
            return ShowReadCommandHelp();


        ReadDataCSV csv(pData);
        return csv.readNewCSVData(list.value(1));
    }

    static QString runFanclubNewsCommand(const QString& cmd, FanclubNews* pNews)
    {
        QStringList list = cmd.split(' ');
        if (list.size() < 2 || list.value(0) != "news")
            return ShowFanclubNewsHelp();

        if (list.value(1) == "show" && list.size() == 2)
            return pNews->showNewsData();

        return ShowFanclubNewsHelp();
    }

    static QString runTokenCommand(const QString& cmd)
    {
        QStringList list = cmd.split(' ');
        if (list.size() != 2 || list.value(0) != "token")
            return ShowTokenHelp();

        if (list.value(1) == "token" || list.value(1) == "guid" || list.value(1) == "version") {
            return g_pushNotify->showCurrentTokenInformation(list.value(1));
        }

        return ShowTokenHelp();
    }

private:
    static QString ShowUserCommandHelp()
    {
        QString rValue;
        rValue.append("User functions - Usage\n\n");

        rValue.append("add %NAME%\t\t");
        rValue.append("add a new user\n");
        rValue.append("remove %NAME%\t\t");
        rValue.append("remove a user\n");
        rValue.append("change %NAME% %PASSW%\t");
        rValue.append("change password a user\n");
        rValue.append("prop %NAME% %PROP%\t");
        rValue.append("change property a user\n");
        rValue.append("show\t\t\t");
        rValue.append("show all users\n");

        return rValue;
    }

    static QString ShowGamesCommandHelp()
    {
        QString rValue;
        rValue.append("Games functions - Usage\n\n");

        rValue.append("show [update]\t\t\t");
        rValue.append("show all games\n");

        return rValue;
    }

    static QString ShowTicketsCommandHelp()
    {
        QString rValue;
        rValue.append("Ticket functions - Usage\n\n");

        rValue.append("show\t\t\t");
        rValue.append("show all season tickets\n");

        return rValue;
    }

    static QString ShowReadCommandHelp()
    {
        QString rValue;
        rValue.append("Read functions - Usage\n\n");

        rValue.append("%PATH%\t\t\t");
        rValue.append("file path to a csv file\n");

        return rValue;
    }

    static QString ShowFanclubNewsHelp()
    {
        QString rValue;
        rValue.append("News functions - Usage\n\n");

        rValue.append("show\t\t\t");
        rValue.append("show all news\n");

        return rValue;
    }

    static QString ShowTokenHelp()
    {
        QString rValue;
        rValue.append("Token functions - Usage\n\n");

        rValue.append("token\t\t\t");
        rValue.append("show all token information\n");
        rValue.append("guid\t\t\t");
        rValue.append("show all guid information\n");
        rValue.append("version\t\t\t");
        rValue.append("show all version information\n");

        return rValue;
    }
};

#endif // USERCOMMAND_H
