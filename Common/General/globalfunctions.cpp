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

bool checkFilePathExistAndCreate(const QString &path)
{
    QFileInfo checkFileInfo(path);
    if (!checkFileInfo.exists())
    {
        /* File is not present, create it */
        qInfo().noquote() << QString("File %1 not found, maybe first run, try to create it: ").arg(path);
        if (!checkFileInfo.dir().exists() && !checkFileInfo.dir().mkpath(checkFileInfo.dir().absolutePath()))
        {
            qCritical() << "Error creating directory\n";
            return false;
        }
        QFile file(path);
        if (!file.open(QIODevice::WriteOnly))
        {
            qCritical() << "Error creating file\n";
            return false;
        }
        qInfo().noquote() << "SUCCESS";
    }
    return true;
}

QString getErrorCodeString(qint32 code)
{
    switch(code) {
    case ERROR_CODE_NEW_VERSION:
        return QString("neue Version: %1").arg(code);
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
    default:
        return QString("Unbekannter Fehler: %1").arg(code);
    }
}

#define DEFAULT_PADDING_SIZE    4

inline uint CalculatePaddingSize(uint uLength)
{
    int nRem = uLength % DEFAULT_PADDING_SIZE;
    if (nRem > 0)
        return DEFAULT_PADDING_SIZE - nRem;
    else
        return 0;
}
