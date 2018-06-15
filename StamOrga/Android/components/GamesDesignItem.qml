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

import QtQuick 2.9
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.2
import QtGraphicalEffects 1.0

import QtQuick.Controls.Material 2.2

import com.watermax.demo 1.0

import "../components" as MyComponents

Item {
    width: parent.width
    height: mainGameLayout.height
//    color: "#36454f"

    property var m_gamePlayItem

    signal clickedCurrentGame(var sender)
    signal pressedAndHoldCurrentGame(var sender)

    MouseArea {
        anchors.fill: parent
        onClicked: {
            clickedCurrentGame(m_gamePlayItem);
        }
        onPressAndHold: {
            pressedAndHoldCurrentGame(m_gamePlayItem);
        }
    }

    ColumnLayout {
        id: mainGameLayout
        width: parent.width
        spacing: 0
        Rectangle {
            color: Material.hintTextColor
            height: 1
            Layout.fillWidth: true
        }

        RowLayout {
//            id: mainGameLayout
            width: parent.width

            Item {
                anchors.top: parent.top
                anchors.left: parent.left
                id: imageItemGameInfo
                width: 50
                height: 50
                Image {
                    id: imageGameInfo
                    sourceSize: Qt.size(parent.width, parent.height)
                    fillMode: Image.PreserveAspectFit
                    //                Layout.alignment: Qt.AlignRight
                    source: "../images/info.png"

                }
            }

            Item {
                id: columnItem
                Layout.fillWidth: true
                Layout.leftMargin: 5
                height: columnItemLayout.height
                ColumnLayout {
                    id: columnItemLayout
                    width: parent.width
                    spacing: 0

                    RowLayout {
                        Text {
                            id: labelLineDate
                            anchors.left: parent.left
                            text: qsTr("Date")
                            color: "#B0BEC5"
                            font.pixelSize: 14
//                            Layout.alignment: Qt.AlignLeft | Qt.AlignHCenter
                        }

                        Text {
                            id: labelLineTimeNotFixed
                            text: "<i>(nicht terminiert)</i>"
                            textFormat: Text.RichText
                            font.pixelSize: 12
                            color: "grey"
                            leftPadding: 5
//                            Layout.alignment: Qt.AlignLeft | Qt.AlignHCenter
                            visible: false
                        }
                    }
                    RowLayout {
                        Text {
                            anchors.left: parent.left
                            id: labelLineHome
                            text: qsTr("Home")
                            font.bold: true
                            color: "white"
                            font.pixelSize: 14
                        }
                    }
                    RowLayout {
                        Text {
                            id: labelLineAway
                            anchors.left: parent.left
                            Layout.fillWidth: true
                            text: qsTr("Away")
                            font.bold: true
                            color: "white"
                            font.pixelSize: 14
                        }
                        Text {
                            id: labelLineScore
                            anchors.right: parent.right
                            text: qsTr("")
                            font.bold: true
                            color: "white"
                            font.pixelSize: 14
//                            Layout.rightMargin: parent.width / 100 * 30
//                            Layout.alignment: Qt.AlignRight // | Qt.AlignHCenter
                        }
                    }
                    RowLayout {
                        Text {
                            anchors.left: parent.left
                            text: ""
                            color: "white"
                            font.pixelSize: 14
                        }
                    }
                }
            }
        }
    }

    function showGamesInfo(gamePlayItem) {
        if (gamePlayItem !== null) {
            m_gamePlayItem = gamePlayItem
            labelLineDate.text = gamePlayItem.timestampReadableLine();
//            labelLineWhat.text = gamePlayItem.getCompetitionLine() + gamePlayItem.competition
            labelLineHome.text = gamePlayItem.home;
            if (gamePlayItem.home === "KSC")
                labelLineHome.font.letterSpacing = 2
            labelLineAway.text = gamePlayItem.away;
            if (gamePlayItem.away === "KSC")
                labelLineAway.font.letterSpacing = 2
            labelLineScore.text = gamePlayItem.score;

            if (gamePlayItem.isGameInPast()) {
//                mainRectangleGame.gradColorStart = "#505050"
//                mainRectangleGame.gradColorStop = "#909090"
//                layoutCurrentInfoItem.visible = false;
                return;
            } else if (gamePlayItem.isGameRunning()) {
//                mainRectangleGame.gradColorStart = "#f30707"
//                mainRectangleGame.gradColorStop = "#ff4747"
            } else {
//                var comp = gamePlayItem.competitionValue();
//                if (comp === 4) { // dfb pokal
//                    mainRectangleGame.gradColorStart = "#105010"
//                    mainRectangleGame.gradColorStop = "#509050"
//                }else if (comp === 5) { // badischer pokal
//                    mainRectangleGame.gradColorStart = "#103050"
//                    mainRectangleGame.gradColorStop = "#507090"
//                } else if (comp === 6) { // TestSpiel
//                    mainRectangleGame.gradColorStart = "#101050"
//                    mainRectangleGame.gradColorStop = "#905090"
//                }
            }

//            var favIndex = gDataGameUserData.getFavoriteGameIndex(gamePlayItem.index);
//            if (favIndex <= 0) {
//                mainRectangleGame.border.color = "grey";
//                mainRectangleGame.border.width = 2;
//            }
//            else{
//                mainRectangleGame.border.color = "orange";
//                mainRectangleGame.border.width = 3;
//            }

//            var eventCount = gamePlayItem.event;
//            if (eventCount > 0) {
//                eventIndicator.disableVisibility = false;
//                eventIndicator.eventCount = eventCount;
//            } else
//                eventIndicator.disableVisibility = true;

            if (!gamePlayItem.timeFixed)
                labelLineTimeNotFixed.visible = true

//            showTripInfo(gamePlayItem);

//            showMeetingInfo(gamePlayItem);

//            showTicketInfo(gamePlayItem);
        }
    }
}
