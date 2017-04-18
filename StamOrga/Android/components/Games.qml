import QtQuick 2.7
import QtQuick.Controls 2.1
import QtQuick.Layouts 1.0

import com.watermax.demo 1.0

Rectangle {
    id: mainRectangle
    property var m_gamePlayItem

    width: parent.width
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

            Layout.preferredWidth: columnLayout.width
            Label {
                id: labelLineHome
                width: contentWidth
                text: qsTr("Home")
                leftPadding: 5
////                Layout.fillWidth: true
//                Layout.minimumWidth: layoutExplicitGame / 30 * 10
//                Layout.preferredWidth: layoutExplicitGame / 30 * 10
//                Layout.maximumWidth: layoutExplicitGame / 30 * 10
                Layout.alignment: Qt.AlignLeft | Qt.AlignHCenter
            }

            Label {
                id: labelLineHyphen
                width: contentWidth
                text: qsTr(" - ")
                Layout.alignment: Qt.AlignHCenter
            }

            Label {
                id: labelLineAway
                width: contentWidth
                text: qsTr("Away")
                Layout.alignment: Qt.AlignHCenter
            }

            Label {
                id: labelLineScore
                width: contentWidth
                text: qsTr("")
                rightPadding: 20
                Layout.alignment: Qt.AlignRight | Qt.AlignHCenter
            }
        }
    }

    function showGamesInfo(gamePlayItem) {
        if (gamePlayItem !== null) {
            m_gamePlayItem = gamePlayItem
            labelLine1.text = gamePlayItem.timestamp;
            labelLine2.text = gamePlayItem.getCompetitionIndex() + " - " + gamePlayItem.competition
//            labelLine3.text = gamePlayItem.home + " - " + gamePlayItem.away + "\t" + gamePlayItem.score
            labelLineHome.text = gamePlayItem.home;
            labelLineAway.text = gamePlayItem.away;
            labelLineScore.text = gamePlayItem.score;
        }
    }
}


