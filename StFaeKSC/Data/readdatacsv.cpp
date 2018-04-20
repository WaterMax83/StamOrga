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

#include <QtCore/QDateTime>
#include <QtCore/QFile>


#include "../Common/General/globalfunctions.h"
#include "readdatacsv.h"

ReadDataCSV::ReadDataCSV(GlobalData* pGlobalData)
{
    this->m_pGlobalData = pGlobalData;
}


enum CSV_IMPORT_GROUP {
    NOTHING = 0,
    USER    = 1,
    GAME    = 2
};

QString ReadDataCSV::readNewCSVData(QString path)
{
    QFile csvFile(path);

    if (!csvFile.exists()) {
        CONSOLE_CRITICAL(QString("CSV File %1 does not exist").arg(path));
        return "Error";
    }

    if (!csvFile.open(QIODevice::ReadOnly)) {
        CONSOLE_CRITICAL(QString("Could not open file %1: %2").arg(path).arg(csvFile.errorString()));
        return "Error";
    }

    CSV_IMPORT_GROUP actGroup = CSV_IMPORT_GROUP::NOTHING;
    while (!csvFile.atEnd()) {
        QByteArray aline = csvFile.readLine();
        QString    line(aline);

        if (line.trimmed() == "GAMES")
            actGroup = CSV_IMPORT_GROUP::GAME;
        else if (line.trimmed() == "USER")
            actGroup = CSV_IMPORT_GROUP::USER;
        else {
            QStringList lline = line.trimmed().split(";");

            switch (actGroup) {
            case CSV_IMPORT_GROUP::GAME:
                this->readNewGameData(lline);
                break;

            case CSV_IMPORT_GROUP::USER:
                this->readNewUserData(lline);
                break;

            default:
                break;
            }
        }
    }
    return "Success";
}

int ReadDataCSV::readNewGameData(QStringList line)
{
    bool   ok;
    quint8 sIndex = 0;
    if (line.size() > 0) {
        sIndex = line.value(0).toUShort(&ok);
        if (!ok) {
            qWarning().noquote() << QString("Could not read %1 as integer value").arg(line.value(0));
            return ERROR_CODE_COMMON;
        }
    }

    CompetitionIndex competition = NO_COMPETITION;
    if (line.size() > 1) {
        competition = CompetitionIndex(line.value(1).toUInt(&ok));
        if (!ok) {
            qWarning().noquote() << QString("Could not read %1 as integer value").arg(line.value(1));
            return ERROR_CODE_COMMON;
        }
    }

    qint64 datetime = 0;
    if (line.size() > 2) {
        QDateTime time = QDateTime::fromString(line.value(2), "dd.MM.yyyy hh:mm");
        datetime       = time.toMSecsSinceEpoch();
    }

    QString home;
    if (line.size() > 3)
        home = line.value(3);

    QString away;
    if (line.size() > 4)
        away = line.value(4);

    QString score;
    if (line.size() > 5)
        score = line.value(5);

    this->m_pGlobalData->m_GamesList.addNewGame(home, away, datetime, sIndex, score, competition);
    this->m_pGlobalData->m_GamesList.sortItemListByTimeAscending();

    return ERROR_CODE_SUCCESS;
}

int ReadDataCSV::readNewUserData(QStringList line)
{
    if (line.length() < 2)
        return -1;

    bool    ok;
    QString name = line.at(0);
    quint32 prop = line.at(1).toInt(&ok, 16);
    if (!ok)
        return ERROR_CODE_COMMON;

    if (this->m_pGlobalData->m_UserList.itemExists(name)) {
        if (!this->m_pGlobalData->m_UserList.userChangeProperties(name, prop)) {
            qWarning() << QString("Error changing property from user %1 to 0x%2").arg(name, QString::number(prop, 16));
            return ERROR_CODE_COMMON;
        }
    } else {
        if (this->m_pGlobalData->m_UserList.addNewUser(name, name, prop) <= 0) {
            qInfo() << QString("Error adding user %1").arg(name);
            return ERROR_CODE_COMMON;
        }
        this->m_pGlobalData->m_UserList.userChangeProperties(name, prop);
    }
    return ERROR_CODE_SUCCESS;
}
