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
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>

#include <iostream>
#include <signal.h>
#include <stdlib.h>

#include "../Common/General/globalfunctions.h"
#include "../Common/Network/messagecommand.h"
#include "console.h"
#include "usercommand.h"


Console*           g_Console;
extern GlobalData* g_GlobalData;

#define SETTINGS_PATH "/Settings/settings.ini"

#define ADD_USER "adduser"
#define ADD_USER_SIZE QString(ADD_USER).size()

Console::Console(QObject* parent)
    : QObject(parent)
{
    /* Notifier to get console input */
    this->m_pSNotify = new QSocketNotifier(fileno(stdin), QSocketNotifier::Read, this);

    /* path for storing and reading files */
    this->m_applicationPath = QCoreApplication::applicationDirPath();

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

    //    for (int i = 0; i < appDirFiles.size(); i++) {
    //        if (appDirFiles.at(i).endsWith(".csv")) {
    //            qInfo() << (QString("Found %1 trying to read in").arg(appDirFiles.at(i)));
    //            UserCommand::runReadCommand("read " + this->m_applicationPath + "/" + appDirFiles.at(i), this->m_pGlobalData);
    //        }
    //    }
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
        QString qLine  = QString::fromStdString(line);
        QString output = this->runCommand(qLine);
        std::cout << output.toStdString() << std::endl;
        std::cout << "> " << std::flush;
    }
}

QString Console::runCommand(QString& command)
{
    QString rValue;
    if (command == "help") {
        rValue = this->printHelp();
    } else if (command == "user" || command.left(5) == "user ") {
        rValue = UserCommand::runUserCommand(command, &g_GlobalData->m_UserList);
    } else if (command == "game" || command.left(5) == "game ") {
        rValue = UserCommand::runGameCommand(command, &g_GlobalData->m_GamesList);
    } else if (command == "ticket" || command.left(7) == "ticket ") {
        rValue = UserCommand::runTicketCommand(command, &g_GlobalData->m_SeasonTicket);
    } else if (command == "token" || command.left(6) == "token ") {
        rValue = UserCommand::runTokenCommand(command);
    } else if (command == "news" || command.left(5) == "news ") {
        rValue = UserCommand::runFanclubNewsCommand(command, &g_GlobalData->m_fanclubNews);
    } else if (command == "read" || command.left(5) == "read ") {
        rValue = UserCommand::runReadCommand(command, g_GlobalData);
    } else if (command == "log" || command.left(4) == "log ") {
        rValue = UserCommand::runLoggingCommand(this->m_logging, command);
    } else if (command.length() == 0) {

    } else {
        rValue = QString("Unkown command: %1\n").arg(command);
    }

    return rValue;
}

MessageProtocol* Console::getCommandAnswer(UserConData* pUserCon, MessageProtocol* request)
{
    QByteArray  data    = QByteArray(request->getPointerToData());
    QJsonObject rootObj = QJsonDocument::fromJson(data).object();

    QString    command = rootObj.value("cmd").toString();
    QByteArray result  = qCompress(this->runCommand(command).toUtf8(), 9);

    QJsonObject rootAns;
    rootAns.insert("ack", ERROR_CODE_SUCCESS);
    rootAns.insert("result", QString(result.toBase64()));

    QByteArray answer = QJsonDocument(rootAns).toJson(QJsonDocument::Compact);

    qInfo().noquote() << QString("User %1 run command %2").arg(pUserCon->m_userName).arg(command);

    return new MessageProtocol(OP_CODE_CMD_RES::ACK_SEND_CONSOLE_CMD, answer);
}

QString Console::printHelp()
{
    QString rValue;
    rValue.append("\nConsole for StFaeKSC\n\n");

    rValue.append("Known commands are:\n");
    rValue.append(QString("help:").leftJustified(15));
    rValue.append("Show this info\n");
    rValue.append(QString("user:").leftJustified(15));
    rValue.append("use the user command\n");
    rValue.append(QString("game:").leftJustified(15));
    rValue.append("use the game command\n");
    rValue.append(QString("ticket:").leftJustified(15));
    rValue.append("use the ticket command\n");
    rValue.append(QString("token:").leftJustified(15));
    rValue.append("show the current token Info\n");
    rValue.append(QString("news:").leftJustified(15));
    rValue.append("show the news Info\n");
    rValue.append(QString("read %PATH%:").leftJustified(15));
    rValue.append("read a new file in csv file format\n");
    rValue.append(QString("log %i:").leftJustified(15));
    rValue.append("show the last user log\n");
    rValue.append(QString("exit:").leftJustified(15));
    rValue.append("exit the program\n");
    rValue.append(QString("quit:").leftJustified(15));
    rValue.append("exit the program\n");

    return rValue;
}


Console::~Console()
{
    if (this->m_ctrlLog.IsRunning())
        this->m_ctrlLog.Stop();
    delete this->m_pSNotify;
}
