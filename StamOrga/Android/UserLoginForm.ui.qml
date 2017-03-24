import QtQuick 2.7
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.0

Item {
    id: item1
    property alias btnSendData: btnSendData
    property alias txtIPAddress: txtIPAddress
    property alias txtUserName: txtUserName
    property alias spBoxPort: spBoxPort

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

        TextField {
            id: txtUserName
            text: qsTr("")
            Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
        }
        Button {
            id: btnSendData
            text: qsTr("Send Data")
            Layout.fillHeight: false
            Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
            transformOrigin: Item.Center
        }

    }

}
