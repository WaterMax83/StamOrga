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
    id: flickableTickets
//    property UserInterface userIntTicket

    contentHeight: mainPaneTickets.height

    rebound: Transition {
            NumberAnimation {
                properties: "y"
                duration: 1000
                easing.type: Easing.OutBounce
            }
        }

    onDragEnded: {
        if (flickableTickets.contentY < -refreshItem.refreshHeight) {
            updateSeasonTicketList();
        }
    }

    MyComponents.RefreshItem {
        id: refreshItem
        contentY: flickableTickets.contentY
    }

    Pane {
        id: mainPaneTickets
        width: parent.width
        padding: 0

        ColumnLayout {
            id: mainTicketColumnLayout
            width: parent.width
            spacing: 10

            MyComponents.BusyLoadingIndicator {
                id: busyIndicatorTicket
                width: parent.width
                Layout.alignment: Qt.AlignHCenter | Qt.AlignTop
                Layout.fillHeight: false
                Layout.topMargin: 10
                infoVisible: true
            }

            Column {
                id: columnLayoutTickets
                anchors.right: parent.right
                anchors.left: parent.left
                width: parent.width
                spacing: 5
                Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter

            }
        }
    }

    function toolButtonClicked() {
        var component = Qt.createComponent("../components/EditableTextDialog.qml");
        if (component.status === Component.Ready) {
            var dialog = component.createObject(flickableTickets,{popupType: 1});
            dialog.headerText = "Neue Dauerkarte";
            dialog.parentHeight = flickableTickets.height
            dialog.parentWidth =  flickableTickets.width
            dialog.textMinSize = 4;
            dialog.editableText = gConUserSettings.getReadableName();
            dialog.checkBoxChecked = false
            dialog.checkBoxText = "ermäßigt";
            dialog.infoText = "Name";
            dialog.acceptedTextEditAndCheckBox.connect(acceptedAddSeasonTicketDialog);
            dialog.font.family= txtForFontFamily.font
            dialog.open();
        } else
            console.log(component.errorString());
    }

    function acceptedAddSeasonTicketDialog(text, discount) {
        gDataTicketManager.startAddSeasonTicket(0, text, text, discount);
//        userInt.startAddSeasonTicket(text, discount);
        busyIndicatorTicket.loadingVisible = true
        busyIndicatorTicket.infoText = "Füge Dauerkarte hinzu"
    }

    function updateSeasonTicketList(){
        gDataTicketManager.startListSeasonTickets()
        busyIndicatorTicket.loadingVisible = true
        busyIndicatorTicket.infoText = "Aktualisiere Dauerkarten Liste"
        for (var j = columnLayoutTickets.children.length; j > 0; j--) {
            columnLayoutTickets.children[j-1].destroy()
        }
    }

    function notifyUserIntSeasonTicketAdd(result) {
        busyIndicatorTicket.loadingVisible = false
        if (result === 1) {
            updateSeasonTicketList()
            toastManager.show("Karte erfolgreich hinzugefügt", 2000);
        }
        else {
            busyIndicatorTicket.infoText = userInt.getErrorCodeToString(result)
            toastManager.show(userInt.getErrorCodeToString(result), 4000);
        }
    }

    function notifyUserIntSeasonTicketRemoveFinished(result) {
        if (result === 1) {
            updateSeasonTicketList()
            toastManager.show("Karte erfolgreich gelöscht", 2000);
        } else {
            toastManager.show(userInt.getErrorCodeToString(result), 4000);
            busyIndicatorTicket.loadingVisible = false;
            busyIndicatorTicket.infoText = "Dauerkarte konnte nicht gelöscht werden"
        }
    }

    function notifyUserIntSeasonTicketListFinished(result) {
//        busyConnectIndicatorTicket.visible = false;
        busyIndicatorTicket.loadingVisible = false
        if (result === 1) {
            toastManager.show("Karten geladen", 2000)
        } else {
            toastManager.show(userInt.getErrorCodeToString(result), 5000);
        }
        showSeasonTickets();
    }

    function notifyUserIntSeasonTicketEditFinished(result) {
        busyIndicatorTicket.loadingVisible = false
        if (result === 1) {
            updateSeasonTicketList()
            toastManager.show("Erfolgreich geändert", 2000);
        }  else {
            busyIndicatorTicket.infoText = userInt.getErrorCodeToString(result);
        }
    }

    function notifyUserIntConnectionFinished(result) {}

    function pageOpenedUpdateView() {

        updateSeasonTicketList();
//        showSeasonTickets()
    }

    function showSeasonTickets() {

        for (var j = columnLayoutTickets.children.length; j > 0; j--) {
            columnLayoutTickets.children[j-1].destroy()
        }

        if (gDataTicketManager.getSeasonTicketLength() > 0) {
//            /* Somehow the first element is not visible, so create an not used item */
//            var test = seasonTicketItem.createObject(columnLayoutTickets)
            for (var i=0; i < gDataTicketManager.getSeasonTicketLength(); i++) {
                var sprite = seasonTicketItem.createObject(columnLayoutTickets)
                sprite.showTicketInfo(i);
            }
            busyIndicatorTicket.infoText = "Letztes Update vor " + gDataTicketManager.getSeasonTicketLastLocalUpdateString()
        } else
            busyIndicatorTicket.infoText = "Keine Daten gespeichert\nZiehen zum Aktualisieren"
    }

    Component {
        id: seasonTicketItem
//        MyComponents.SeasonTicket {
        MyComponents.SeasonTicketDesignItem {
            onClickedSeasonTicket: {
                seasonTicketClickedMenu.openWithNameAndIndex(sender.name, sender.place, sender.discount, sender.index)
            }
        }
    }

    Text {
        id: txtForFontFamily
        visible: false
    }



    ScrollIndicator.vertical: ScrollIndicator { }

    property var m_ticketNameToChangeIndex
    property var m_ticketNameToChange
    property var m_ticketPlaceToChange
    property var m_ticketDiscountToChange

    Menu {
            id: seasonTicketClickedMenu
            x: (flickableTickets.width - width) / 2
            y: flickableTickets.height / 6

            background: Rectangle {
                    implicitWidth: menuItemChangePlace.width
                    color: "#4f4f4f"
                }

            MenuItem {
                id: menuItemEdit
                font.family: txtForFontFamily.font
                onClicked: {
                    var component = Qt.createComponent("../components/EditableTextDialog.qml");
                    if (component.status === Component.Ready) {
                        var dialog = component.createObject(flickableTickets,{popupType: 1});
                        dialog.headerText = "Editiere Dauerkarte";
                        dialog.parentHeight = flickableTickets.height
                        dialog.parentWidth =  flickableTickets.width
                        dialog.textMinSize = 4;
                        dialog.font.family= txtForFontFamily.font
                        dialog.editableText = m_ticketNameToChange;
                        if (m_ticketDiscountToChange === 0)
                            dialog.checkBoxChecked = false
                        else
                            dialog.checkBoxChecked = true
                        dialog.checkBoxText = "ermäßigt";
                        dialog.infoText = "Name";
                        dialog.acceptedTextEditAndCheckBox.connect(acceptedEditSeasonTicketDialog);
                        dialog.open();
                    } else
                        console.log(component.errorString());
                }
            }
            MenuItem {
                id: menuItemChangePlace
                font.family: txtForFontFamily.font
                onClicked: {
                    var component = Qt.createComponent("../components/EditableTextDialog.qml");
                    if (component.status === Component.Ready) {
                        var dialog = component.createObject(flickableTickets,{popupType: 1});
                        dialog.headerText = m_ticketNameToChange;
                        dialog.parentHeight = flickableTickets.height
                        dialog.parentWidth = flickableTickets.width
                        dialog.textMinSize = 4;
                        dialog.font.family= txtForFontFamily.font
                        dialog.infoText = "Neuer Ort"
                        dialog.acceptedTextEdit.connect(acceptedEditNewSeasonTicketPlace);
                        dialog.open();
                    }
                }
            }
            MenuItem {
                id: menuItemRemove
                font.family: txtForFontFamily.font
                onClicked: {
                    var component = Qt.createComponent("../components/AcceptDialog.qml");
                    if (component.status === Component.Ready) {
                        var dialog = component.createObject(flickableTickets,{popupType: 1});
                        dialog.headerText = "Bestätigung";
                        dialog.parentHeight = flickableTickets.height
                        dialog.parentWidth = flickableTickets.width
                        dialog.font.family= txtForFontFamily.font
                        dialog.textToAccept = "Soll die Dauerkarte " + m_ticketNameToChange + " wirklich gelöscht werden?";
                        dialog.acceptedDialog.connect(acceptedDeletingTicket);
                        dialog.open();
                    }
                }
            }

            function openWithNameAndIndex(name, place, discount, index)
            {
                m_ticketNameToChangeIndex = index
                m_ticketNameToChange = name
                m_ticketPlaceToChange = place
                m_ticketDiscountToChange = discount
                menuItemEdit.text = "Editiere " + name
                menuItemChangePlace.text = "Neuer Ort für " + name
                menuItemRemove.text = "Lösche " + name
                seasonTicketClickedMenu.open()
            }
        }

    function acceptedEditNewSeasonTicketPlace(text) {
        busyIndicatorTicket.loadingVisible = true;
        busyIndicatorTicket.infoText = "Ändere Ort für " + m_ticketNameToChange;
        gDataTicketManager.startAddSeasonTicket(m_ticketNameToChangeIndex, m_ticketNameToChange, text, m_ticketDiscountToChange)
    }

    function acceptedDeletingTicket() {
        busyIndicatorTicket.loadingVisible = true;
        busyIndicatorTicket.infoText = "Lösche Dauerkarte"
        gDataTicketManager.startRemoveSeasonTicket(m_ticketNameToChangeIndex);
    }

    function acceptedEditSeasonTicketDialog(text, discount) {
        busyIndicatorTicket.loadingVisible = true;
        busyIndicatorTicket.infoText = "Editiere " + m_ticketNameToChange;
        gDataTicketManager.startAddSeasonTicket(m_ticketNameToChangeIndex, text, m_ticketPlaceToChange, discount);
    }
}
