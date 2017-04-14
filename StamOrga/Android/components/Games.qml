import QtQuick 2.7
import QtQuick.Controls 2.1
import QtQuick.Layouts 1.0

import com.watermax.demo 1.0

Rectangle {
    id: mainRectangle
    property var m_gamePlayItem

    width: parent.width
    height: 50
    color: "#85C4BE"
    ColumnLayout {
        id: columnLayout
        width: mainRectangle.width
        spacing: 3
        Label {
            id: labelLine1
            text: qsTr("Date and what")
            Layout.alignment: Qt.AlignLeft | Qt.AlignHCenter
        }

        Label {
            id: labelLine2
            text: qsTr("Game")
            Layout.alignment: Qt.AlignLeft | Qt.AlignHCenter
        }
    }

    function showGamesInfo(gamePlayItem) {
        if (gamePlayItem !== null) {
            m_gamePlayItem = gamePlayItem
            labelLine1.text = gamePlayItem.timestamp + "\t" +  gamePlayItem.getCompetitionIndex() + " - " + gamePlayItem.competition
            labelLine2.text = gamePlayItem.home + " - " + gamePlayItem.away + "\t" + gamePlayItem.score
        }
    }
}


