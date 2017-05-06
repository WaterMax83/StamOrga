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

Flickable {
    id: flickableCurrentGame
    property UserInterface userIntCurrentGame
    property var m_gamePlayCurrentItem

    contentHeight: mainPaneCurrentGame.height

    Pane {
        id: mainPaneCurrentGame
        width: parent.width

        ColumnLayout {
            id: mainColumnLayoutCurrentGame
            width: parent.width
            MyComponents.Games {
                id: gameHeader
            }

            ColumnLayout {
                id: columnLayoutBusyInfoCurrGame
                spacing: 0
                Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter

                BusyIndicator {
                    id: busyLoadingIndicatorCurrentGames
                    visible: false
                    Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                }

                Label {
                    id: txtInfoCurrentGame
                    visible: true
                    Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                }
            }

            ToolSeparator {
                id: toolSeparator1
                orientation: "Horizontal"
                implicitWidth: mainPaneCurrentGame.width / 3 * 1
                Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
            }

            Label {
                id: txtInfoCurrentGameBlockedTickets
                visible: false
                text: "Nicht verfügbar:"
                Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
            }

            ListView {
                id: listViewBlockedTickets
                focus: false
                implicitWidth: mainColumnLayoutCurrentGame.width
                implicitHeight: flickableCurrentGame.height

                delegate: RowLayout {
                    width: parent.width
                    height: 30

                    MouseArea {
                        anchors.fill: parent
                        onClicked: {
                            console.log("Clicked element: " + model.index + " " + model.title);
                            clickedBlockedElement(model.index);
                        }
                    }
                    Rectangle {
                        id: imageItem
                        anchors.left: parent.left
                        anchors.leftMargin: parent.height
                        width: parent.height / 4 * 2
                        height: parent.height / 4 * 2
                        radius: width * 0.5
                        color: "red"
                    }

                    Text {
                        text: model.title
                        anchors.left: imageItem.right
                        anchors.leftMargin: 10
                        anchors.verticalCenter: parent.verticalCenter
                        Layout.alignment: Qt.AlignVCenter
                        color: "white"
                        font.pixelSize: parent.height / 4 * 2
                    }
                }

                model: ListModel {
                    id: listViewModelBlockedTickets
                }
            }
        }


    }

    function clickedBlockedElement(index) {
        menuItemFree.text = "Freigeben";
        clickedBlockedMenu.open();
    }

    Menu {
            id: clickedBlockedMenu
            x: (flickableCurrentGame.width - width) / 2
            y: flickableCurrentGame.height / 6
            title: "Test"


            background: Rectangle {
                    implicitWidth: menuItemFree.width
                    color: "#303030"
                }

            MenuItem {
                id: menuItemFree
                onClicked: {
                    console.log("Freigeben");
                }
            }
        }

    function showAllInfoAboutGame(sender) {

        busyLoadingIndicatorCurrentGames.visible = true
        txtInfoCurrentGame.text = "Lade Daten"
        userIntCurrentGame.startGettingSeasonTicketList()

        m_gamePlayCurrentItem = sender
        gameHeader.showGamesInfo(sender)
    }

    function pageOpenedUpdateView() {}

    function notifyUserIntSeasonTicketListFinished(result) {
        busyLoadingIndicatorCurrentGames.visible = false
        if (result === 1) {
            listViewModelBlockedTickets.clear()

            if (globalUserData.getSeasonTicketLength() > 0) {
                txtInfoCurrentGameBlockedTickets.visible = true
                for (var i = 0; i < globalUserData.getSeasonTicketLength(); i++) {
                    var discount = globalUserData.getSeasonTicket(i).discount > 0 ? " *" : ""
                    listViewModelBlockedTickets.append({
                                                           title: globalUserData.getSeasonTicket(i).name + discount,
                                                           index: i
                                                       })
                }
            }
            txtInfoCurrentGame.text = "Letzes Update am "
                    + globalUserData.getSeasonTicketLastUpdate()
        } else {
            txtInfoCurrentGame.text = userIntCurrentGame.getErrorCodeToString(
                        result)
        }
    }

    function notifyUserIntConnectionFinished(result) {}
}
