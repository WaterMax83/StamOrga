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

import QtQuick 2.7
import QtQuick.Controls 2.1
import QtQuick.Layouts 1.0

import com.watermax.demo 1.0

import "../components" as MyComponents

Pane {
//    id: mainPaneCurrentNews
    width: parent.width
    height: parent.height
    property UserInterface userIntCurrentNews

    ColumnLayout {
        id: mainColumnLayoutCurrentNews
        width: parent.width
        height: parent.height

        MyComponents.BusyLoadingIndicator {
            id: busyIndicatorNews
            width: parent.width
        }

        MyComponents.EditableTextWithHint {
            id: textHeader
            hint: "Überschrift"
            imageSource: ""
            width: parent.width
            enabled: true
        }

        ToolSeparator {
            id: toolSeparator1
            orientation: "Horizontal"
            Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
            Layout.fillWidth: true
        }

        Flickable {
            width: parent.width
            anchors.top: toolSeparator1.bottom
            anchors.bottom: mainColumnLayoutCurrentNews.bottom
            flickableDirection: Flickable.VerticalFlick

            TextArea.flickable: TextArea {
                id: textAreaInfo
                width: parent.width
                placeholderText: qsTr("Enter description")
                color: "#505050"
                background: Rectangle {
                    implicitWidth: parent.width
                    implicitHeight: 400
                }
                leftPadding: 5
                rightPadding: 5
                font.pixelSize: 16
                enabled: true
                wrapMode: TextEdit.WordWrap
            }

            ScrollBar.vertical: ScrollBar {
                id: currentScrollBar
                contentItem: Rectangle {
                        implicitWidth: 6
                        implicitHeight: 100
                        radius: width / 2
                        color: currentScrollBar.pressed ? "#0080FF" : "#58ACFA"
                    }
            }
        }

    }


    function pageOpenedUpdateView() {

    }

    function toolButtonClicked() {
        userIntCurrentNews.startChangeFanclubNews(0, textHeader.input, textAreaInfo.text);
        busyIndicatorNews.loadingVisible = true;
        busyIndicatorNews.infoVisible = true;
        busyIndicatorNews.infoText = "Speichere News"
    }

    function startEditMode() {
        updateHeaderFromMain("Neue News", "images/save.png")
    }

    function notifyChangeNewsDataFinished(result) {

        if (result === 1) {
            toastManager.show("News erfolgreich gespeichert", 2000);
            busyIndicatorNews.loadingVisible = false;
            busyIndicatorNews.infoVisible = false;
        } else {
            toastManager.show(userIntCurrentNews.getErrorCodeToString(result), 4000);
            busyIndicatorNews.loadingVisible = false;
            busyIndicatorNews.infoText = "News speichern hat nicht funktioniert"
        }
    }

    function notifyUserIntConnectionFinished(result) {}

}
