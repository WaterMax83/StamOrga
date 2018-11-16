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

#include <QtCore/QCoreApplication>
#include <QtCore/QDateTime>
#include <QtCore/QStandardPaths>
#include <QtCore/QUuid>

#include "globalfunctions.h"


QString getUserHomePath()
{
    return QDir::homePath();
}

QString getUserHomeConfigPath()
{
    QString m_itemName = getUserHomePath();
    m_itemName.append("/.config/StFaeKSC");
    return m_itemName;
}

QString getUserAppDataLocation()
{
    return QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
}

QString getApplicationPath()
{
    return QCoreApplication::applicationDirPath();
}

bool checkFilePathExistAndCreate(const QString& path)
{
    QFileInfo checkFileInfo(path);
    if (!checkFileInfo.exists()) {
        /* File is not present, create it */
        //        qInfo().noquote() << QString("File %1 not found, maybe first run, try to create it: ").arg(path);
        if (!checkFileInfo.dir().exists() && !checkFileInfo.dir().mkpath(checkFileInfo.dir().absolutePath())) {
            qCritical() << "Error creating directory\n";
            return false;
        }
        QFile file(path);
        if (!file.open(QIODevice::WriteOnly)) {
            qCritical() << "Error creating file\n";
            return false;
        }
    }
    return true;
}

qint32 getSeasonFromTimeStamp(qint64 msec)
{
    QDate date = QDateTime::fromMSecsSinceEpoch(msec).date();
    if (date.month() >= 6)
        return date.year();
    else
        return date.year() - 1;
}

QString getErrorCodeString(qint32 code)
{
    switch (code) {
    case ERROR_CODE_NEW_VERSION:
        return QString("neue Version: %1").arg(code);
    case ERROR_CODE_UPDATE_APP:
        return QString("Bitte App updaten: %1").arg(code);
    case ERROR_CODE_SUCCESS:
        return QString("Erfolgreich: %1").arg(code);
    case ERROR_CODE_NO_ERROR:
        return QString("Kein Fehler: %1").arg(code);
    case ERROR_CODE_COMMON:
        return QString("Fehler: Standart Fehler: %1").arg(code);
    case ERROR_CODE_WRONG_SIZE:
        return QString("Fehler: Falsche Datengroesse: %1").arg(code);
    case ERROR_CODE_TIMEOUT:
        return QString("Fehler: Keine Antwort: %1").arg(code);
    case ERROR_CODE_ERR_SEND:
        return QString("Fehler: Fehler beim Senden: %1").arg(code);
    case ERROR_CODE_NOT_FOUND:
        return QString("Objekt nicht gefunden: %1").arg(code);
    case ERROR_CODE_NO_USER:
        return QString("Fehler: Benutzer unbekannt: %1").arg(code);
    case ERROR_CODE_WRONG_PASSWORD:
        return QString("Fehler: Passwort falsch: %1").arg(code);
    case ERROR_CODE_NO_CONNECT:
        return QString("Fehler: keine Verbindung: %1").arg(code);
    case ERROR_CODE_ALREADY_EXIST:
        return QString("Fehler: existiert bereits: %1").arg(code);
    case ERROR_CODE_IN_PAST:
        return QString("Nicht möglich, liegt in der Vergangenheit: %1").arg(code);
    case ERROR_CODE_NOT_POSSIBLE:
        return QString("Nicht möglich: %1").arg(code);
    case ERROR_CODE_WRONG_PARAMETER:
        return QString("Falsche Parameter: %1").arg(code);
    case ERROR_CODE_MISSING_TICKET:
        return QString("Karte nicht vorhanden: %1").arg(code);
    case ERROR_CODE_NOT_IMPLEMENTED:
        return QString("Funktion nicht implementiert: %1").arg(code);
    case ERROR_CODE_UPDATE_FUNCTION:
        return QString("Version veraltet, Funktion nicht möglich: %1").arg(code);
    case ERROR_CODE_UPDATE_LIST:
        return QString("Liste muss aktualisiert werden: %1").arg(code);
    case ERROR_CODE_NOT_READY:
        return QString("Server is not nicht bereit: %1").arg(code);
    case ERROR_CODE_MISSING_PARAMETER:
        return QString("A Parameter is missing: %1").arg(code);
    case ERROR_CODE_NOT_INITIALIZED:
        return QString("Nicht initialisiert: %1").arg(code);
    case ERROR_CODE_NOT_LOGGED_IN:
        return QString("Nicht eingeloggt: %1").arg(code);
    case ERROR_CODE_NO_CONNECTION:
        return QString("Verbindung nicht möglich: %1").arg(code);
    case ERROR_CODE_NOT_UNIQUE:
        return QString("Element ist schon vorhanden: %1").arg(code);
    case ERROR_CODE_NOT_SUPPORTED:
        return QString("Nicht unterstützt: %1").arg(code);
    default:
        return QString("Unbekannter Fehler: %1").arg(code);
    }
}

#define DEFAULT_PADDING_SIZE 4

inline uint CalculatePaddingSize(uint uLength)
{
    int nRem = uLength % DEFAULT_PADDING_SIZE;
    if (nRem > 0)
        return DEFAULT_PADDING_SIZE - nRem;
    else
        return 0;
}

CompetitionIndex getCompetitionIndex(QString comp)
{
    if (comp == "1.Bundesliga")
        return BUNDESLIGA_1;
    if (comp == "2.Bundesliga")
        return BUNDESLIGA_2;
    if (comp == "3.Liga")
        return LIGA_3;
    if (comp == "DFB Pokal")
        return DFB_POKAL;
    if (comp == "Badischer Pokal")
        return BADISCHER_POKAL;
    if (comp == "TestSpiel")
        return TESTSPIEL;
    if (comp == "Other")
        return OTHER_COMP;

    return NO_COMPETITION;
}

QString getCompetitionString(CompetitionIndex index)
{
    switch (index) {
    case BUNDESLIGA_1:
        return "1.Bundesliga";
    case BUNDESLIGA_2:
        return "2.Bundesliga";
    case LIGA_3:
        return "3.Liga";
    case DFB_POKAL:
        return "DFB Pokal";
    case BADISCHER_POKAL:
        return "Badischer Pokal";
    case TESTSPIEL:
        return "TestSpiel";
    case OTHER_COMP:
        return "Other";
    default:
        return "not implemented";
    }
}

QString getCompetitionShortString(CompetitionIndex index)
{
    switch (index) {
    case BUNDESLIGA_1:
        return "Bl";
    case BUNDESLIGA_2:
        return "2B";
    case LIGA_3:
        return "3L";
    case DFB_POKAL:
        return "DFB";
    case BADISCHER_POKAL:
        return "Bfv";
    case TESTSPIEL:
        return "T";
    case OTHER_COMP:
        return "O";
    default:
        return "not implemented";
    }
}

QString createRandomString(qint32 size)
{
    QString rValue = QUuid::createUuid().toString();
    qint32  rand   = qrand() % (rValue.size() - size);
    rValue.remove(0, rand);
    if (rValue.length() > size)
        rValue = rValue.left(size);

    return rValue;
}

QString getTimeStampSinceString(QDateTime& time)
{
    QDateTime now = QDateTime::currentDateTime();

    qint64 days = time.daysTo(now);
    if (days >= 2) {
        if (days > 730)
            return QString("%1 Jahren").arg(days / 365);
        else if (days >= 60)
            return QString("%1 Monaten").arg(days / 30);

        return QString("%1 Tagen").arg(days);
    }

    qint64 msecs = time.msecsTo(now);
    if (msecs >= 7200000)
        return QString("%1 Stunden").arg(msecs / 3600000);
    else if (msecs >= 120000)
        return QString("%1 Minuten").arg(msecs / 60000);

    return QString("%1 Sekunden").arg(msecs / 1000);
}

bool gIsGameASeasonTicketGame(CompetitionIndex comp)
{
    switch (comp) {
    case BUNDESLIGA_1:
    case BUNDESLIGA_2:
    case LIGA_3:
        return true;
    default:
        return false;
    }

    return false;
}
