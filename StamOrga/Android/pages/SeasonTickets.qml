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
    id: flickableTickets
    property UserInterface userIntTicket

    contentHeight: mainPaneTickets.height

    rebound: Transition {
            NumberAnimation {
                properties: "y"
                duration: 1000
                easing.type: Easing.OutBounce
            }
        }

    onDragEnded: {
        if (flickableTickets.contentY < -100) {
            updateSeasonTicketList();
        }
    }

    Rectangle {
        Image {
            id: refreshImage
            source: "../images/refresh.png"
            rotation: (flickableTickets.contentY > -100) ? (flickableTickets.contentY * -1) * 2 : 220
            transformOrigin: Item.Center
        }
        opacity: (flickableTickets.contentY * -1) / 100
        color: "black"
        width: refreshImage.width
        height: refreshImage.height
        radius: width * 0.5
        y: 50
        x: (mainWindow.width / 2) - (width / 2)
        z: 1000
    }

    Pane {
        id: mainPaneTickets
        width: parent.width

        ColumnLayout {
            id: mainTicketColumnLayout
            width: parent.width
            spacing: 10

            ColumnLayout {
                id: columnLayout
                spacing: 5
                Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter

                BusyIndicator {
                    id: busyConnectIndicatorTicket
                    opacity: 1
                    visible: false
                    Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                }

                Label {
                    id: txtInfoSeasonTicket
                    text: qsTr("Label")
                    visible: true
                    horizontalAlignment: Text.AlignHCenter
                    Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                }
            }

            Column {
                id: columnLayoutTickets
                anchors.right: parent.right
                anchors.left: parent.left
                width: parent.width
                spacing: 10
                Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter

            }
        }
    }

    function toolButtonClicked() {
        if (globalSettings.useReadableName)
            txtnewSeasonTicketName.text = globalUserData.readableName
        else
            txtnewSeasonTicketName.text = ""
        addSeasonTicketDlg.open()
    }

    function updateSeasonTicketList(){
        userIntTicket.startListSeasonTickets()
        busyConnectIndicatorTicket.visible = true;
        txtInfoSeasonTicket.text = "Aktualisiere Dauerkarten Liste"
        for (var j = columnLayoutTickets.children.length; j > 0; j--) {
            columnLayoutTickets.children[j-1].destroy()
        }
    }

    function notifyUserIntSeasonTicketAdd(result) {
        busyConnectIndicatorTicket.visible = false;
        if (result === 1) {
            updateSeasonTicketList()
            toastManager.show("Karte erfolgreich hinzugefügt", 2000);
        }
        else
            txtInfoSeasonTicket.text = userIntTicket.getErrorCodeToString(result)
    }

    function notifyUserIntSeasonTicketRemoveFinished(result) {
        if (result === 1) {
            updateSeasonTicketList()
            toastManager.show("Karte erfolgreich gelöscht", 2000);
        }
    }

    function notifyUserIntSeasonTicketListFinished(result) {
        busyConnectIndicatorTicket.visible = false;
        if (result === 1) {
            toastManager.show("Karten geladen", 2000)
        } else {
            toastManager.show(userIntTicket.getErrorCodeToString(result), 5000);
        }
        showSeasonTickets();
    }

    function notifyUserIntSeasonTicketNewPlaceFinished(result) {
        busyConnectIndicatorTicket.visible = false;
        if (result === 1) {
            updateSeasonTicketList()
            toastManager.show("Ort erfolgreich geändert", 2000);
        }  else {
            txtInfoSeasonTicket.text = userIntTicket.getErrorCodeToString(result);
        }
    }

    function notifyUserIntConnectionFinished(result) {}

    function pageOpenedUpdateView() {
        showSeasonTickets()
    }

    function showSeasonTickets() {

        for (var j = columnLayoutTickets.children.length; j > 0; j--) {
            columnLayoutTickets.children[j-1].destroy()
        }

        if (globalUserData.getSeasonTicketLength() > 0) {
//            /* Somehow the first element is not visible, so create an not used item */
//            var test = seasonTicketItem.createObject(columnLayoutTickets)
            for (var i=0; i<globalUserData.getSeasonTicketLength(); i++) {
                var sprite = seasonTicketItem.createObject(columnLayoutTickets)
                sprite.showTicketInfo(i);
            }
            txtInfoSeasonTicket.text = "Letztes Update am " + globalUserData.getSeasonTicketLastUpdateString()
        } else
            txtInfoSeasonTicket.text = "Keine Daten zum Anzeigen\nZiehen zum Aktualisieren"

    }

    Component {
        id: seasonTicketItem
        MyComponents.SeasonTicket {
            onClickedSeasonTicket: {
                seasonTicketClickedMenu.openWithNameAndIndex(sender.name, sender.index)
            }
        }
    }



    ScrollIndicator.vertical: ScrollIndicator { }

    property var m_ticketNameToChangeIndex
    property var m_ticketNameToChange

    Menu {
            id: seasonTicketClickedMenu
            x: (flickableTickets.width - width) / 2
            y: flickableTickets.height / 6
            title : "Test"
//            implicitHeight: mainPaneTickets.height

            background: Rectangle {
                    implicitWidth: menuItemChange.width
                    color: "#4f4f4f"
                }

            MenuItem {
                id: menuItemChange
                onClicked: {
                    var component = Qt.createComponent("../components/EditableTextDialog.qml");
                    if (component.status === Component.Ready) {
                        var dialog = component.createObject(mainPaneTickets,{popupType: 1});
                        dialog.headerText = "Neuer Ort für " + m_ticketNameToChange;
                        dialog.parentHeight = flickableTickets.height
                        dialog.parentWidth = flickableTickets.width
                        dialog.textMinSize = 4;
                        dialog.acceptedTextEdit.connect(acceptedEditNewSeasonTicketPlace);
                        dialog.open();
                    }
                }
            }
            MenuItem {
                id: menuItemRemove
                onClicked: {
                    var component = Qt.createComponent("../components/AcceptDialog.qml");
                    if (component.status === Component.Ready) {
                        var dialog = component.createObject(mainPaneTickets,{popupType: 1});
                        dialog.headerText = "Bestätigung";
                        dialog.parentHeight = flickableTickets.height
                        dialog.parentWidth = flickableTickets.width
                        dialog.textToAccept = "Soll die Dauerkarte " + m_ticketNameToChange + " wirklich gelöscht werden?";
                        dialog.acceptedDialog.connect(acceptedDeletingTicket);
                        dialog.open();
                    }
                }
            }

            function openWithNameAndIndex(name, index)
            {
                m_ticketNameToChangeIndex = index
                m_ticketNameToChange = name
                menuItemChange.text = "Neuer Ort für " + name
                menuItemRemove.text = "Lösche " + name
                seasonTicketClickedMenu.open()
            }
        }

    function acceptedEditNewSeasonTicketPlace(text) {
        busyConnectIndicatorTicket.visible = true;
        txtInfoSeasonTicket.visible = true;
        txtInfoSeasonTicket.text = "Ändere Ort für " + m_ticketNameToChange;
        userIntTicket.startNewPlaceSeasonTicket(m_ticketNameToChangeIndex, text);
    }

    function acceptedDeletingTicket() {
        busyConnectIndicatorTicket.visible = true;
        txtInfoSeasonTicket.visible = true;
        txtInfoSeasonTicket.text = "Lösche Dauerkarte"
        userIntTicket.startRemoveSeasonTicket(m_ticketNameToChangeIndex)
        console.log("Lösche Dauerkarte")
    }


    Dialog {
        id: addSeasonTicketDlg
        modal: true
        focus: true
        title: "Neue Dauerkarte"
        x: (flickableTickets.width - width) / 2
        y: flickableTickets.height / 6
        width: Math.min(flickableTickets.width, flickableTickets.height) / 3 * 2

        standardButtons: Dialog.Ok | Dialog.Cancel
        onAccepted: {
            labelTicketNameTooShort.visible = false
            if (txtnewSeasonTicketName.text.trim().length < 4) {
                labelTicketNameTooShort.visible = true
                addSeasonTicketDlg.open()
            } else {
                userIntTicket.startAddSeasonTicket(txtnewSeasonTicketName.text.trim(), chBoxDiscount.checked ? 1 : 0);
                busyConnectIndicatorTicket.visible = true;
                txtInfoSeasonTicket.text = "Füge Dauerkarte hinzu"
                addSeasonTicketDlg.close();
            }

        }
        onRejected: {
            addSeasonTicketDlg.close()
            labelTicketNameTooShort.visible = false
        }

        contentItem: ColumnLayout {
            id: addSeasonTicketColumn
            spacing: 20
            width: addSeasonTicketDlg.width

            ColumnLayout {
                id: columnLayoutAddTicketName
                Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter

                Label {
                    id: labelAddTicketName
                    text: qsTr("Name")
                    Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                }

                TextField {
                    id: txtnewSeasonTicketName
                    implicitWidth: addSeasonTicketColumn.width / 4 * 3
                    Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                }
            }

            CheckBox {
                id: chBoxDiscount
                text: "ermäßigt"
                checked: false
            }

            Label {
                id: labelTicketNameTooShort
                visible: false
                text: qsTr("Der Name muss mindestens 6 Zeichen lang sein")
                Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                wrapMode: Text.WordWrap
                Layout.maximumWidth: parent.width
                color: "orange"
            }
        }
    }
}
