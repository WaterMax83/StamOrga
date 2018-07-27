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

#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>
#include <QtCore/QStandardPaths>

#include "../Common/General/config.h"
#include "../Common/General/globalfunctions.h"
#include "../Common/Network/messagecommand.h"
#include "Connection/cconmanager.h"
#include "Connection/cconnetworkaccess.h"
#include "cstaglobalsettings.h"
#include "cstasettingsmanager.h"
#include "cstaversionmanager.h"

// clang-format off

#define SETTINGS_GROUP      "GLOBAL_SETTINGS"

#define SETT_LAST_SHOWN_VERSION     "LastShownVersion"
// clang-format on

enum StaVersionStatus {
    VersionStatusNoInfo       = 0,
    VersionStatusNoNewVersion = 1,
    VersionStatusNewVersion   = 2,
    VersionStatusBusy         = 3,
    VersionStatusNoSSL        = 4
};

cStaVersionManager* g_StaVersionManager;

cStaVersionManager::cStaVersionManager(QObject* parent)
    : cGenDisposer(parent)
{
}

qint32 cStaVersionManager::initialize()
{
    QString value;
    g_StaSettingsManager->getValue(SETTINGS_GROUP, SETT_LAST_SHOWN_VERSION, value);
    this->m_lastShownVersion = value;

    this->m_versionUpdateIndex = VersionStatusNoInfo;
    emit this->versionUpdateIndexChanged();

    qInfo() << QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
    qInfo() << QStandardPaths::writableLocation(QStandardPaths::DownloadLocation);
    qInfo() << QStandardPaths::writableLocation(QStandardPaths::TempLocation);

    qInfo().noquote() << "SSH Version: " << QSslSocket::sslLibraryVersionString();

    connect(g_ConNetworkAccess, &cConNetworkAccess::signalDownloadFinished, this, &cStaVersionManager::slotDownloadFinished);

    this->m_initialized = true;

    return ERROR_CODE_SUCCESS;
}

qint32 cStaVersionManager::startGettingVersionInfo()
{
    QJsonObject rootObj;
    rootObj.insert("version", (double)STAM_ORGA_VERSION_I);
    QString sVersion = STAM_ORGA_VERSION_S;
#ifdef Q_OS_WIN
    sVersion.append("_Win");
#elif defined(Q_OS_ANDROID)
    sVersion.append("_Android");
#elif defined(Q_OS_IOS)
    sVersion.append(("_iOS");
#endif
    rootObj.insert("sVersion", sVersion);

    TcpDataConRequest* req = new TcpDataConRequest(OP_CODE_CMD_REQ::REQ_GET_VERSION);
    req->m_lData           = QJsonDocument(rootObj).toJson(QJsonDocument::Compact);

    g_ConManager->sendNewRequest(req);
    return ERROR_CODE_SUCCESS;
}

qint32 cStaVersionManager::handleVersionResponse(MessageProtocol* msg)
{
    QByteArray  data    = QByteArray(msg->getPointerToData());
    QJsonObject rootObj = QJsonDocument::fromJson(data).object();

    qint32  result        = rootObj.value("ack").toInt(ERROR_CODE_NOT_FOUND);
    quint32 uVersion      = (quint32)rootObj.value("version").toDouble();
    this->m_remoteVersion = rootObj.value("sVersion").toString();

    qInfo().noquote() << QString("Version from server %1:0x%2").arg(this->m_remoteVersion, QString::number(uVersion, 16));

    if ((uVersion & 0xFFFFFF00) > (STAM_ORGA_VERSION_I & 0xFFFFFF00)) {
        this->m_versionInfo = QString("Deine Version: %2<br>Aktuelle Version: %1<br><br>").arg(this->m_remoteVersion, STAM_ORGA_VERSION_S);
        this->m_versionInfo.append(QString(STAM_ORGA_VERSION_LINK_WITH_TEXT).arg(this->m_remoteVersion.toLower(), this->m_remoteVersion));
        this->m_updateLink = QString(STAM_ORGA_VERSION_LINK).arg(this->m_remoteVersion.toLower());

        if (QSslSocket::supportsSsl())
            this->m_versionUpdateIndex = VersionStatusNewVersion;
        else
            this->m_versionUpdateIndex = VersionStatusNoSSL;
        emit this->versionUpdateIndexChanged();

        return ERROR_CODE_NEW_VERSION;
    }

    if (result == ERROR_CODE_SUCCESS)
        this->m_versionUpdateIndex = VersionStatusNoNewVersion;
    else
        this->m_versionUpdateIndex = VersionStatusNoInfo;
    emit this->versionUpdateIndexChanged();

    return result;
}

qint32 cStaVersionManager::startDownloadCurrentVersion()
{
    emit g_ConNetworkAccess->signalStartDownload("https://github.com/WaterMax83/StamOrga/releases/download/v1.1.1/StamOrga.Winx64.v1.1.1.7z");

    this->m_versionUpdateIndex = VersionStatusBusy;
    emit this->versionUpdateIndexChanged();

    return ERROR_CODE_SUCCESS;
}

void cStaVersionManager::slotDownloadFinished(QString url, qint32 statusCode)
{
    emit this->signalVersionDownloadFinished();
}

bool cStaVersionManager::isVersionChangeAlreadyShown()
{
    if (this->m_lastShownVersion == STAM_ORGA_VERSION_S)
        return true;

    return false;
}

QString cStaVersionManager::getVersionChangeInfo()
{
    QString rValue;

    rValue.append("<b>V1.1.2:</b>(XX.XX.2018)<br>");
    rValue.append("- Update überarbeitet<br>");

    rValue.append("<br><b>V1.1.1:</b>(06.07.2018)<br>");
    rValue.append("- Benachrichtigung per Email (auf Wunsch)<br>");
    rValue.append("- Kommentare bei Treffen und Fahrt<br>");
    rValue.append("- lade vergangene Spiele dynamisch<br>");
    rValue.append("- Design Überarbeitung<br>");

    rValue.append("<br><b>V1.1.0:</b>(10.05.2018)<br>");
    rValue.append("- Umstellung UDP auf TCP<br>");
    rValue.append("- interne Struktur umgebaut<br>");
    rValue.append("- Statistik nach Jahren getrennt<br>");
    rValue.append("- diverse Fehler beseitigt<br>");

    rValue.append("<br><b>V1.0.7:</b>(03.03.2018)<br>");
    rValue.append("- Statistic hinzugefügt<br>");
    rValue.append("- Icons in Drawer und Übersicht überarbeitet<br>");
    rValue.append("- Easteregg versteckt<br>");
    rValue.append("- Infos über eigene Karte und Reservierung in der Übersicht<br>");

    rValue.append("<br><b>V1.0.6:</b>(08.02.2018)<br>");
    rValue.append("- Infos über Fahrt in der Übersicht<br>");
    rValue.append("- Icons überarbeitet<br>");
    rValue.append("- Spiel als Favorit markieren<br>");

    rValue.append("<br><b>V1.0.5:</b>(16.12.2017)<br>");
    rValue.append("- Infos über letzte Neuigkeiten markieren<br>");
    rValue.append("- Framework Version aktualisiert<br>");
    rValue.append("- Fahrt bei Auswärtsspiel hinzugefügt<br>");
    rValue.append("- Versionsupdate über Liste<br>");

    rValue.append("<br><b>V1.0.4:</b>(24.10.2017)<br>");
    rValue.append("- Fanclub Nachrichten (Mitglieder)<br>");
    rValue.append("- neue Benachrichtigung \"Erster Auswärtsfahrer\" & \"Fanclub Nachricht\"<br>");
    rValue.append("- Schrift änderbar (Android)<br>");
    rValue.append("- Verbindungsfehler beim Start behoben<br>");

    rValue.append("<br><b>V1.0.3:</b>(25.08.2017)<br>");
    rValue.append("- Push Notifications (Android)<br>");
    rValue.append("- Schrift änderbar (Windows)<br>");
    rValue.append("- Infos über Ort bei Spieltagstickets<br>");
    rValue.append("- Optische Anpassungen<br>");

    rValue.append("<br><b>V1.0.2:</b>(31.07.2017)<br>");
    rValue.append("- Spielterminierung hinzugefügt<br>");
    rValue.append("- Spielliste in Aktuell/Vergangenheit aufgeteilt<br>");
    rValue.append("- Daten nur nach Bedarf vom Server laden<br>");
    rValue.append("- Fehler beseitigt (Einstellungen/Tickets/etc..)<br>");

    rValue.append("<br><b>V1.0.1:</b>(17.07.2017)<br>");
    rValue.append("- Mehr Informationen in der Spielübersicht<br>");
    rValue.append("- automatisches Laden der Spielinformationen<br>");
    rValue.append("- Dauerkarten editierbar<br>");
    rValue.append("- Passwörter vollständig gehasht<br>");
    rValue.append("- Versionshistorie hinzugefügt<br>");
    rValue.append("- Diverse Fehler beseitigt<br>");

    rValue.append("<br><b>V1.0.0:</b>(30.06.2017)<br>");
    rValue.append("Erste Version<br>");

    if (this->m_lastShownVersion != STAM_ORGA_VERSION_S) {
        this->m_lastShownVersion = STAM_ORGA_VERSION_S;

        g_StaSettingsManager->setValue(SETTINGS_GROUP, SETT_LAST_SHOWN_VERSION, this->m_lastShownVersion);

        g_StaGlobalSettings->updatePushNotification();
    }

    return rValue;
}

QString cStaVersionManager::getRemoteVersion()
{
    return this->m_remoteVersion;
}
QString cStaVersionManager::getUpdateLink()
{
    return this->m_updateLink;
}
QString cStaVersionManager::getVersionInfo()
{
    return this->m_versionInfo;
}


QString cStaVersionManager::getCurrentVersion()
{
    return STAM_ORGA_VERSION_S;
}

QString cStaVersionManager::getCurrentVersionLink()
{
    return QString(STAM_ORGA_VERSION_COPY_WITH_TEXT).arg(QString(STAM_ORGA_VERSION_S).toLower(), STAM_ORGA_VERSION_S);
}
