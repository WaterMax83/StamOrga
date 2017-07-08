
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

import "pages" as MyPages
import "components" as MyComponents

ApplicationWindow {
    id: mainWindow
    visible: true
    width: userInt.isDeviceMobile() ? 540 : 360
    height: userInt.isDeviceMobile() ? 960 : 600
    title: qsTr("StamOrga")

    onClosing: {
        if (userInt.isDeviceMobile() && stackView.depth > 1) {
            close.accepted = false
            stackView.pop()
        } else {
            return
        }
    }

    Shortcut {
        sequence: "Esc"
        enabled: stackView.depth > 1
        onActivated: {
            stackView.pop()
        }
    }

    header: ToolBar {
        height: 50

        RowLayout {
            spacing: 20
            anchors.fill: parent

            ToolButton {
                contentItem: Image {
                    fillMode: Image.Pad
                    horizontalAlignment: Image.AlignHCenter
                    verticalAlignment: Image.AlignVCenter
                    source: stackView.depth > 1 ? "images/back.png" : "images/drawer.png"
                }
                onClicked: {
                    if (stackView.depth > 1) {
                        stackView.pop()
                    } else {
                        drawer.open()
                    }
                }
            }

            Label {
                id: titleLabel
                //                text: listView.currentItem ? listView.currentItem.text : "StamOrga"
                font.pixelSize: 25
                elide: Label.ElideRight
                horizontalAlignment: Qt.AlignHCenter
                verticalAlignment: Qt.AlignVCenter
                Layout.fillWidth: true
            }

            ToolButton {
                contentItem: Image {
                    id: imageToolButton
                    fillMode: Image.Pad
                    horizontalAlignment: Image.AlignHCenter
                    verticalAlignment: Image.AlignVCenter
//                    source: "images/refresh.png"
                    //                    visible: stackView.depth > 1 ? false : true
                }
                onClicked: {
                    if (imageToolButton.visible === true) {
                        stackView.currentItem.toolButtonClicked()
                    }
                }
            }
        }
    }

    Drawer {
        id: drawer
        width: Math.min(mainWindow.width, mainWindow.height) / 4 * 2
        height: mainWindow.height

        ColumnLayout {
            implicitWidth: drawer.width
            spacing: 1
            anchors.fill: parent

            Rectangle {
                implicitWidth: drawer.width
                implicitHeight: drawer.width

                color: "white"
                Image {
                    height: parent.height
                    width: parent.width
                    fillMode: Image.Stretch
                    horizontalAlignment: Image.AlignHCenter
                    verticalAlignment: Image.AlignVCenter
                    source: "images/StammtischLogo.png"
                }
            }

            ListView {
                id: listView

                focus: true
                currentIndex: -1
                implicitWidth: drawer.width
                implicitHeight: drawer.height

                delegate: ItemDelegate {
                    width: parent.width
                    text: model.title
                    highlighted: ListView.isCurrentItem
                    onClicked: {
                        stackView.push(model.element)
                        titleLabel.text = model.title
                        if (model.imgsource !== "") {
                            imageToolButton.visible = true
                            imageToolButton.source = model.imgsource
                        } else {
                            imageToolButton.visible = false
                        }

                        drawer.close()
                    }
                }

                model: ListModel {
                    id: listViewListModel
                    Component.onCompleted: {
                        append({
                                   title: "Benutzerprofil",
                                   element: viewUserLogin,
                                   imgsource: "images/menu.png"
                               })
                        append({
                                   title: "Dauerkarten",
                                   element: viewSeasonTickets,
                                   imgsource: "images/add.png"
                               })
                        append({
                                   title: "Einstellungen",
                                   element: viewSettingsPage,
                                   imgsource: ""
                               })
                        if (userInt.isDebuggingEnabled())
                            append({
                                       title: "Logging",
                                       element: viewLoggingPage,
                                       imgsource: ""
                                   })
                    }
                }

                ScrollIndicator.vertical: ScrollIndicator {}
            }
        }
    }

    function updateHeaderFromMain(text, img) {
        if (img !== "") {
            imageToolButton.visible = true
            imageToolButton.source = img
        } else {
            imageToolButton.visible = false
        }

        if (text !== "")
            titleLabel.text = text
    }

    StackView {
        id: stackView
        anchors.fill: parent

        initialItem: viewGames

        onCurrentItemChanged: {
            stackView.currentItem.pageOpenedUpdateView()
        }
    }

    Component {
        id: viewGames
        MyPages.GamesPage {
            userIntGames: userInt
        }
    }

    MyComponents.ToastManager {
        id: toastManager
    }

    Component {
        id: viewUserLogin
        MyPages.UserLogin {
            userIntUser: userInt
        }
    }

    Component {
        id: viewSeasonTickets
        MyPages.SeasonTickets {
            userIntTicket: userInt
        }
    }

    Component {
        id: viewSettingsPage
        MyPages.Settings {
        }
    }

    Component {
        id: viewLoggingPage
        MyPages.LogginPage {
        }
    }

    UserInterface {
        id: userInt
        globalData: globalUserData
        onNotifyConnectionFinished: {
            stackView.currentItem.notifyUserIntConnectionFinished(result)
        }
        onNotifyVersionRequestFinished: {
            //            stackView.currentItem.notifyUserIntVersionRequestFinished(result, msg);
            if (result === 5) {
                var component = Qt.createComponent(
                            "/components/VersionDialog.qml")
                if (component.status === Component.Ready) {
                    var dialog = component.createObject(stackView, {
                                                            popupType: 1
                                                        })
                    dialog.versionText = msg
                    dialog.parentHeight = stackView.height
                    dialog.parentWidth = stackView.width
                    dialog.open()
                }
            }
        }
        property bool isDebugWindowEnabled : false;
        onNotifyUserPropertiesFinished: {
            if (result > 0 && !userInt.isDebuggingEnabled() && !isDebugWindowEnabled) {
                isDebugWindowEnabled = true;
                if (globalUserData.userIsDebugEnabled()) {
                    listViewListModel.append({
                                            title: "Logging",
                                            element: viewLoggingPage,
                                            imgsource: ""
                                        })
                }
            }
            if (globalUserData.userIsGameAddingEnabled() || userInt.isDebuggingEnabled())
                updateHeaderFromMain("StamOrga", "images/add.png")
            else
                updateHeaderFromMain("StamOrga", "")
        }

        onNotifyUpdatePasswordRequestFinished: {
            stackView.currentItem.notifyUserIntUpdatePasswordFinished(result)
        }
        onNotifyUpdateReadableNameRequest: {
            stackView.currentItem.notifyUserIntUpdateReadableNameFinished(
                        result)
        }
        onNotifyGamesListFinished: {
            stackView.currentItem.notifyUserIntGamesListFinished(result)
        }
        onNotifyGamesInfoListFinished: {
            stackView.currentItem.notifyUserIntGamesInfoListFinished(result)
        }
        onNotifySeasonTicketAddFinished: {
            stackView.currentItem.notifyUserIntSeasonTicketAdd(result)
        }
        onNotifySeasonTicketListFinished: {
            stackView.currentItem.notifyUserIntSeasonTicketListFinished(result)
        }
        onNotifySeasonTicketRemoveFinished: {
            stackView.currentItem.notifyUserIntSeasonTicketRemoveFinished(result)
        }
        onNotifySeasonTicketNewPlaceFinished: {
            stackView.currentItem.notifyUserIntSeasonTicketNewPlaceFinished(result)
        }

        onNotifyAvailableTicketStateChangedFinished: {
            stackView.currentItem.notifyAvailableTicketStateChangedFinished(result);
        }
        onNotifyAvailableTicketListFinsished: {
            stackView.currentItem.notifyAvailableTicketListFinished(result);
        }
        onNotifyChangedGameFinished: {
            stackView.currentItem.notifyGameChangedFinished(result);
        }
        onNotifyChangedMeetingInfoFinished: {
            stackView.currentItem.notifyChangedMeetingInfoFinished(result);
        }
        onNotifyLoadMeetingInfoFinished: {
            stackView.currentItem.notifyLoadMeetingInfoFinished(result);
        }
        onNotifyAcceptMeetingFinished: {
            stackView.currentItem.notifyAcceptMeetingFinished(result);
        }
    }

    function openUserLogin(open) {

        if (open === true) {
            stackView.push(viewUserLogin)
            imageToolButton.visible = false
        } else {
            stackView.currentItem.showListedGames()

            if (!globalSettings.isVersionChangeAlreadyShown()) {
                var component = Qt.createComponent("../pages/newVersionInfo.qml");
                if (component.status === Component.Ready) {
                    stackView.push(component);
                }
            }
        }
    }
}
