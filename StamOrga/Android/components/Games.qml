import QtQuick 2.7
import QtQuick.Controls 2.1
import QtQuick.Layouts 1.0

import com.watermax.demo 1.0

Rectangle {
    id: mainRectangle
//    property alias gamePlayItem: gamePlayItem
//    property var gamePlayItem

    width: parent.width
//    width: 100
    height: 50
    color: "red"
//    Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
    ColumnLayout {
        id: columnLayout
        width: mainRectangle.width
        spacing: 3
        Label {
            id: labelLine1
            text: qsTr("Benutzername")
            Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
        }

        Label {
            id: labelLine2
            text: qsTr("Benutzername")
            Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
        }
    }

    function showGamesInfo(gamePlayItem) {
//        console.log("Inside Games")
        labelLine1.text = gamePlayItem.home
        labelLine2.text = gamePlayItem.away
    }

//    Component {
//        id: test
//    }
//    GamePlay*{

//    }
}


