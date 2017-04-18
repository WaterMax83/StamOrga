import QtQuick 2.7
import QtQuick.Controls 2.1
import QtQuick.Layouts 1.0

import com.watermax.demo 1.0


Flickable {
    id: flickableTickets
    property UserInterface userIntTicket

    contentHeight: mainTicketColumnLayout.height

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

        Button {
            id: btnAddSeasonTicket
            text: qsTr("Dauerkarte hinzufügen")
            implicitWidth: mainTicketColumnLayout.width / 3 * 2
            Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
            transformOrigin: Item.Center
            onClicked: {
//                userInt

                txtnewSeasonTicketName.text = globalUserData.readableName
                addSeasonTicketDlg.open()
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
    }

    Component {
        id: seasonTicketItem
        Rectangle {
            id: ticketRectangleItem
            property var m_SeasonTicketItem

            width: parent.width
            height: 50
            color: "#81848E"
            ColumnLayout {
                id: columnLayoutTicketItem
                width: ticketRectangleItem.width
                spacing: 3
                Label {
                    id: labelTicketItem
                    text: qsTr("text")
                    Layout.alignment: Qt.AlignLeft | Qt.AlignHCenter
                }
            }

            function showTicketInfo(seasontTicketItem) {
                if (seasontTicketItem !== null) {
                    m_SeasonTicketItem = seasontTicketItem
                    labelTicketItem.text = seasontTicketItem.name + " bei " + seasontTicketItem.place
                }
            }
        }
    }

    ScrollIndicator.vertical: ScrollIndicator { }

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
                userIntTicket.startAddSeasonTicket(txtnewSeasonTicketName.text, 0);
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
