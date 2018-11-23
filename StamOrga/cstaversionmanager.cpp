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
#include <QtCore/QProcess>
#include <QtCore/QStandardPaths>
#include <QtGui/QDesktopServices>

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
    VersionStatusNoSSL        = 4,
    VersionStatusWinOpenFold  = 5,
    VersionStatusAndroidIns   = 6,
    VersionStatusErrorDownl   = 7
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

    qInfo().noquote() << "SSH Version: " << QSslSocket::sslLibraryVersionString();

    connect(g_ConNetworkAccess, &cConNetworkAccess::signalDownloadProgress, this, &cStaVersionManager::slotDownloadProgress);
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
    this->m_remoteVersion = rootObj.value("sVersion").toString().toLower();

    qInfo().noquote() << QString("Version from server %1:0x%2").arg(this->m_remoteVersion, QString::number(uVersion, 16));

    if ((uVersion & 0xFFFFFF00) > (STAM_ORGA_VERSION_I & 0xFFFFFF00)) {
        this->m_versionInfo = QString("Deine Version: %2<br>Aktuelle Version: %1<br><br>").arg(this->m_remoteVersion, STAM_ORGA_VERSION_S);
        this->m_versionInfo.append(QString(STAM_ORGA_VERSION_LINK_WITH_TEXT).arg(this->m_remoteVersion));
        this->m_updateLink = QString(STAM_ORGA_VERSION_LINK).arg(this->m_remoteVersion);

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
    this->m_downloadURL = this->m_updateLink;
    emit g_ConNetworkAccess->signalStartDownload(this->m_downloadURL);
    this->m_downloadSavePath = QStandardPaths::writableLocation(QStandardPaths::DownloadLocation);
    this->m_downloadSavePath.append(QString("/%1").arg(QString(STAM_ORGA_VERSION_SAVE).arg(this->m_remoteVersion)));

    this->m_progress = 0.0;
    emit this->currentProgressChanged();
    this->m_versionUpdateIndex = VersionStatusBusy;
    emit this->versionUpdateIndexChanged();

    return ERROR_CODE_SUCCESS;
}

void cStaVersionManager::slotDownloadProgress(qint64 current, qint64 max)
{
    this->m_progress = (double)current / max;
    emit this->currentProgressChanged();
}

void cStaVersionManager::slotDownloadFinished(QString url, qint32 statusCode)
{
    if (this->m_downloadURL == url) {
        if (statusCode == ERROR_CODE_SUCCESS) {

            QByteArray data;
            if (g_ConNetworkAccess->getDownload(url, data) == ERROR_CODE_SUCCESS) {
                QFile file(this->m_downloadSavePath);
                if (file.open(QIODevice::WriteOnly)) {
                    file.write(data);
                    file.flush();
                    file.close();
                    qInfo().noquote() << "Saved to " << this->m_downloadSavePath;
#ifdef Q_OS_WIN
                    this->m_versionUpdateIndex = VersionStatusWinOpenFold;
#elif defined(Q_OS_ANDROID)
                    this->m_versionUpdateIndex = VersionStatusAndroidIns;
#endif

                } else
                    statusCode = ERROR_CODE_NOT_POSSIBLE;
            } else
                statusCode = ERROR_CODE_NOT_FOUND;
        } else
            this->m_versionUpdateIndex = VersionStatusErrorDownl;

        emit this->versionUpdateIndexChanged();
        emit this->signalVersionDownloadFinished(statusCode);
    }
}

qint32 cStaVersionManager::startInstallCurrentVersion()
{
    QString filePath = QDir::toNativeSeparators(this->m_downloadSavePath);
#ifdef Q_OS_WIN
    QFileInfo fileInfo(filePath);
    QString   pathToOpen = QString("explorer \"%1\"").arg(QDir::toNativeSeparators(fileInfo.absolutePath()));
    QProcess::startDetached(pathToOpen);
#elif defined Q_OS_ANDROID
    QDesktopServices::openUrl(QUrl(filePath));
#endif

    return ERROR_CODE_SUCCESS;
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
    rValue.append("- Überarbeitung Benachrichtigungen<br>");
    rValue.append("- Zeiten besser darstellen<br>");
    rValue.append("- Update Android SDK API 28<br>");
    rValue.append("- Bautagebuch<br>");

    rValue.append("<br><b>V1.1.2:</b>(08.09.2018)<br>");
    rValue.append("- Installiere neue Version intern<br>");
    rValue.append("- Verbindung über SSL möglich (-> siehe Einstellungen)<br>");
    rValue.append("- Ticketänderungszeit, mehrere Benachrichtungen<br>");
    rValue.append("- Weitere Events in Spielliste möglich<br>");

    rValue.append("<br><b>V1.1.1:</b>(06.07.2018)<br>");
    rValue.append("- Benachrichtigung per Email (auf Wunsch)<br>");
    rValue.append("- Kommentare bei Treffen und Fahrt<br>");
    rValue.append("- Lade vergangene Spiele dynamisch<br>");
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
    return QString(STAM_ORGA_VERSION_S).toLower();
}

QString cStaVersionManager::getCurrentVersionLink()
{
    return QString(STAM_ORGA_VERSION_COPY_WITH_TEXT).arg(QString(STAM_ORGA_VERSION_S).toLower(), STAM_ORGA_VERSION_S);
}
