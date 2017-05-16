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

    property int listViewItemHeight : 30

    contentHeight: mainPaneCurrentGame.height

    Pane {
        id: mainPaneCurrentGame
        width: parent.width



        ColumnLayout {
            id: mainColumnLayoutCurrentGame
            width: parent.width
            MyComponents.Games {
                id: gameHeader
                width: parent.width
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
                text: "Karte geblockt:"
                Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
            }

            ListView {
                id: listViewBlockedTickets
                focus: false
                implicitWidth: mainColumnLayoutCurrentGame.width
//                implicitHeight: listViewModelBlockedTickets.count * listViewItemHeight

                delegate: RowLayout {
                    id: singleBlockedRow
                    width: parent.width
                    height: listViewItemHeight

                    MouseArea {
                        anchors.fill: parent
                        onClicked: {
                            var globalCoordinates = singleBlockedRow.mapToItem(flickableCurrentGame, 0, 0)
                            menuItemFree.text = "Freigeben";
                            clickedBlockedMenu.y = globalCoordinates.y - singleBlockedRow.height / 2
                            clickedBlockedMenu.menuTicketIndex = model.index
                            clickedBlockedMenu.open();
                        }
                    }
                    Rectangle {
                        id: imageItemBlocked
                        anchors.left: parent.left
                        anchors.leftMargin: parent.height
                        width: parent.height / 4 * 2
                        height: parent.height / 4 * 2
                        radius: width * 0.5
                        color: "red"
                    }

                    Text {
                        text: model.title
                        anchors.left: imageItemBlocked.right
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




            Label {
                id: txtInfoCurrentGameFreeTickets
                visible: false
                text: "Karte frei:"
                Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
            }

            ListView {
                id: listViewFreeTickets
                focus: false
                implicitWidth: mainColumnLayoutCurrentGame.width
//                implicitHeight: listViewModelFreeTickets * listViewItemHeight

                delegate: RowLayout {
                    id: singleFreeRow
                    width: parent.width
                    height: listViewItemHeight

                    MouseArea {
                        anchors.fill: parent
                        onClicked: {
//                            var globalCoordinates = singleBlockedRow.mapToItem(flickableCurrentGame, 0, 0)
//                            menuItemFree.text = "Freigeben";
//                            clickedBlockedMenu.y = globalCoordinates.y - singleBlockedRow.height / 2
//                            clickedBlockedMenu.menuTicketIndex = model.index
//                            clickedBlockedMenu.open();
                        }
                    }
                    Rectangle {
                        id: imageItemFree
                        anchors.left: parent.left
                        anchors.leftMargin: parent.height
                        width: parent.height / 4 * 2
                        height: parent.height / 4 * 2
                        radius: width * 0.5
                        color: "green"
                    }

                    Text {
                        text: model.title
                        anchors.left: imageItemFree.right
                        anchors.leftMargin: 10
                        anchors.verticalCenter: parent.verticalCenter
                        Layout.alignment: Qt.AlignVCenter
                        color: "white"
                        font.pixelSize: parent.height / 4 * 2
                    }
                }

                model: ListModel {
                    id: listViewModelFreeTickets
                }
            }
        }


    }

    Menu {
            id: clickedBlockedMenu
            x: (flickableCurrentGame.width - width) / 2
            y: flickableCurrentGame.height / 6
            property int menuTicketIndex



            background: Rectangle {
                    implicitWidth: menuItemFree.width
                    color: "#4f4f4f"
                }

            MenuItem {
                id: menuItemFree
                onClicked: {
                    console.log("Freigeben " + clickedBlockedMenu.menuTicketIndex + " " + m_gamePlayCurrentItem.index);
                    userIntCurrentGame.startFreeAvailableTicket(clickedBlockedMenu.menuTicketIndex, m_gamePlayCurrentItem.index)
                    busyLoadingIndicatorCurrentGames.visible = true
                    txtInfoCurrentGame.text = "Gebe Karte frei"
                }
            }
        }

    function showAllInfoAboutGame(sender) {

        busyLoadingIndicatorCurrentGames.visible = true
        txtInfoCurrentGame.text = "Aktualisiere Daten"
        userIntCurrentGame.startListSeasonTickets()

        m_gamePlayCurrentItem = sender
        gameHeader.showGamesInfo(sender)
    }

    function pageOpenedUpdateView() {}

    function notifyUserIntSeasonTicketListFinished(result) {
        busyLoadingIndicatorCurrentGames.visible = false
        if (result === 1) {
            userIntCurrentGame.startRequestAvailableTickets(m_gamePlayCurrentItem.index);
        } else {
            txtInfoCurrentGame.text = userIntCurrentGame.getErrorCodeToString(
                        result)
        }
    }

    function notifyAvailableTicketFreeFinished(result) {
        busyLoadingIndicatorCurrentGames.visible = false
        if (result === 1) {
            toastManager.show("Karte erfolgreich freigegeben", 2000);
            userIntCurrentGame.startRequestAvailableTickets(m_gamePlayCurrentItem.index);
        } else {
            toastManager.show(userIntCurrentGame.getErrorCodeToString(result), 4000);
        }
    }

    function notifyAvailableTicketListFinished(result) {
        if (result === 1) {
            toastManager.show("Karten geladen", 2000);
        } else {
            toastManager.show(userIntCurrentGame.getErrorCodeToString(result), 4000);
        }
        showInternalTicketList();
    }

    function notifyUserIntConnectionFinished(result) {}

    function showInternalTicketList() {
        listViewModelBlockedTickets.clear()
        listViewModelFreeTickets.clear()

        if (globalUserData.getSeasonTicketLength() > 0) {
            for (var i = 0; i < globalUserData.getSeasonTicketLength(); i++) {
                var seasonTicketItem = globalUserData.getSeasonTicketFromArrayIndex(i)
                var discount = seasonTicketItem.discount > 0 ? " *" : ""
                if (seasonTicketItem.isTicketFree()) {
                    listViewModelFreeTickets.append({
                                                        title: seasonTicketItem.name + discount,
                                                        index: seasonTicketItem.index
                                                    });
                }
                else
                    listViewModelBlockedTickets.append({
                                                       title: seasonTicketItem.name + discount,
                                                       index: seasonTicketItem.index
                                                   })
            }
            /* Does not work in defintion for freeTickets, so set it here */
            listViewBlockedTickets.implicitHeight = listViewModelBlockedTickets.count * listViewItemHeight
            listViewFreeTickets.implicitHeight = listViewModelFreeTickets.count * listViewItemHeight

            if (listViewModelFreeTickets.count > 0)
                txtInfoCurrentGameFreeTickets.visible = true
            else
                txtInfoCurrentGameFreeTickets.visible = false
            if (listViewModelBlockedTickets.count > 0)
                txtInfoCurrentGameBlockedTickets.visible = true
            else
                txtInfoCurrentGameBlockedTickets.visible = false

        } else {
            txtInfoCurrentGameBlockedTickets.visible = false
            txtInfoCurrentGameFreeTickets.visible = false
        }

        txtInfoCurrentGame.text = "Letztes Update am "
                + globalUserData.getSeasonTicketLastUpdate()
    }
}
