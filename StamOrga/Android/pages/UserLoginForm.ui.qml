import QtQuick 2.7
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.0
import QtQuick.Extras 1.4
import QtQuick.Window 2.2

import "../components" as MyComponents

Pane {
    id: mainPane

    ColumnLayout {
        id: mainColumnLayout
        anchors.right: parent.right
        anchors.left: parent.left
        ColumnLayout {
            id: columnLayoutBusyInfo
            spacing: 5
            Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter

            BusyIndicator {
                id: busyLoadingIndicator
                visible: true
                Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
            }

            Label {
                id: txtInfoLoading
                visible: true
                Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
            }
        }
        ColumnLayout {

            Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
        }
    }


}
