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
                visible: false
            }
        }

        Rectangle {
            width: 150
            height: 150
            color: "blue"
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
        console.log("Clicked the ToolButton inside SeasonTIcket")
    }

    function notifySeasonTicketAdd(result) {
        busyConnectIndicatorTicket.visible = false;
        if (result === 1) {
            txtInfoSeasonTicket.text = "Success"
        }
        else if (result === -7)
            txtInfoSeasonTicket.text = "Dauerkarte existiert bereits"
        else
            txtInfoSeasonTicket.text = "Error: " + result
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
                txtInfoSeasonTicket.visible = true;
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
