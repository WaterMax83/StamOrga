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
    property alias txtInfoConnecting: txtInfoConnecting
    property alias busyConnectIndicator: busyConnectIndicator

    ColumnLayout {
        id: mainColumnLayout
        spacing: 35
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
            implicitWidth: 250
            Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
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
            spacing: 5
            Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter

            Label {
                id: label1
                text: qsTr("Benutzername")
                Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
            }

            TextField {
                id: txtUserName
                text: qsTr("")
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
        }
    }
}
