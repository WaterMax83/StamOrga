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

//import "../components" as MyComponents

Flickable {
    id: flickableCurrentTicketInfo
//    interactive: true
//    clip: true
    height: parent.height * 1.2

    property int listViewItemHeight : 30

    contentHeight: mainPaneCurrentTicketInfo.height

    onDragEnded: {
        if (flickableCurrentTicketInfo.contentY < -100) {
            loadAvailableTicketList();
        }
    }

    Rectangle {
        Image {
            id: refreshImage
            source: "../images/refresh.png"
            rotation: (flickableCurrentTicketInfo.contentY > -100) ? (flickableCurrentTicketInfo.contentY * -1) * 2 : 220
            transformOrigin: Item.Center
        }
        opacity: (flickableCurrentTicketInfo.contentY * -1) / 100
        color: "black"
        width: refreshImage.width
        height: refreshImage.height
        radius: width * 0.5
        y: 50
        x: (mainWindow.width / 2) - (width / 2)
        z: 1000
    }

    Pane {
        id: mainPaneCurrentTicketInfo
        width: parent.width

        ColumnLayout {
            id: mainColumnLayoutCurrentTicketInfo
            width: parent.width

            Label {
                id: txtInfoCurrentGameBlockedTickets
                visible: false
                text: "<b>Gesperrte Karten</b> <i>(Besitzer geht selbst)</i><b>:</b>"
                Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
            }

            ListView {
                id: listViewBlockedTickets
                focus: false
                interactive: false
                implicitWidth: mainColumnLayoutCurrentGame.width

                delegate: RowLayout {
                    id: singleBlockedRow
                    width: parent.width
                    height: listViewItemHeight

                    MouseArea {
                        anchors.fill: parent
                        onClicked: {
                            var globalCoordinates = singleBlockedRow.mapToItem(flickableCurrentTicketInfo, 0, 0)
                            clickedBlockedMenu.y = globalCoordinates.y - singleBlockedRow.height / 2
                            menuTicketIndex = model.index
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
                id: txtInfoCurrentGameReservedTickets
                visible: false
                text: "<b>Reservierte Karten:</b>"
                Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
            }

            ListView {
                id: listViewReservedTickets
                focus: false
                interactive: false
                implicitWidth: mainColumnLayoutCurrentGame.width

                delegate: ColumnLayout {
                    id: singleReservedRow
                    width: parent.width
                    spacing: 0

                    MouseArea {
                        anchors.fill: parent
                        onClicked: {
                            var globalCoordinates = singleReservedRow.mapToItem(flickableCurrentTicketInfo, 0, 0)
                            clickedReservedMenu.y = globalCoordinates.y - singleReservedRow.height / 2
                            menuTicketIndex = model.index
                            clickedReservedMenu.open();
                        }
                    }

                    RowLayout {
                        width: parent.width
                        height: listViewItemHeight
                        anchors.left: parent.left
                        anchors.leftMargin: listViewItemHeight


                        Rectangle {
                            id: imageItemReserved
                            anchors.left: parent.left
                            width: parent.height / 4 * 2
                            height: parent.height / 4 * 2
                            radius: width * 0.5
                            color: "yellow"
                        }

                        Text {
                            text: model.title
                            anchors.left: imageItemReserved.right
                            anchors.leftMargin: 10
                            anchors.verticalCenter: parent.verticalCenter
                            Layout.alignment: Qt.AlignVCenter
                            color: "white"
                            font.pixelSize: listViewItemHeight / 4 * 2
                        }
                    }

                    Text {
                        text: "-> für " + model.reserve
                        anchors.left: parent.left
                        anchors.leftMargin: listViewItemHeight + imageItemReserved.width + 10
                        bottomPadding: 5
                        Layout.alignment: Qt.AlignVCenter
                        color: "white"
                        font.pixelSize: listViewItemHeight / 4 * 2
                    }
                }

                model: ListModel {
                    id: listViewModelReservedTickets
                }
            }


            Label {
                id: txtInfoCurrentGameFreeTickets
                visible: false
                text: "<b>Freie Karten:</b>"
                Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
            }

            ListView {
                id: listViewFreeTickets
                focus: false
                interactive: false
                implicitWidth: mainColumnLayoutCurrentGame.width

                delegate: RowLayout {
                    id: singleFreeRow
                    width: parent.width
                    height: listViewItemHeight

                    MouseArea {
                        anchors.fill: parent
                        onClicked: {
                            var globalCoordinates = singleFreeRow.mapToItem(flickableCurrentTicketInfo, 0, 0)
                            clickedFreeMenu.y = globalCoordinates.y - singleFreeRow.height / 2
                            menuTicketIndex = model.index
                            clickedFreeMenu.open();
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

    ScrollIndicator.vertical: ScrollIndicator {
    }

    property int menuTicketIndex

    Menu {
            id: clickedBlockedMenu
            x: (flickableCurrentTicketInfo.width - width) / 2
            y: flickableCurrentTicketInfo.height / 6



            background: Rectangle {
                    implicitWidth: menuItemFree.width
                    color: "#4f4f4f"
                }

            MenuItem {
                id: menuItemFree
                text : "Freigeben"
                onClicked: {
                    userIntCurrentGame.startChangeAvailableTicketState(menuTicketIndex, m_gamePlayCurrentItem.index, 2);
                    busyLoadingIndicatorCurrentGames.visible = true
                    txtInfoCurrentGame.text = "Gebe Karte frei"
                }
            }
        }

    Menu {
            id: clickedReservedMenu
            x: (flickableCurrentTicketInfo.width - width) / 2
            y: flickableCurrentTicketInfo.height / 6

            background: Rectangle {
                    implicitWidth: menuItemBlockFromReserve.width
                    color: "#4f4f4f"
                }

            MenuItem {
                id: menuItemFreeFromReserve
                text: "Freigeben"
                onClicked: {
                    userIntCurrentGame.startChangeAvailableTicketState(menuTicketIndex, m_gamePlayCurrentItem.index, 2);
                    busyLoadingIndicatorCurrentGames.visible = true
                    txtInfoCurrentGame.text = "Gebe Karte frei"
                }
            }

            MenuItem {
                id: menuItemBlockFromReserve
                text: "Sperren"
                onClicked: {
                    userIntCurrentGame.startChangeAvailableTicketState(menuTicketIndex, m_gamePlayCurrentItem.index, 1);
                    busyLoadingIndicatorCurrentGames.visible = true
                    txtInfoCurrentGame.text = "Sperre Karte"
                }
            }

            MenuItem {
                id: menuItemChangeReserve
                text: "Reservierung ändern"
                onClicked: {

                    var component = Qt.createComponent("../components/EditableTextDialog.qml");
                    if (component.status === Component.Ready) {
                        var dialog = component.createObject(mainPaneCurrentGame,{popupType: 1});
                        dialog.headerText = "Reservierung ändern";
                        dialog.parentHeight = mainWindow.height
                        dialog.parentWidth =  flickableCurrentTicketInfo.width
                        dialog.textMinSize = 5;
                        dialog.acceptedTextEdit.connect(acceptedEditReserveNameDialog);
                        dialog.open();
                    }
                }
            }
        }

    Menu {
            id: clickedFreeMenu
            x: (flickableCurrentTicketInfo.width - width) / 2
            y: flickableCurrentTicketInfo.height / 6

            background: Rectangle {
                    implicitWidth: menuItemBlock.width
                    color: "#4f4f4f"
                }

            MenuItem {
                id: menuItemReserve
                text: "Reservieren"
                onClicked: {


                    var component = Qt.createComponent("../components/EditableTextDialog.qml");
                    if (component.status === Component.Ready) {
                        var dialog = component.createObject(mainPaneCurrentGame,{popupType: 1});
                        dialog.headerText = "Reserviere für";
                        dialog.parentHeight = mainWindow.height
                        dialog.parentWidth =  flickableCurrentTicketInfo.width
                        dialog.textMinSize = 5;
                        dialog.acceptedTextEdit.connect(acceptedEditReserveNameDialog);
                        dialog.open();
                    }
                }
            }

            MenuItem {
                id: menuItemBlock
                text: "Sperren"
                onClicked: {
                    userIntCurrentGame.startChangeAvailableTicketState(menuTicketIndex, m_gamePlayCurrentItem.index, 1);
                    busyLoadingIndicatorCurrentGames.visible = true
                    txtInfoCurrentGame.text = "Sperre Karte"
                }
            }
        }

    function acceptedEditReserveNameDialog(text)
    {
        userIntCurrentGame.startChangeAvailableTicketState(menuTicketIndex, m_gamePlayCurrentItem.index, 3, text);
        busyLoadingIndicatorCurrentGames.visible = true
        txtInfoCurrentGame.text = "Reserviere Karte"
    }

    function showAllInfoAboutGame(sender) {

        loadAvailableTicketList()
    }

    function notifyUserIntSeasonTicketListFinished(result) {

        if (result === 1) {
            loadAvailableTicketList()
        } else {
            txtInfoCurrentGame.text = userIntCurrentGame.getErrorCodeToString(result)
            toastManager.show(userIntCurrentGame.getErrorCodeToString(result), 4000);
            busyLoadingIndicatorCurrentGames.visible = false
        }
    }

    function notifyAvailableTicketStateChangedFinished(result) {
        if (result === 1) {
            toastManager.show("Status erfolgreich geändert", 2000);
            loadAvailableTicketList()
        } else {
            toastManager.show(userIntCurrentGame.getErrorCodeToString(result), 4000);
            busyLoadingIndicatorCurrentGames.visible = false;
        }
    }

    function loadAvailableTicketList() {
        listViewModelBlockedTickets.clear()
        listViewModelReservedTickets.clear()
        listViewModelFreeTickets.clear()
        listViewBlockedTickets.implicitHeight = 0;
        listViewReservedTickets.implicitHeight = 0;
        listViewFreeTickets.implicitHeight = 0;
        busyLoadingIndicatorCurrentGames.visible = true
        txtInfoCurrentGame.visible = true;
        txtInfoCurrentGame.text = "Aktualisiere Daten"
        userIntCurrentGame.startRequestAvailableTickets(m_gamePlayCurrentItem.index);
    }

    function notifyAvailableTicketListFinished(result) {
        busyLoadingIndicatorCurrentGames.visible = false;
        if (result === 1) {
            toastManager.show("Karten geladen", 2000);
        } else {
            toastManager.show(userIntCurrentGame.getErrorCodeToString(result), 4000);
            txtInfoCurrentGame.text = "Karten konnten nicht geladen werden"
        }
        showInternalTicketList(result);
    }

    function showInternalTicketList(result) {
        listViewModelBlockedTickets.clear()
        listViewModelReservedTickets.clear()
        listViewModelFreeTickets.clear()

        if (result === 1 && globalUserData.getSeasonTicketLength() > 0) {
            for (var i = 0; i < globalUserData.getSeasonTicketLength(); i++) {
                var seasonTicketItem = globalUserData.getSeasonTicketFromArrayIndex(i)
                var discount = seasonTicketItem.discount > 0 ? " *" : "";
                if (seasonTicketItem.getTicketState() === 2) {
                    listViewModelFreeTickets.append({
                                                        title: seasonTicketItem.name + discount,
                                                        index: seasonTicketItem.index
                                                    });
                }
                else if (seasonTicketItem.getTicketState() === 3) {
                    listViewModelReservedTickets.append({
                                                        title: seasonTicketItem.name + discount,
                                                        reserve: seasonTicketItem.getTicketReserveName(),
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
            listViewReservedTickets.implicitHeight = listViewModelReservedTickets.count * listViewItemHeight * ( 5 / 4)
            listViewFreeTickets.implicitHeight = listViewModelFreeTickets.count * listViewItemHeight

//            if (listViewModelFreeTickets.count > 0)
//                txtInfoCurrentGameFreeTickets.visible = true
//            else
//                txtInfoCurrentGameFreeTickets.visible = false
//            if (listViewModelReservedTickets.count > 0)
//                txtInfoCurrentGameReservedTickets.visible = true
//            else
//                txtInfoCurrentGameReservedTickets.visible = false
//            if (listViewModelBlockedTickets.count > 0)
//                txtInfoCurrentGameBlockedTickets.visible = true
//            else
//                txtInfoCurrentGameBlockedTickets.visible = false

            txtInfoCurrentGameBlockedTickets.visible = true
            txtInfoCurrentGameReservedTickets.visible = true
            txtInfoCurrentGameFreeTickets.visible = true

            txtInfoCurrentGame.visible = false;

        } else {
            txtInfoCurrentGameBlockedTickets.visible = false
            txtInfoCurrentGameReservedTickets.visible = false
            txtInfoCurrentGameFreeTickets.visible = false
            listViewBlockedTickets.implicitHeight = 0;
            listViewReservedTickets.implicitHeight = 0;
            listViewFreeTickets.implicitHeight = 0;

            txtInfoCurrentGame.visible = true;
        }


//        txtInfoCurrentGame.text = "Letztes Update am "
//                + globalUserData.getSeasonTicketLastUpdateString()
    }
}
