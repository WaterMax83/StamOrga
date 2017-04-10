#ifndef USERCOMMAND_H
#define USERCOMMAND_H

#include <QtCore/QDebug>
#include <QtCore/QStringList>

#include <iostream>

#include "../Common/General/logging.h"
#include "../Data/readdatacsv.h"
#include "../General/globaldata.h"

class UserCommand
{

public:
    static int runUserCommand(const QString& cmd, ListedUser* pUsers)
    {
        QStringList list = cmd.split(' ');
        if (list.size() < 2 || list.value(0) != "user")
            return ShowUserCommandHelp();

        if (list.value(1) == "add" && list.size() == 3)
            return pUsers->addNewUser(list.value(2));
        else if (list.value(1) == "remove" && list.size() == 3)
            return pUsers->removeUser(list.value(2));
        else if (list.value(1) == "change" && list.size() == 4) {
            if (pUsers->userExists(list.value(2))) {
                if (pUsers->userChangePassword(list.value(2), list.value(3)))
                    std::cout << "Changed password from user " << list.value(2).toStdString() << std::endl;
                else
                    std::cout << QString("Error changing password %2 from user %1").arg(list.value(3), list.value(2)).toStdString();
                return 0;
            } else {
                std::cout << "User " << list.value(2).toStdString() << " does not exist" << std::endl;
            }
        } else if (list.value(1) == "prop" && list.size() == 4) {
            if (pUsers->userExists(list.value(2))) {
                bool    ok;
                quint32 prop = list.value(3).toInt(&ok, 16);
                if (ok && pUsers->userChangeProperties(list.value(2), prop))
                    std::cout << "Changed property from user " << list.value(2).toStdString() << std::endl;
                else
                    std::cout << QString("Error changing property %2 from user %1").arg(list.value(3), list.value(2)).toStdString();
                return 0;
            } else {
                std::cout << "User " << list.value(2).toStdString() << " does not exist" << std::endl;
            }
        } else if (list.value(1) == "show" && list.size() == 2)
            return pUsers->showAllUsers();

        return ShowUserCommandHelp();
    }

    static int runLoggingCommand(const QString& cmd)
    {
        QStringList list = cmd.split(' ');
        if (list.size() < 2 || list.value(0) != "log")
            return showLoggingInfo(5);

        bool ok;
        int  numb = list.value(1).toInt(&ok);
        if (ok)
            return showLoggingInfo(numb);
        return showLoggingInfo(5);
    }

    static int runGameCommand(const QString& cmd, Games* pGames)
    {
        QStringList list = cmd.split(' ');
        if (list.size() < 2 || list.value(0) != "game")
            return ShowGamesCommandHelp();

        if (list.value(1) == "show" && list.size() == 2)
            return pGames->showAllGames();

        return ShowGamesCommandHelp();
    }

    static int runTicketCommand(const QString& cmd, SeasonTicket* pTicket)
    {
        QStringList list = cmd.split(' ');
        if (list.size() < 2 || list.value(0) != "ticket")
            return ShowTicketsCommandHelp();

        if (list.value(1) == "show" && list.size() == 2)
            return pTicket->showAllSeasonTickets();

        return ShowTicketsCommandHelp();
    }

    static int runReadCommand(const QString& cmd, GlobalData* pData)
    {
        QStringList list = cmd.split(' ');

        if (list.size() != 2 || list.value(0) != "read")
            return ShowReadCommandHelp();


        ReadDataCSV csv(pData);
        return csv.readNewCSVData(list.value(1));
    }

private:
    static int ShowUserCommandHelp()
    {
        std::cout << "User functions - Usage\n\n";

        std::cout << "add %NAME%\t\t"
                  << "add a new user" << std::endl;
        std::cout << "remove %NAME%\t\t"
                  << "remove a user" << std::endl;
        std::cout << "change %NAME% %PASSW%\t"
                  << "change password a user" << std::endl;
        std::cout << "prop %NAME% %PROP%\t"
                  << "change property a user" << std::endl;
        std::cout << "show\t\t\t"
                  << "show all users" << std::endl;

        return 0;
    }

    static int ShowGamesCommandHelp()
    {
        std::cout << "Games functions - Usage\n\n";

        std::cout << "show\t\t\t"
                  << "show all games" << std::endl;

        return 0;
    }

    static int ShowTicketsCommandHelp()
    {
        std::cout << "Ticket functions - Usage\n\n";

        std::cout << "show\t\t\t"
                  << "show all season tickets" << std::endl;

        return 0;
    }

    static int ShowReadCommandHelp()
    {
        std::cout << "Read functions - Usage\n\n";

        std::cout << "%PATH%\t\t\t"
                  << "file path to a csv file" << std::endl;

        return 0;
    }
};

#endif // USERCOMMAND_H
