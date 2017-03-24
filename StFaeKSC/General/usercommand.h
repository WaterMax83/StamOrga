#ifndef USERCOMMAND_H
#define USERCOMMAND_H

#include <QtCore/QDebug>
#include <QtCore/QStringList>

#include <iostream>

#include "listeduser.h"

class UserCommand
{

public:

    static int runUserCommand(const QString &cmd, ListedUser *pUsers)
    {
        QStringList list = cmd.split(' ');
        if (list.size() < 2 || list.value(0) != "user")
            return ShowUserCommandHelp();

        if (list.value(1) == "add" && list.size() == 3)
            return pUsers->addNewUser(list.value(2));
        else if (list.value(1) == "remove" && list.size() == 3)
            return pUsers->removeUser(list.value(2));
        else if (list.value(1) == "show" && list.size() == 2)
            return pUsers->showAllUsers();

        return ShowUserCommandHelp();
    }

private:
    static int ShowUserCommandHelp()
    {
        std::cout << "User functions - Usage\n\n";

        std::cout << "add %NAME%\t\t"       << "add a new user" << std::endl;
        std::cout << "remove %NAME%\t"      << "remove a user" << std::endl;
        std::cout << "show\t\t"             << "show all users" << std::endl;

        return 0;
    }

};

#endif // USERCOMMAND_H
