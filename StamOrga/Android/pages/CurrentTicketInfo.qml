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

Flickable {
    id: flickableCurrentTicketInfo
    clip: true
    height: parent.height * 1.2
    contentHeight: mainPaneCurrentTicketInfo.height
//    property bool useCommentLine : false

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
        if (flickableCurrentTicketInfo.contentY < -refreshItem.refreshHeight) {
            loadAvailableTicketList();
        }
    }

    MyComponents.RefreshItem {
        id: refreshItem
        contentY: flickableCurrentTicketInfo.contentY
    }

    Pane {
        id: mainPaneCurrentTicketInfo
        width: parent.width

        ColumnLayout {
            id: mainColumnLayoutCurrentTicketInfo
            width: parent.width

            Text {
                id: txtInfoCurrentGameFreeTickets
                visible: columnLayoutFreeTickets.children.length > 0 ? true : false
                color: "grey"
                font.pixelSize: 14
                text: "<b>Freie Karten:</b>"
                Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
            }

            Column {
                id: columnLayoutFreeTickets
                anchors.right: parent.right
                anchors.left: parent.left
                spacing: 5
                visible: children.length > 0 ? true : false
                Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
            }

            Text {
                id: txtInfoCurrentGameReservedTickets
                visible: columnLayoutReservedTickets.children.length > 0 ? true : false
                color: "grey"
                font.pixelSize: 14
                text: "<b>Reservierte Karten:</b>"
                Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
            }

            Column {
                id: columnLayoutReservedTickets
                anchors.right: parent.right
                anchors.left: parent.left
                spacing: 5
                visible: children.length > 0 ? true : false
                Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
            }

            Text {
                id: txtInfoCurrentGameBlockedTickets
                visible: false
                color: "grey"
                font.pixelSize: 14
                text: "<b>Gesperrte Karten</b> <i>(Besitzer geht selbst)</i><b>:</b>"
                Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
            }

            Column {
                id: columnLayoutBlockedTickets
                anchors.right: parent.right
                anchors.left: parent.left
                spacing: 5
                Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
            }
        }
    }

    Text {
        id: txtForFontFamily
        visible: false
    }

    Component {
        id: singleTicketView
        MyComponents.TicketInfoSingleItem {
            onClickedItem: {
                if (imageColor == "green") {
                    var globalCoordinates1 = columnLayoutFreeTickets.mapToItem(flickableCurrentTicketInfo, 0, 0)
                    clickedFreeMenu.y = y + globalCoordinates1.y
                    clickedFreeMenu.open();
                }else {
                    var globalCoordinates2 = columnLayoutBlockedTickets.mapToItem(flickableCurrentTicketInfo, 0, 0)
                    clickedBlockedMenu.y = y + globalCoordinates2.y
                    clickedBlockedMenu.open();
                }
                menuTicketIndex = ticketIndex
                menuOpen = true
            }
        }
    }

    Component {
        id: doubleTicketView
        MyComponents.TicketInfoDoubleItem {
            onClickedItem: {
                var globalCoordinates = columnLayoutReservedTickets.mapToItem(flickableCurrentTicketInfo, 0, 0)
                clickedReservedMenu.y = y + globalCoordinates.y
                menuTicketIndex = ticketIndex
                clickedReservedMenu.open();
                menuOpen = true
            }
        }
    }

    ScrollIndicator.vertical: ScrollIndicator {
    }

    property int menuTicketIndex
    property string menuTicketReserveName

    function freeTicketItem() {
        gDataTicketManager.startChangeAvailableTicketState(menuTicketIndex, m_gamePlayCurrentItem.index, 2);
        showInfoHeader("Gebe Karte frei", true)
    }

    function blockTicketItem() {
        gDataTicketManager.startChangeAvailableTicketState(menuTicketIndex, m_gamePlayCurrentItem.index, 1);
        showInfoHeader("Sperre Karte", true)
    }

    function reserveTicketItem(header, text) {
        var component = Qt.createComponent("../components/EditableTextDialog.qml");
        if (component.status === Component.Ready) {
            var dialog = component.createObject(mainPaneCurrentGame,{popupType: 1});
            dialog.headerText = header;
            dialog.parentHeight = mainWindow.height
            dialog.parentWidth =  mainPaneCurrentGame.width
            dialog.textMinSize = 4;
            dialog.editableText = text;
            dialog.font.family= txtForFontFamily.font
            dialog.acceptedTextEdit.connect(acceptedEditReserveNameDialog);
            dialog.open();
        }
    }

    function acceptedEditReserveNameDialog(text)
    {
        gDataTicketManager.startChangeAvailableTicketState(menuTicketIndex, m_gamePlayCurrentItem.index, 3, text);
        showInfoHeader("Reserviere Karte", true)
    }

    Menu {
        id: clickedBlockedMenu
        x: (flickableCurrentTicketInfo.width - width) / 2
        y: flickableCurrentTicketInfo.height / 6
        onAboutToHide:  {
            for (var i = 0; i < columnLayoutBlockedTickets.children.length; i++) {
                var model = columnLayoutBlockedTickets.children[i];
                model.menuOpen = false;
            }
        }

        background: Rectangle {
            implicitWidth: menuItemFree.width
            color: "#4f4f4f"
        }
        MenuItem {
            id: menuItemFree
            font.family: txtForFontFamily.font
            text : "Freigeben"
            onClicked: freeTicketItem();
        }
    }

    Menu {
        id: clickedReservedMenu
        x: (flickableCurrentTicketInfo.width - width) / 2
        y: flickableCurrentTicketInfo.height / 6
        onAboutToHide:  {
            for (var i = 0; i < columnLayoutReservedTickets.children.length; i++)
                var model = columnLayoutReservedTickets.children[i].menuOpen = false;
        }

        background: Rectangle {
            implicitWidth: menuItemBlockFromReserve.width
            color: "#4f4f4f"
        }

        MenuItem {
            id: menuItemFreeFromReserve
            font.family: txtForFontFamily.font
            text: "Freigeben"
            onClicked: freeTicketItem();
        }

        MenuItem {
            id: menuItemBlockFromReserve
            font.family: txtForFontFamily.font
            text: "Sperren"
            onClicked: blockTicketItem();
        }

        MenuItem {
            id: menuItemChangeReserve
            font.family: txtForFontFamily.font
            text: "Reservierung ändern"
            onClicked: reserveTicketItem("Reservierung ändern", menuTicketReserveName);
        }
    }

    Menu {
        id: clickedFreeMenu
        x: (flickableCurrentTicketInfo.width - width) / 2
        y: flickableCurrentTicketInfo.height / 6
        onAboutToHide:  {
            for (var i = 0; i < columnLayoutFreeTickets.children.length; i++)
                var model = columnLayoutFreeTickets.children[i].menuOpen = false;
        }

        background: Rectangle {
            implicitWidth: menuItemBlock.width
            color: "#4f4f4f"
        }

        MenuItem {
            id: menuItemReserve
            font.family: txtForFontFamily.font
            text: "Reservieren"
            onClicked: reserveTicketItem("Reserviere für", gConUserSettings.getReadableName());
        }

        MenuItem {
            id: menuItemBlock
            font.family: txtForFontFamily.font
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
            toastManager.show(userIntGames.getErrorCodeToString(result), 4000);
            showInfoHeader(userIntGames.getErrorCodeToString(result), false)
        }
    }

    function notifyAvailableTicketStateChangedFinished(result) {
        if (result === 1) {
            toastManager.show("Status erfolgreich geändert", 2000);
            loadAvailableTicketList()
        } else {
            toastManager.show(userIntGames.getErrorCodeToString(result), 4000);
            showInfoHeader("", false);
        }
    }

    function deleteValues() {

        for (var j = columnLayoutBlockedTickets.children.length; j > 0; j--) {
            columnLayoutBlockedTickets.children[j - 1].destroy()
        }

        for (var k = columnLayoutFreeTickets.children.length; k > 0; k--) {
            columnLayoutFreeTickets.children[k - 1].destroy()
        }

        for (var l = columnLayoutReservedTickets.children.length; l > 0; l--) {
            columnLayoutReservedTickets.children[l - 1].destroy()
        }
    }

    function loadAvailableTicketList() {
        deleteValues();
        showInfoHeader("Aktualisiere Daten", true);
        gDataTicketManager.startListAvailableTickets(m_gamePlayCurrentItem.index);
    }

    function notifyAvailableTicketListFinished(result) {
        if (result === 1) {
            toastManager.show("Karten geladen", 2000);
            showInfoHeader("", false)
        } else {
            toastManager.show(userIntGames.getErrorCodeToString(result), 4000);
            showInfoHeader("Karten konnten nicht geladen werden", false)
        }
        showInternalTicketList(result);
    }

    function showInternalTicketList(result) {

        if (result === 1 && gDataTicketManager.getSeasonTicketLength() > 0) {
            for (var i = 0; i < gDataTicketManager.getSeasonTicketLength(); i++) {
                var seasonTicketItem = gDataTicketManager.getSeasonTicketFromArrayIndex(i)
                var discount = seasonTicketItem.discount > 0 ? " *" : "";
                if (seasonTicketItem.getTicketState() === 2) {
                    var sprTicket1 = singleTicketView.createObject(columnLayoutFreeTickets)
                    sprTicket1.title = seasonTicketItem.name + discount;
                    sprTicket1.ticketIndex = seasonTicketItem.index;
                    sprTicket1.place = seasonTicketItem.place
                    sprTicket1.timestamp = seasonTicketItem.getTimeStamp();
                    sprTicket1.menuOpen = false;
                    sprTicket1.imageColor = "green";
                    sprTicket1.isTicketYourOwn = seasonTicketItem.isTicketYourOwn();
                }
                else if (seasonTicketItem.getTicketState() === 3) {
                    var sprTicket2 = doubleTicketView.createObject(columnLayoutReservedTickets)
                    sprTicket2.title = seasonTicketItem.name + discount;
                    sprTicket2.ticketIndex = seasonTicketItem.index;
                    sprTicket2.reserve = seasonTicketItem.getTicketReserveName();
                    sprTicket2.place = seasonTicketItem.place
                    sprTicket2.timestamp = seasonTicketItem.getTimeStamp();
                    sprTicket2.menuOpen = false;
                    sprTicket2.imageColor = "red";
                    sprTicket2.isTicketYourOwn = seasonTicketItem.isTicketYourOwn();
                }
                else {
                    var sprTicket3 = singleTicketView.createObject(columnLayoutBlockedTickets)
                    sprTicket3.title = seasonTicketItem.name + discount;
                    sprTicket3.ticketIndex = seasonTicketItem.index;
                    sprTicket3.place = seasonTicketItem.place
                    sprTicket3.timestamp = seasonTicketItem.getTimeStamp();
                    sprTicket3.menuOpen = false;
                    sprTicket3.imageColor = "red";
                    sprTicket3.isTicketYourOwn = seasonTicketItem.isTicketYourOwn();
                }
            }

            txtInfoCurrentGameBlockedTickets.visible = true
        } else {
            txtInfoCurrentGameBlockedTickets.visible = false
        }
    }
}
