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
    id: flickableCurrentTicketInfo
    clip: true
    height: parent.height * 1.2
    contentHeight: mainPaneCurrentTicketInfo.height

    signal showInfoHeader(var text, var load)

    flickableDirection: Flickable.VerticalFlick
    rebound: Transition {
            NumberAnimation {
                properties: "y"
                duration: 1000
                easing.type: Easing.OutBounce
            }
        }

    property int listViewItemHeight : 30



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

            Text {
                id: txtInfoCurrentGameBlockedTickets
                visible: false
                color: "white"
                font.pointSize: 10
                text: "<b>Gesperrte Karten</b> <i>(Besitzer geht selbst)</i><b>:</b>"
                Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
            }

            ListView {
                id: listViewBlockedTickets
//                focus: false
                interactive: false
                implicitWidth: mainColumnLayoutCurrentGame.width

                delegate: MyComponents.TicketInfoSingleItem {
                    id: singleBlockedRow
                    height: listViewItemHeight
                    itemModel: model
                    imageColor : "red"
                    onClickedItem: {
                        var globalCoordinates = singleBlockedRow.mapToItem(flickableCurrentTicketInfo, 0, 0)
                        clickedBlockedMenu.y = globalCoordinates.y - singleBlockedRow.height / 2
                        menuTicketIndex = model.index
                        clickedBlockedMenu.open();
                        model.menuOpen = true
                    }
                }

                model: ListModel {
                    id: listViewModelBlockedTickets
                }
            }

            Text {
                id: txtInfoCurrentGameReservedTickets
                visible: false
                color: "white"
                font.pointSize: 10
                text: "<b>Reservierte Karten:</b>"
                Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
            }

            ListView {
                id: listViewReservedTickets
//                focus: false
                interactive: false
                implicitWidth: mainColumnLayoutCurrentGame.width

                delegate: MyComponents.TicketInfoDoubleItem {
                    id: singleReservedRow
//                    height: listViewItemHeight
                    itemModel: model
                    onClickedItem: {
                        var globalCoordinates = singleReservedRow.mapToItem(flickableCurrentTicketInfo, 0, 0)
                        clickedReservedMenu.y = globalCoordinates.y - singleReservedRow.height / 2
                        menuTicketIndex = model.index
                        clickedReservedMenu.open();
                        model.menuOpen = true
                    }
                }

                model: ListModel {
                    id: listViewModelReservedTickets
                }
            }


            Text {
                id: txtInfoCurrentGameFreeTickets
                visible: false
                color: "white"
                font.pointSize: 10
                text: "<b>Freie Karten:</b>"
                Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
            }

            ListView {
                id: listViewFreeTickets
//                focus: false
                interactive: false
                implicitWidth: mainColumnLayoutCurrentGame.width

                delegate: MyComponents.TicketInfoSingleItem {
                    id: singleFreeRow
                    height: listViewItemHeight
                    itemModel: model
                    imageColor : "green"
                    onClickedItem: {
                        var globalCoordinates = singleFreeRow.mapToItem(flickableCurrentTicketInfo, 0, 0)
                        clickedFreeMenu.y = globalCoordinates.y - singleFreeRow.height / 2
                        menuTicketIndex = model.index
                        clickedFreeMenu.open();
                        model.menuOpen = true
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

    function freeTicketItem() {
        userIntCurrentGame.startChangeAvailableTicketState(menuTicketIndex, m_gamePlayCurrentItem.index, 2);
        showInfoHeader("Gebe Karte frei", true)
    }

    function blockTicketItem() {
        userIntCurrentGame.startChangeAvailableTicketState(menuTicketIndex, m_gamePlayCurrentItem.index, 1);
        showInfoHeader("Sperre Karte", true)
    }

    function reserveTicketItem(text) {
        var component = Qt.createComponent("../components/EditableTextDialog.qml");
        if (component.status === Component.Ready) {
            var dialog = component.createObject(mainPaneCurrentGame,{popupType: 1});
            dialog.headerText = text;
            dialog.parentHeight = mainWindow.height
            dialog.parentWidth =  mainPaneCurrentGame.width
            dialog.textMinSize = 4;
            dialog.acceptedTextEdit.connect(acceptedEditReserveNameDialog);
            dialog.open();
        }
    }

    function acceptedEditReserveNameDialog(text)
    {
        userIntCurrentGame.startChangeAvailableTicketState(menuTicketIndex, m_gamePlayCurrentItem.index, 3, text);
        showInfoHeader("Reserviere Karte", true)
    }

    Menu {
        id: clickedBlockedMenu
        x: (flickableCurrentTicketInfo.width - width) / 2
        y: flickableCurrentTicketInfo.height / 6
        onAboutToHide:  {
            for (var i = 0; i < listViewModelBlockedTickets.count; i++) {
                var model = listViewModelBlockedTickets.get(i);
                model.menuOpen = false;
            }
        }

        background: Rectangle {
                implicitWidth: menuItemFree.width
                color: "#4f4f4f"
            }
        MenuItem {
            id: menuItemFree
            text : "Freigeben"
            onClicked: freeTicketItem();
        }
    }

    Menu {
        id: clickedReservedMenu
        x: (flickableCurrentTicketInfo.width - width) / 2
        y: flickableCurrentTicketInfo.height / 6
        onAboutToHide:  {
            for (var i = 0; i < listViewModelReservedTickets.count; i++)
                var model = listViewModelReservedTickets.get(i).menuOpen = false;
        }

        background: Rectangle {
                implicitWidth: menuItemBlockFromReserve.width
                color: "#4f4f4f"
            }

        MenuItem {
            id: menuItemFreeFromReserve
            text: "Freigeben"
            onClicked: freeTicketItem();
        }

        MenuItem {
            id: menuItemBlockFromReserve
            text: "Sperren"
            onClicked: blockTicketItem();
        }

        MenuItem {
            id: menuItemChangeReserve
            text: "Reservierung ändern"
            onClicked: reserveTicketItem("Reservierung ändern");
        }
    }

    Menu {
        id: clickedFreeMenu
        x: (flickableCurrentTicketInfo.width - width) / 2
        y: flickableCurrentTicketInfo.height / 6
        onAboutToHide:  {
            for (var i = 0; i < listViewModelFreeTickets.count; i++)
                var model = listViewModelFreeTickets.get(i).menuOpen = false;
        }

        background: Rectangle {
                implicitWidth: menuItemBlock.width
                color: "#4f4f4f"
            }

        MenuItem {
            id: menuItemReserve
            text: "Reservieren"
            onClicked: reserveTicketItem("Reserviere für");
        }

        MenuItem {
            id: menuItemBlock
            text: "Sperren"
            onClicked: blockTicketItem();
        }
    }

    function showAllInfoAboutGame(sender) {

        loadAvailableTicketList()
    }

    function notifyUserIntSeasonTicketListFinished(result) {

        if (result === 1) {
            loadAvailableTicketList()
        } else {
            toastManager.show(userIntCurrentGame.getErrorCodeToString(result), 4000);
            showInfoHeader(userIntCurrentGame.getErrorCodeToString(result), false)
        }
    }

    function notifyAvailableTicketStateChangedFinished(result) {
        if (result === 1) {
            toastManager.show("Status erfolgreich geändert", 2000);
            loadAvailableTicketList()
        } else {
            toastManager.show(userIntCurrentGame.getErrorCodeToString(result), 4000);
            showInfoHeader("", false);
        }
    }

    function loadAvailableTicketList() {
        listViewModelBlockedTickets.clear()
        listViewModelReservedTickets.clear()
        listViewModelFreeTickets.clear()
        listViewBlockedTickets.implicitHeight = 0;
        listViewReservedTickets.implicitHeight = 0;
        listViewFreeTickets.implicitHeight = 0;
        showInfoHeader("Aktualisiere Daten", true);
//        console.log("Aktualisisere Daten")
        userIntCurrentGame.startRequestAvailableTickets(m_gamePlayCurrentItem.index);
    }

    function notifyAvailableTicketListFinished(result) {
        if (result === 1) {
            toastManager.show("Karten geladen", 2000);
            showInfoHeader("", false)
        } else {
            toastManager.show(userIntCurrentGame.getErrorCodeToString(result), 4000);
            txtInfoCurrentGame.text = "Karten konnten nicht geladen werden"
            showInfoHeader("Karten konnten nicht geladen werden", false)
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
                                                        index: seasonTicketItem.index,
                                                        place: seasonTicketItem.place,
                                                        menuOpen: false
                                                    });
                }
                else if (seasonTicketItem.getTicketState() === 3) {
                    listViewModelReservedTickets.append({
                                                        title: seasonTicketItem.name + discount,
                                                        reserve: seasonTicketItem.getTicketReserveName(),
                                                        index: seasonTicketItem.index,
                                                        place: seasonTicketItem.place,
                                                        menuOpen: false
                                                    });
                }
                else
                    listViewModelBlockedTickets.append({
                                                       title: seasonTicketItem.name + discount,
                                                       index: seasonTicketItem.index,
                                                       place: seasonTicketItem.place,
                                                       menuOpen: false
                                                   })
            }
            /* Does not work in defintion for freeTickets, so set it here */
            listViewBlockedTickets.implicitHeight = listViewModelBlockedTickets.count * listViewItemHeight
            listViewReservedTickets.implicitHeight = listViewModelReservedTickets.count * listViewItemHeight * ( 20 / 12)
            listViewFreeTickets.implicitHeight = listViewModelFreeTickets.count * listViewItemHeight

            txtInfoCurrentGameBlockedTickets.visible = true
            txtInfoCurrentGameReservedTickets.visible = true
            txtInfoCurrentGameFreeTickets.visible = true

        } else {
            txtInfoCurrentGameBlockedTickets.visible = false
            txtInfoCurrentGameReservedTickets.visible = false
            txtInfoCurrentGameFreeTickets.visible = false
            listViewBlockedTickets.implicitHeight = 0;
            listViewReservedTickets.implicitHeight = 0;
            listViewFreeTickets.implicitHeight = 0;

//            txtInfoCurrentGame.visible = true;
        }


//        txtInfoCurrentGame.text = "Letztes Update am "
//                + globalUserData.getSeasonTicketLastLocalUpdateString()
    }
}
