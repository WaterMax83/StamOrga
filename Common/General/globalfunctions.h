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

// clang-format off
#define ERROR_CODE_NEW_VERSION          5
#define ERROR_CODE_UPDATE_APP           4
#define ERROR_CODE_SUCCESS              1
#define ERROR_CODE_NO_ERROR             0
#define ERROR_CODE_COMMON               -1
#define ERROR_CODE_WRONG_SIZE           -2
#define ERROR_CODE_TIMEOUT              -3
#define ERROR_CODE_ERR_SEND             -4
#define ERROR_CODE_NOT_FOUND            -5
#define ERROR_CODE_NO_CONNECT           -6
#define ERROR_CODE_ALREADY_EXIST        -7
#define ERROR_CODE_IN_PAST              -8
#define ERROR_CODE_NOT_POSSIBLE         -9
#define ERROR_CODE_MISSING_TICKET       -10
#define ERROR_CODE_WRONG_PARAMETER      -11
#define ERROR_CODE_NOT_IMPLEMENTED      -12
#define ERROR_CODE_UPDATE_FUNCTION      -13
#define ERROR_CODE_UPDATE_LIST          -14
#define ERROR_CODE_NOT_READY            -15
#define ERROR_CODE_MISSING_PARAMETER    -16
#define ERROR_CODE_NO_USER              -20
#define ERROR_CODE_WRONG_PASSWORD       -21
#define ERROR_CODE_NOT_INITIALIZED      -22
// clang-format on

extern QString getErrorCodeString(qint32 code);


enum TICKET_STATE {
    TICKET_STATE_NOT_POSSIBLE = 0,
    TICKET_STATE_BLOCKED      = 1,
    TICKET_STATE_FREE         = 2,
    TICKET_STATE_RESERVED     = 3,
};

enum ACCEPT_STATE {
    ACCEPT_STATE_NOT_POSSIBLE = 0,
    ACCEPT_STATE_ACCEPT       = 1,
    ACCEPT_STATE_MAYBE        = 2,
    ACCEPT_STATE_DECLINE      = 3,
};

enum CompetitionIndex {
    NO_COMPETITION  = 0,
    BUNDESLIGA_1    = 1,
    BUNDESLIGA_2    = 2,
    LIGA_3          = 3,
    DFB_POKAL       = 4,
    BADISCHER_POKAL = 5,
    TESTSPIEL       = 6,
    MAX_COMPETITION = 7,
};

enum UpdateIndex {
    UpdateAll  = 0,
    UpdateDiff = 1,
};

enum MeetingType {
    MEETING_TYPE_MEETING  = 0,
    MEETING_TYPE_AWAYTRIP = 1,
};

extern CompetitionIndex getCompetitionIndex(QString comp);
extern QString          getCompetitionString(CompetitionIndex index);
extern QString          createRandomString(qint32 size);

// clang-format off
#define USER_ENABLE_LOG                     0x0001
#define USER_ENABLE_ADD_GAME                0x0002
#define USER_ENABLE_FIXED_GAME_TIME         0x0004
#define USER_ENABLE_FANCLUB                 0x0008
#define USER_ENABLE_FANCLUB_EDIT            0x0010
// clang-format on


// clang-format off
#define NOTIFY_TOPIC_NEW_APP_VERSION        "NewAppVersion"
#define NOTIFY_TOPIC_NEW_MEETING            "NewMeeting"
#define NOTIFY_TOPIC_CHANGE_MEETING         "ChangeMeeting"
#define NOTIFY_TOPIC_NEW_FREE_TICKET        "NewFreeTicket"
#define NOTIFY_TOPIC_NEW_AWAY_ACCEPT        "NewAwayAccept"
#define NOTIFY_TOPIC_NEW_FANCLUB_NEWS       "FanclubNews"
#define NOTIFY_TOPIC_GENERAL                "GeneralTopic"
#define NOTIFY_TOPIC_GENERAL_BACKUP         "GeneralBackup"     // after versoin 1.0.3
// clang-format on


#endif // GLOBALFUNCTIONS_H
