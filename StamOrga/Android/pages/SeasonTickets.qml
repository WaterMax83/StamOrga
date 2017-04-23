import QtQuick 2.7
import QtQuick.Controls 2.1
import QtQuick.Layouts 1.0

import com.watermax.demo 1.0

import "../components" as MyComponents

Flickable {
    id: flickableTickets
    property UserInterface userIntTicket

    contentHeight: mainPaneTickets.height

    Pane {
        id: mainPaneTickets
        width: parent.width

        ColumnLayout {
            id: mainTicketColumnLayout
            width: parent.width
            spacing: 10

            ColumnLayout {
                id: columnLayout
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
                }
            }

            ColumnLayout {
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
        userIntTicket.startGettingSeasonTicketList()
        busyConnectIndicatorTicket.visible = true;
        txtInfoSeasonTicket.text = "Lade Dauerkarten Liste"
    }

    function notifyUserIntSeasonTicketAdd(result) {
        busyConnectIndicatorTicket.visible = false;
        if (result === 1) {
            toolButtonClicked()
        }
        else
            txtInfoSeasonTicket.text = userIntTicket.getErrorCodeToString(result)
    }

    function notifyUserIntSeasonTicketRemoveFinished(result) {
        console.log("Finished Delete " + result)
        if (result === 1) {
            toolButtonClicked()
        }
    }

    function notifyUserIntSeasonTicketListFinished(result) {
        busyConnectIndicatorTicket.visible = false;
        if (result === 1) {
            showSeasonTickets()
        } else {
            txtInfoSeasonTicket.text = userIntTicket.getErrorCodeToString(result);
        }
    }

    function notifyUserIntVersionRequestFinished(result, msg) {
        if (result === 5) {
            var component = Qt.createComponent("../components/VersionDialog.qml");
            if (component.status === Component.Ready) {
                var dialog = component.createObject(parent,{popupType: 1});
                dialog.versionText = msg;
                dialog.parentHeight = flickableTickets.height
                dialog.parentWidth = flickableTickets.width
                dialog.open();
            }
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
            for (var i=0; i<globalUserData.getSeasonTicketLength(); i++) {
                var sprite = seasonTicketItem.createObject(columnLayoutTickets)
                sprite.showTicketInfo(globalUserData.getSeasonTicket(i))
            }
            txtInfoSeasonTicket.text = "Letzes Update am " + globalUserData.getSeasonTicketLastUpdate()
        } else
            txtInfoSeasonTicket.text = "Keine Daten zum anzeigen"

        var btn = btnAddSeasonTicket.createObject(columnLayoutTickets)
    }

    Component {
        id: seasonTicketItem
        MyComponents.SeasonTicket {
            onClickedSeasonTicket: {
                console.log("Outside clicked " + sender.name + " " + sender.index);
                seasonTicketClickedMenu.openWithNameAndIndex(sender.name, sender.index)
            }
        }
    }

    Component {
        id: btnAddSeasonTicket
        Button {
            text: qsTr("Dauerkarte hinzufügen")
            implicitWidth: mainTicketColumnLayout.width / 3 * 2
            Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
            transformOrigin: Item.Center
            onClicked: {
                txtnewSeasonTicketName.text = globalUserData.readableName
                addSeasonTicketDlg.open()
            }
        }
    }

    ScrollIndicator.vertical: ScrollIndicator { }

    property var m_ticketNameToChangeIndex

    Menu {
            id: seasonTicketClickedMenu
            x: (flickableTickets.width - width) / 2
            y: flickableTickets.height / 6

            background: Rectangle {
                    implicitWidth: menuItemChange.width
                    color: "#303030"
                }

            MenuItem {
                id: menuItemChange
                onClicked: {
                    console.log("Ändern")
                }
            }
            MenuItem {
                id: menuItemRemove
                onClicked: {
                    userIntTicket.startRemoveSeasonTicket(m_ticketNameToChangeIndex)
                }
            }
            function openWithNameAndIndex(name, index)
            {
                console.log("new indx for openeing " + index)
                m_ticketNameToChangeIndex = index
                menuItemChange.text = "Ändere " + name
                menuItemRemove.text = "Lösche " + name
                seasonTicketClickedMenu.open()
            }
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
            if (txtnewSeasonTicketName.text.length < 6) {
                labelTicketNameTooShort.visible = true
                addSeasonTicketDlg.open()
            } else {
                userIntTicket.startAddSeasonTicket(txtnewSeasonTicketName.text, chBoxDiscount.checked ? 1 : 0);
                busyConnectIndicatorTicket.visible = true;
                txtInfoSeasonTicket.text = "Füge Dauerkarte hinzu"
                addSeasonTicketDlg.close()
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
