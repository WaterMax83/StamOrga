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


#include <QCoreApplication>
#include <QDebug>
#include <QThread>
#include <QtCore/QDir>
#include <QtCore/QFileInfo>

#include <iostream>
#include <signal.h>
#include <stdlib.h>

#include "../Common/General/globalfunctions.h"
#include "console.h"
#include "usercommand.h"


#define SETTINGS_PATH "/Settings/settings.ini"

#define ADD_USER "adduser"
#define ADD_USER_SIZE QString(ADD_USER).size()

Console::Console(GlobalData* pData, QObject* parent)
    : QObject(parent)
{
    /* Notifier to get console input */
    this->m_pSNotify = new QSocketNotifier(fileno(stdin), QSocketNotifier::Read, this);

    /* path for storing and reading files */
    this->m_applicationPath = QCoreApplication::applicationDirPath();

    this->m_pGlobalData = pData;

    this->m_logging = new Logging();
    this->m_logging->initialize();

    this->m_ctrlLog.Start(this->m_logging, false);
}


void Console::run()
{
    //: This is the welcome text
    std::cout << "Welcome to Console for StFaeKSC" << std::endl;
    std::cout << "> " << std::flush;

    /* connect the input of a new line in the console */
    connect(this->m_pSNotify, SIGNAL(activated(int)), this, SLOT(readCommand()));

    QDir        appDir(this->m_applicationPath);
    QStringList appDirFiles = appDir.entryList(QDir::Readable | QDir::Files);

    if (appDirFiles.size() == 0)
        return;

    for (int i = 0; i < appDirFiles.size(); i++) {
        if (appDirFiles.at(i).endsWith(".csv")) {
            qInfo() << (QString("Found %1 trying to read in").arg(appDirFiles.at(i)));
            UserCommand::runReadCommand("read " + this->m_applicationPath + "/" + appDirFiles.at(i), this->m_pGlobalData);
        }
    }
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
        } else if (qLine == "user" || qLine.left(5) == "user ") {
            UserCommand::runUserCommand(qLine, &this->m_pGlobalData->m_UserList);
        } else if (qLine == "game" || qLine.left(5) == "game ") {
            UserCommand::runGameCommand(qLine, &this->m_pGlobalData->m_GamesList);
        } else if (qLine == "ticket" || qLine.left(7) == "ticket ") {
            UserCommand::runTicketCommand(qLine, &this->m_pGlobalData->m_SeasonTicket);
        } else if (qLine == "read" || qLine.left(5) == "read ") {
            UserCommand::runReadCommand(qLine, this->m_pGlobalData);
        } else if (qLine == "log" || qLine.left(4) == "log ") {
            UserCommand::runLoggingCommand(this->m_logging, qLine);

        } else if (line.length() == 0) {

        } else {
            std::cout << "Unkown command: " << qLine.toStdString() << std::endl;
        }
        std::cout << "> " << std::flush;
    }
}

void Console::printHelp()
{
    std::cout << "\nConsole for StFaeKSC\n\n";

    std::cout << "Known commands are:" << std::endl;
    std::cout << "help:\t\t"
              << "Show this info" << std::endl;
    std::cout << "user:\t\t"
              << "use the user command" << std::endl;
    std::cout << "game:\t\t"
              << "use the game command" << std::endl;
    std::cout << "ticket:\t\t"
              << "use the ticket command" << std::endl;
    std::cout << "read %PATH%:\t"
              << "read a new file in csv file format" << std::endl;
    std::cout << "log %i:\t\t"
              << "show the last user log" << std::endl;
    std::cout << "exit:\t\t"
              << "exit the program" << std::endl;
    std::cout << "quit:\t\t"
              << "exit the program" << std::endl;
}


Console::~Console()
{
    if (this->m_ctrlLog.IsRunning())
        this->m_ctrlLog.Stop();
    delete this->m_pSNotify;
}
