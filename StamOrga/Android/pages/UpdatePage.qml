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

import QtQuick 2.9
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.2

import com.watermax.demo 1.0

import "../components" as MyComponents
import "../controls" as MyControls

Flickable {
    id: flickableUpdate

    contentHeight: mainPaneUpdate.height

    boundsBehavior: Flickable.StopAtBounds

    Pane {
        id: mainPaneUpdate
        width: parent.width

        ColumnLayout {
            width: parent.width
            spacing: 20

            Text {
                id: txtForFontFamily
                visible: false
            }

            Text {
                id: labelVersionText
                textFormat: Text.RichText
                wrapMode: Text.WordWrap
                Layout.maximumWidth: parent.width
                Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
                Layout.topMargin: 20
                font.pixelSize: 14
                color: "white"
                text: "Deine Version: <a href=\"tmp\">" + gStaVersionManager.getCurrentVersion() + "</a>"
                onLinkActivated: {
                        stackView.push(viewSettingsVersionInfoPage);
                }
            }

            ToolSeparator {
                orientation: "Horizontal"
                implicitWidth: parent.width / 3 * 2
                Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                //                Layout.bottomMargin: 35
            }

            MyControls.Button {
                id: btnChangeCheckVersion
                font.family: txtForFontFamily.font
                text: qsTr("Version prüfen")
                implicitWidth: parent.width / 4 * 2
                Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                transformOrigin: Item.Center
                visible: gStaVersionManager.versionUpdateIndex === 0
                onClicked: {
                    busyIndicatorUpdate.loadingVisible = true;
                    busyIndicatorUpdate.infoVisible = true;
                    busyIndicatorUpdate.infoText = "Prüfe Version";
                    gStaVersionManager.startGettingVersionInfo();
                }
            }

            Text {
                wrapMode: Text.WordWrap
                Layout.maximumWidth: parent.width
                Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                font.pixelSize: 14
                visible: gStaVersionManager.versionUpdateIndex === 1
                color: "white"
                text: "Du hast bereits die aktuellste Version"
            }

            Text {
                wrapMode: Text.WordWrap
                Layout.maximumWidth: parent.width
                Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
                font.pixelSize: 14
                visible: gStaVersionManager.versionUpdateIndex === 2 || gStaVersionManager.versionUpdateIndex === 4
                color: "white"
                text: "Aktuelle Version: " + gStaVersionManager.remoteVersion;
            }

            MyControls.Button {
                id: btnDownloadCurrentVersion
                font.family: txtForFontFamily.font
                text: "Lade " + gStaVersionManager.remoteVersion;
                implicitWidth: parent.width / 4 * 2
                Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                transformOrigin: Item.Center
                visible: gStaVersionManager.versionUpdateIndex === 2
                onClicked: {
                    busyIndicatorUpdate.loadingVisible = true;
                    busyIndicatorUpdate.infoVisible = true;
                    busyIndicatorUpdate.infoText = "Download der aktuellen Version " + gStaVersionManager.remoteVersion;
                    gStaVersionManager.startDownloadCurrentVersion();
                }
            }

            Text {
                wrapMode: Text.WordWrap
                Layout.maximumWidth: parent.width
                Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
                font.pixelSize: 14
                visible: gStaVersionManager.versionUpdateIndex === 4
                color: "white"
                text: "Oops leider ist kein SSL installiert um die Version zu laden!";
            }

            Text {
                id: labelLinkToCurrentVersion
                textFormat: Text.RichText
                wrapMode: Text.Wrap
                font.pixelSize: 14
                visible: gStaVersionManager.versionUpdateIndex === 4
                color: "white"
                text: "Lade sie extern: <a href=\"" + gStaVersionManager.getUpdateLink() + "\">" + gStaVersionManager.remoteVersion + "</a>"
                onLinkActivated:  {
                    Qt.openUrlExternally(link)
                }
            }



            Text {
                wrapMode: Text.WordWrap
                Layout.maximumWidth: parent.width
                Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
                font.pixelSize: 14
                visible: gStaVersionManager.versionUpdateIndex === 5 || gStaVersionManager.versionUpdateIndex === 6
                color: "white"
                text: "Neue Version " + gStaVersionManager.remoteVersion + " erfolgreich geladen.";
            }

            MyControls.Button {
                id: btnInstallCurrentVersion
                font.family: txtForFontFamily.font
                text: gStaVersionManager.versionUpdateIndex === 5 ? "Zielordner öffnen" : "Installiere " + gStaVersionManager.remoteVersion;
                implicitWidth: parent.width / 4 * 2
                Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                transformOrigin: Item.Center
                visible: gStaVersionManager.versionUpdateIndex === 5 || gStaVersionManager.versionUpdateIndex === 6
                onClicked: {
                    gStaVersionManager.startInstallCurrentVersion();
                }
            }

            Text {
                id: txtErrorString
                wrapMode: Text.WordWrap
                Layout.maximumWidth: parent.width
                Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
                font.pixelSize: 14
                visible: gStaVersionManager.versionUpdateIndex === 7
                color: "white"
            }

            MyComponents.BusyLoadingIndicator {
                id: busyIndicatorUpdate
                width: parent.width
                Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                Layout.fillHeight: false
                infoVisible: false
            }

            ProgressBar {
                width: parent.width
                Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                visible: gStaVersionManager.versionUpdateIndex === 3
                value: gStaVersionManager.currentProgress
            }
        }
    }

    Connections {
        target: gStaVersionManager
        onSignalVersionDownloadFinished: {
            busyIndicatorUpdate.loadingVisible = false;
            busyIndicatorUpdate.infoVisible = false;
            if (result === 1) {
                toastManager.show("Download erfolgreich", 2000)
            } else {
                toastManager.show(userInt.getErrorCodeToString(result), 5000)
                txtErrorString.text = "Fehler beim Laden von " + gStaVersionManager.getUpdateLink();
            }
        }
    }

    function pageOpenedUpdateView() {
        gDataAppUserEvents.clearUserEventUpdate();
    }

    function notifyVersionRequestFinished(result) {
        busyIndicatorUpdate.loadingVisible = false;
        busyIndicatorUpdate.infoVisible = false;

    }
}
