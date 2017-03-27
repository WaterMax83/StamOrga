
#include <QCoreApplication>
#include <QThread>
#include <QDebug>

#include <iostream>
#include <signal.h>
#include <stdlib.h>

#include "console.h"
#include "usercommand.h"


#define SETTINGS_PATH "/Settings/settings.ini"

#define ADD_USER        "adduser"
#define ADD_USER_SIZE   QString(ADD_USER).size()

Console::Console(GlobalData *pData, QObject *parent) : QObject(parent)
{
    /* Notifier to get console input */
    this->m_pSNotify = new QSocketNotifier(fileno(stdin), QSocketNotifier::Read,this);

    /* path for storing and reading files */
    this->m_applicationPath = QCoreApplication::applicationDirPath();


    this->m_pGlobalData = pData;

}


void Console::run()
{
    //: This is the welcome text
    std::cout << "Welcome to Console for StFaeKSC" << std::endl;
    std::cout << "> " << std::flush;

    /* connect the input of a new line in the console */
    connect(this->m_pSNotify, SIGNAL(activated(int)), this, SLOT(readCommand()));

}

/* parsing the input commands */
void Console::readCommand()
{
    std::string line;
    std::getline(std::cin, line);
    if (std::cin.eof() || line == "quit" || line == "exit") {
        std::cout << "Ending console!" << std::endl;
        emit quit();
    } else {
        QString qLine = QString::fromStdString(line);
        if (qLine == "help") {
            this->printHelp();
        }
        else if (qLine == "user" || qLine.left(5) == "user ") {
            UserCommand::runUserCommand(qLine, &this->m_pGlobalData->m_UserList);
        }
        else if (qLine == "log" || qLine.left(4) == "log ") {
            UserCommand::runLoggingCommand(qLine);

        }
        else if (line.length() == 0) {

        }
        else {
            std::cout << "Unkown command: " << qLine.toStdString() << std::endl;
        }
        std::cout << "> " << std::flush;
    }
}

void Console::printHelp()
{
    std::cout << "\nConsole for StFaeKSC\n\n";

    std::cout << "Known commands are:" << std::endl;
    std::cout << "help:\t\t"   << "Show this info" << std::endl;
    std::cout << "user:\t\t"   << "use the user command" << std::endl;
    std::cout << "log %i:\t\t"   << "show the last user log" << std::endl;
    std::cout << "exit:\t\t"   << "exit the program" << std::endl;
    std::cout << "quit:\t\t"   << "exit the program" << std::endl;
}


Console::~Console()
{
    delete this->m_pSNotify;
}
