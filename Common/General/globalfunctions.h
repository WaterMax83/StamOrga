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

#ifndef GLOBALFUNCTIONS_H
#define GLOBALFUNCTIONS_H

#include <QtCore/QDebug>
#include <QtCore/QDir>
#include <QtCore/QFileInfo>
#include <QtCore/QString>

#include <iostream>


#define CONSOLE_DEBUG(msg)                       \
    std::cout << msg.toStdString() << std::endl; \
    qDebug().noquote() << msg;
#define CONSOLE_INFO(msg)                        \
    std::cout << msg.toStdString() << std::endl; \
    qInfo().noquote() << msg;
#define CONSOLE_WARNING(msg)                     \
    std::cout << msg.toStdString() << std::endl; \
    qWarning().noquote() << msg;
#define CONSOLE_CRITICAL(msg)                    \
    std::cout << msg.toStdString() << std::endl; \
    qCritical().noquote() << msg;
#define CONSOLE_ONLY(msg) std::cout << msg.toStdString() << std::endl


inline void SetMessagePattern()
{
    qSetMessagePattern("%{time dd.MM hh:mm:ss.zzz} %{type} - %{message}");
}

extern QString getUserHomePath();

extern QString getUserAppDataLocation();

extern QString getUserHomeConfigPath();

extern bool checkFilePathExistAndCreate(const QString& path);


#define ERROR_CODE_NEW_VERSION 5
#define ERROR_CODE_SUCCESS 1
#define ERROR_CODE_NO_ERROR 0
#define ERROR_CODE_COMMON -1
#define ERROR_CODE_WRONG_SIZE -2
#define ERROR_CODE_TIMEOUT -3
#define ERROR_CODE_ERR_SEND -4
#define ERROR_CODE_NOT_FOUND -5
#define ERROR_CODE_NO_CONNECT -6
#define ERROR_CODE_ALREADY_EXIST -7
#define ERROR_CODE_IN_PAST -8
#define ERROR_CODE_NOT_POSSIBLE -9
#define ERROR_CODE_MISSING_TICKET -10
#define ERROR_CODE_WRONG_PARAMETER -11
#define ERROR_CODE_NO_USER -20
#define ERROR_CODE_WRONG_PASSWORD -21


extern QString getErrorCodeString(qint32 code);


enum TICKET_STATE {
    TICKET_STATE_NOT_POSSIBLE = 0,
    TICKET_STATE_BLOCKED      = 1,
    TICKET_STATE_FREE         = 2,
    TICKET_STATE_RESERVED     = 3,
};

enum CompetitionIndex {
    NO_COMPETITION   = 0,
    BUNDESLIGA_1     = 1,
    BUNDESLIGA_2     = 2,
    LIGA_3           = 3,
    DFB_POKAL        = 4,
    KROMBACHER_POKAL = 5,
    TESTSPIEL        = 6,
    MAX_COMPETITION  = 7,
};

extern CompetitionIndex getCompetitionIndex(QString comp);
extern QString getCompetitionString(CompetitionIndex index);


#define USER_ENABLE_LOG 0x1
#define USER_ENABLE_ADD_GAME 0x2


#endif // GLOBALFUNCTIONS_H
