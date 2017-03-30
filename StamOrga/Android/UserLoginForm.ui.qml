import QtQuick 2.7
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.0
import QtQuick.Extras 1.4
import QtQuick.Window 2.2

Item {
    id: item1
    property alias btnSendData: btnSendData
    property alias txtIPAddress: txtIPAddress
    property alias txtUserName: txtUserName
    property alias spBoxPort: spBoxPort
    property alias txtPassWord: txtPassWord
    property alias busyConnectIndicator: busyConnectIndicator
    property alias txtInfoConnecting: txtInfoConnecting

    ColumnLayout {
        id: columnLayout1
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 20
        anchors.right: parent.right
        anchors.rightMargin: 0
        anchors.left: parent.left
        anchors.leftMargin: 0
        anchors.top: parent.top
        anchors.topMargin: 20

        TextField {
            id: txtIPAddress
            text: "140.80.61.57"
            Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
            Layout.fillHeight: false
            Layout.fillWidth: false
            placeholderText: qsTr("Text Field")
        }

        SpinBox {
            id: spBoxPort
            to: 100000
            from: 1
            value: 55000
            Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
        }

        ColumnLayout {
            id: columnLayout2
            width: 100
            height: 100
            Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter

            Label {
                id: label1
                text: qsTr("Benutzername")
                Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
            }

            TextField {
                id: txtUserName
                text: qsTr("")
                Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
            }
        }

        ColumnLayout {
            id: columnLayout3
            width: 100
            height: 100
            Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter

            Label {
                id: label2
                text: qsTr("Passwort")
                Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
            }

            TextField {
                id: txtPassWord
            }
        }

        RowLayout {
            id: rowLayout
            width: 100
            height: 50
            Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter

            Column {
                id: space
                width: 0
                height: 60
            }

            ColumnLayout {
                id: columnLayout
                width: 100
                height: 100
                Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter

                BusyIndicator {
                    id: busyConnectIndicator
                    opacity: 1
                    visible: false
                    Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                }

                Text {
                    id: txtInfoConnecting
                    text: qsTr("Text")
                    visible: false
                    Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                    font.pixelSize: 12
                }
            }
        }

        Button {
            id: btnSendData
            width: 250
            text: qsTr("Verbindung prüfen")
            Layout.preferredWidth: 150
            Layout.fillWidth: false
            Layout.fillHeight: false
            Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
            transformOrigin: Item.Center
        }
    }
}
