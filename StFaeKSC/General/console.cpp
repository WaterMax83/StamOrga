
#include <QCoreApplication>
#include <QThread>
#include <QDebug>

#include <iostream>
#include <signal.h>
#include <stdlib.h>

#include "console.h"


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
    qDebug().noquote() << tr("Welcome to Console for StFaeKSC");
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
        qDebug().noquote() << tr("Ending console!");
        emit quit();
    } else {
        QString qLine = QString::fromStdString(line);
        if (qLine == "help")
        {
            this->printHelp();
        }
        else if (qLine.length() > ADD_USER_SIZE && qLine.left(ADD_USER_SIZE) == ADD_USER)
        {
            this->m_pGlobalData->m_UserList.addNewUser(qLine.right(qLine.length() - ADD_USER_SIZE).trimmed());
        }
        else if (line.length() == 0)
        {

        }
        else
        {
            qDebug().noquote() << tr("Unkown command: ") << qLine;
        }
        std::cout << "> " << std::flush;
    }
}

void Console::printHelp()
{
    qDebug().noquote() << tr("\nConsole for StFaeKSC\n\n");

    qDebug().noquote() << tr("Known commands are:");
    qDebug().noquote() << "help:\t\t"   << tr("Show this info");
    qDebug().noquote() << ADD_USER << " %NAME%\t" << "Add a new user";
}


Console::~Console()
{
    delete this->m_pSNotify;
}
