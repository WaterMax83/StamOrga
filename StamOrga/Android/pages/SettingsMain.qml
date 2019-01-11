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

import QtQuick.Controls.Material 2.2

import com.watermax.demo 1.0

import "../pages" as MyPages

Item {
    id: mainItemSettings
//    height: parent.height

   Pane {
        id: mainPaneSettings
        width: parent.width
        height: parent.height

        ListView {
            id: listViewSettings

            focus: true
            currentIndex: -1
            implicitWidth: parent.width
            implicitHeight: parent.height
            clip: true
            snapMode: ListView.SnapToItem

            boundsBehavior: Flickable.StopAtBounds

            delegate: listDelegateSettings

            model: ListModel {
                id: listViewListModelSettings
                Component.onCompleted: {
                    append({
                               title: "BenutzerProfil",
                               element: viewUserLogin,
                               showSeperator: true,
                               listImgSource : "/images/account.png"
                           })
                    append({
                               title: "Allgemein",
                               element: viewSettingsGeneral,
                               showSeperator: true,
                               listImgSource : "/images/settings.png"
                           })
                    if (userInt.isDeviceMobile() || userInt.isDebuggingEnabled()){
                        append({
                                   title: "Benachrichtigungen",
                                   element: viewSettingsNotify,
                                   showSeperator: true,
                                   listImgSource : "/images/notification.png"
                               })
                    }
                    append({
                               title: "Version",
                               element: viewSettingsVersionInfoPage,
                               showSeperator: false,
                               listImgSource : "/images/info.png"
                           })
                }
            }

            ScrollIndicator.vertical: ScrollIndicator {}
        }
    }

    Component {
        id: listDelegateSettings
        Item {
            id: delegateItemSettings
            width: listViewSettings.width
            height: 60

                GridLayout {
                    anchors.fill: parent
                    columns: 2
                    rows: 2
                    Image {
                        Layout.column: 0
                        Layout.row: 0
                        Layout.preferredHeight: parent.height / 1.5
                        Layout.preferredWidth: parent.height / 1.5
                        Layout.leftMargin: 10
                        horizontalAlignment: Qt.AlignHCenter
                        verticalAlignment: Qt.AlignVCenter
                        source: listImgSource
                    }

                    Text {
                        Layout.column: 1
                        Layout.row: 0
                        height: parent.height
                        text: title
                        color: "white"
                        font.pixelSize: 16
                        elide: Text.ElideRight
                        verticalAlignment: Qt.AlignVCenter
                        Layout.leftMargin: 10
                        Layout.fillWidth: true
                    }
                    Rectangle {
                        Layout.column: 0
                        Layout.row: 1
                        Layout.columnSpan: 2
                        color: Material.hintTextColor
                        height: 1
                        visible: showSeperator
                        Layout.fillWidth: true
                    }
                }

            MouseArea {
                anchors.fill: parent
                hoverEnabled: true
                onClicked: {
                    listViewSettings.currentIndex = index
                    if (model.element) {
                        titleLabel.text = title
                        stackView.push(element)
                    }
                }
            }
        }
    }

    SettingsGeneral {
        id: viewSettingsGeneral
        visible: false
    }

    SettingsNotification {
        id: viewSettingsNotify
        visible: false
    }


    function toolButtonClicked() {
    }

    function pageOpenedUpdateView() {

        updateHeaderFromMain("Einstellungen", "");
    }

    function notifyUserIntConnectionFinished(result) {}
}
