import QtQuick 2.7
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.0
import QtQuick.Extras 1.4
import QtQuick.Window 2.2

import "../components" as MyComponents

Rectangle {
    id: mainRectangle
    property var m_gamePlayItem

    width: 200
    height: childrenRect.height * 1.1
    color: "#85C4BE"
    radius: 8
    border.color: "grey"
    border.width: 2
    ColumnLayout {
        id: columnLayout
        width: mainRectangle.width
        spacing: 1
        Label {
            id: labelLine1
            text: qsTr("Date")
            leftPadding: 5
            topPadding: 5
            Layout.alignment: Qt.AlignLeft | Qt.AlignHCenter
        }

        Label {
            id: labelLine2
            text: qsTr("What")
            leftPadding: 5
            Layout.alignment: Qt.AlignLeft | Qt.AlignHCenter
        }

        RowLayout {
            id: layoutExplicitGame
            spacing: 0
            Layout.fillWidth: true

            Layout.minimumWidth: columnLayout.width
            Layout.preferredWidth: columnLayout.width
            //            columns: 4
            Label {
                id: labelLineHome
                width: contentWidth
                text: qsTr("Home")
                leftPadding: 5
                //                Layout.fillWidth: true
                Layout.minimumWidth: layoutExplicitGame / 30 * 10
                Layout.preferredWidth: layoutExplicitGame / 30 * 10
                Layout.maximumWidth: layoutExplicitGame / 30 * 10
                Layout.alignment: Qt.AlignLeft | Qt.AlignHCenter
            }

            Label {
                id: labelLineHyphen
                width: contentWidth
                text: qsTr(" - ")
                //                Layout.fillWidth: true
                Layout.minimumWidth: layoutExplicitGame / 30 * 1
                Layout.preferredWidth: layoutExplicitGame / 30 * 1
                Layout.maximumWidth: layoutExplicitGame / 30 * 1
                Layout.alignment: Qt.AlignHCenter
            }

            Label {
                id: labelLineAway
                width: contentWidth
                text: qsTr("AwayBigger")
                //                Layout.fillWidth: true
                Layout.minimumWidth: layoutExplicitGame / 30 * 10
                Layout.preferredWidth: layoutExplicitGame / 30 * 10
                Layout.maximumWidth: layoutExplicitGame / 30 * 10
                Layout.alignment: Qt.AlignHCenter
            }

            Label {
                id: labelLineScore
                width: contentWidth
                text: qsTr("0:0")
                rightPadding: 10
                //                Layout.fillWidth: true
                Layout.minimumWidth: layoutExplicitGame / 30 * 5
                Layout.preferredWidth: layoutExplicitGame / 30 * 5
                Layout.maximumWidth: layoutExplicitGame / 30 * 5
                Layout.alignment: Qt.AlignRight | Qt.AlignVCenter
            }
        }
    }
}
