/*
*	This file is part of StamOrga
*   Copyright (C) 2017 Markus Schneider
*
*	This program is free software; you can redistribute it and/or modify
*   it under the terms of the GNU General Public License as published by
*   the Free Software Foundation; either version 3 of the License, or
*   (at your option) any later version.
*
*	StamOrga is distributed in the hope that it will be useful,
*    but WITHOUT ANY WARRANTY; without even the implied warranty of
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*    GNU General Public License for more details.

*    You should have received a copy of the GNU General Public License
*    along with StamOrga.  If not, see <http://www.gnu.org/licenses/>.
*/

import QtQuick 2.7
import QtQuick.Controls 2.1
import QtQuick.Layouts 1.0

import com.watermax.demo 1.0

Rectangle {
    id: mainRectangleGame
    property var m_gamePlayItem

    signal clickedCurrentGame(var sender)

    width: parent.width
    height: childrenRect.height * 1.05
    property color gradColorStart: "#105050"
    property color gradColorStop: "#509090"
    gradient: Gradient {
        GradientStop {
            position: 0
            color: gradColorStart
        }

        GradientStop {
            position: 0.5
            color: gradColorStop
        }
    }
    radius: 8
    border.color: "grey"
    border.width: 2
    ColumnLayout {
        id: columnLayout
        width: mainRectangleGame.width
        spacing: 1

        MouseArea {
            anchors.fill: parent
            onClicked: {
                mainRectangleGame.clickedCurrentGame(m_gamePlayItem);
            }
        }

        Label {
            id: labelLineDate
            text: qsTr("Date")
            leftPadding: 5
            topPadding: 3
            Layout.alignment: Qt.AlignLeft | Qt.AlignHCenter
        }

        Label {
            id: labelLineWhat
            text: qsTr("What")
            leftPadding: 5
            Layout.alignment: Qt.AlignLeft | Qt.AlignHCenter
        }

        Label {
            id: labelLineHome
            text: qsTr("Home")
            leftPadding: 5
            topPadding: 5
            Layout.fillWidth: true
            Layout.alignment: Qt.AlignLeft | Qt.AlignHCenter
        }

        RowLayout {
            id: layoutExplicitGame

            anchors.right: parent.right
            anchors.left: parent.left
            Layout.preferredWidth: columnLayout.width
            Layout.fillWidth: true

            Label {
                id: labelLineAway
                text: qsTr("Away")
                leftPadding: 5
                Layout.fillWidth: true
                Layout.alignment: Qt.AlignLeft | Qt.AlignHCenter
            }

            Label {
                id: labelLineScore
                text: qsTr("")
                rightPadding: layoutExplicitGame.width / 100 * 30
                Layout.alignment: Qt.AlignRight | Qt.AlignHCenter
            }
        }


    }

    function showGamesInfo(gamePlayItem) {
        if (gamePlayItem !== null) {
            m_gamePlayItem = gamePlayItem
            labelLineDate.text = gamePlayItem.timestamp;
            labelLineWhat.text = gamePlayItem.getCompetitionIndex() + " - " + gamePlayItem.competition
            labelLineHome.text = gamePlayItem.home;
            if (gamePlayItem.home === "KSC")
                labelLineHome.font.letterSpacing = 2
            labelLineAway.text = gamePlayItem.away;
            if (gamePlayItem.away === "KSC")
                labelLineAway.font.letterSpacing = 2
            labelLineScore.text = gamePlayItem.score;

            if (gamePlayItem.isGameInPast()) {
                mainRectangleGame.gradColorStart = "#505050"
                mainRectangleGame.gradColorStop = "#909090"
            }

        }
    }


}







