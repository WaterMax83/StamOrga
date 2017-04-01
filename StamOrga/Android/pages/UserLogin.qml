import QtQuick 2.7
import QtQuick.Controls 2.1
import QtQuick.Layouts 1.0

import com.watermax.demo 1.0

Flickable {
    id: flickable
    width: parent.width
    contentHeight: pane.height

    Pane {
        id: pane
        width: flickable.width
        height: flickable.height * 1.25

        Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter

        ColumnLayout {
            id: mainColumnLayout
            width: parent.width
            spacing: 35

//            anchors.bottom: parent.bottom
//            anchors.bottomMargin: 20
//            anchors.right: parent.right
//            anchors.rightMargin: 0
//            anchors.left: parent.left
//            anchors.leftMargin: 0
//            anchors.top: parent.top
//            anchors.topMargin: 20

            TextField {
                id: txtIPAddress
                text: globalUserData.ipAddr
                implicitWidth: 250
                Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                placeholderText: qsTr("Text Field")
            }

            SpinBox {
                id: spBoxPort
                to: 100000
                from: 1
                value: globalUserData.conMasterPort
                Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
            }

            ColumnLayout {
                id: columnLayout2
                spacing: 5
                Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter

                Label {
                    id: label1
                    text: qsTr("Benutzername")
                    Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                }

                TextField {
                    id: txtUserName
                    text: globalUserData.userName
                    padding: 8
                    implicitWidth: 250
                    Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                }
            }

            ColumnLayout {
                id: columnLayout3
                Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter

                Label {
                    id: label2
                    text: qsTr("Passwort")
                    Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                }

                TextField {
                    id: txtPassWord
                    text: globalUserData.passWord
                    implicitWidth: 250
                    Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                }
            }

            RowLayout {
                id: rowLayout
                Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter

                Column {
                    id: space
                    width: 0
                    height: 81
                }

                ColumnLayout {
                    id: columnLayout
                    Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter

                    BusyIndicator {
                        id: busyConnectIndicator
                        opacity: 1
                        visible: false
                        Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                    }

                    Label {
                        id: txtInfoConnecting
                        text: qsTr("Label")
                        visible: false
                    }
                }
            }

            Button {
                id: btnSendData
                text: qsTr("Verbindung prüfen")
                implicitWidth: 250
                Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                transformOrigin: Item.Center
                onClicked: {
                    globalUserData.ipAddr = txtIPAddress.text
                    globalUserData.conPort = spBoxPort.value
                    if (userInt.startSendingData(txtUserName.text, txtPassWord.text) > 0) {
                        btnSendData.enabled = false
                        busyConnectIndicator.visible = true;
                        txtInfoConnecting.text = "Verbinde ..."
                        txtInfoConnecting.visible = true;
                    }
                }
            }
        }
    }

    UserInterface {
        id: userInt
        globalData: globalUserData
        onNotifyConnectionFinished : {
            btnSendData.enabled = true
            busyConnectIndicator.visible = false;
//            console.log("UserLogin return value: " + result)
            if (result === 1) {
                btnSendData.background.color = "green"
                txtInfoConnecting.text = "Verbindung erfolgreich"
            }
            else {
                btnSendData.background.color = "red"
                if (result === -3)  {  // timeout
                    txtInfoConnecting.text = "Fehler: keine Antwort"
                }
                else {
                    txtInfoConnecting.text = "Verbindungsdaten fehlerhaft"
                }
            }
        }
        onNotifyVersionRequestFinished : {
//            console.log("version request: " + result + " - " + msg);
            if (result === 5) {
                versionDialogTextUpdate.text = msg;
                versionDialog.open()
            }
        }
    }

    Dialog {
        id: versionDialog
        modal: true
        focus: true
        title: "Neue Version"
        x: (flickable.width - width) / 2
        y: flickable.height / 6
        width: Math.min(flickable.width, flickable.height) / 3 * 2
        contentHeight: versionColumn.height

        Column {
            id: versionColumn
            spacing: 20

            Label {
                width: versionDialog.availableWidth
                text: "Es gibt eine neue Version von StamOrga"
                wrapMode: Label.Wrap
                font.pixelSize: 12
            }

            Label {
                id: versionDialogTextUpdate
                width: versionDialog.availableWidth
                text: ""
                wrapMode: Label.Wrap
                font.pixelSize: 12
            }
        }
    }

    ScrollIndicator.vertical: ScrollIndicator { }
}



