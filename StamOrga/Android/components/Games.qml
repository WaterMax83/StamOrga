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
import QtGraphicalEffects 1.0

import com.watermax.demo 1.0

Rectangle {
    id: mainRectangleGame
    property var m_gamePlayItem

    signal clickedCurrentGame(var sender)
    signal pressedAndHoldCurrentGame(var sender)

    width: parent.width
    height: childrenRect.height * 1.05
    property color gradColorStart: "#105050"
//    property color colorTest: "#f30707"
    property color gradColorStop: "#509090"
    gradient: Gradient {
        GradientStop {
            position: 0
            color: gradColorStart
        }

        GradientStop {
            position: 0.9
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
            onPressAndHold: {
                mainRectangleGame.pressedAndHoldCurrentGame(m_gamePlayItem);
            }
        }

        RowLayout {
            id: layoutDateLine

            anchors.right: parent.right
            anchors.left: parent.left
            Layout.preferredWidth: columnLayout.width
            Layout.fillWidth: true
            spacing: 5

            Text {
                id: labelLineDate
                text: qsTr("Date")
                leftPadding: 5
                topPadding: 3
                color: "white"
                font.pixelSize: 12
                Layout.alignment: Qt.AlignLeft | Qt.AlignHCenter
            }

            Text {
                id: labelLineTimeNotFixed
                text: "<i>(nicht terminiert)</i>"
                textFormat: Text.RichText
                font.pixelSize: 12
                color: "white"
                leftPadding: 5
                topPadding: 3
                Layout.fillWidth: true
                Layout.alignment: Qt.AlignLeft | Qt.AlignHCenter
                visible: false
            }
        }

        RowLayout {
            id: layoutWhatLine

            anchors.right: parent.right
            anchors.left: parent.left
            Layout.preferredWidth: columnLayout.width
            Layout.fillWidth: true
            spacing: 5

            Text {
                id: labelLineWhat
                text: qsTr("What")
                leftPadding: 5
                color: "white"
                font.pixelSize: 12
                Layout.fillWidth: true
                Layout.alignment: Qt.AlignLeft | Qt.AlignHCenter
            }
            Image {
                id: imageMeetingIsPresent
                anchors.top : parent.top
                Layout.rightMargin: acceptedMeetingItem.visible ? 0 : 5
                anchors.topMargin: 3
                Layout.alignment: Qt.AlignRight
                Layout.preferredWidth: labelLineDate.height / 1.2
                Layout.preferredHeight: labelLineDate.height / 1.2
                source: "../images/place.png";
                visible: false
            }


            RowLayout {
                id: acceptedMeetingItem
                visible: false
                spacing: 0
                Layout.rightMargin: interestMeetingItem.visible ? 0 : freeTicketsItem.visible ? 10 : 5
                Text {
                    id: labelAcceptedMeeting
                    topPadding: 3
                    color: "white"
                    font.pixelSize: 12
                    Layout.alignment: Qt.AlignRight
                }

                Item {
                    Layout.preferredHeight: labelAcceptedMeeting.height / 1.2
                    Layout.preferredWidth: labelAcceptedMeeting.height / 1.2
                    anchors.top : parent.top
                    anchors.right: parent. right

                    anchors.topMargin: 3
                    Layout.alignment: Qt.AlignRight
                    Image {
                        id: imageAccpetedMeeting
                        anchors.fill: parent
                        source: "../images/done.png";
                    }
                    ColorOverlay {
                        anchors.fill: imageAccpetedMeeting
                        source: imageAccpetedMeeting
                        color: "green"
                    }
                }
            }

            RowLayout {
                id: interestMeetingItem
                visible: false
                spacing: 0
                Layout.rightMargin: freeTicketsItem.visible ? 10 : 5
                Text {
                    id: labelInterestMeeting
                    topPadding: 3
                    color: "white"
                    font.pixelSize: 12
                    Layout.alignment: Qt.AlignRight
                }

                Item {
                    Layout.preferredHeight: labelInterestMeeting.height / 1.2
                    Layout.preferredWidth: labelInterestMeeting.height / 1.2
                    anchors.top : parent.top
                    anchors.right: parent. right

                    anchors.topMargin: 3
                    Layout.alignment: Qt.AlignRight
                    Image {
                        id: imageInterestMeeting
                        anchors.fill: parent
                        source: "../images/help.png";
                    }
                    ColorOverlay {
                        anchors.fill: imageInterestMeeting
                        source: imageInterestMeeting
                        color: "orange"
                    }
                }
            }

            RowLayout {
                id: freeTicketsItem
                visible: false
                spacing: 5
                Text {
                    id: labelFreeTickets
                    topPadding: 3
                    color: "white"
                    font.pixelSize: 12
                    Layout.alignment: Qt.AlignRight
                }

                Rectangle {
                    anchors.top : parent.top
                    anchors.right: parent. right
                    anchors.rightMargin: 5
                    anchors.topMargin: 5
                    Layout.alignment: Qt.AlignRight
                    width: labelFreeTickets.height / 1.5
                    height: labelFreeTickets.height / 1.5
                    radius: width * 0.5
                    color: "green"
                }
            }
        }

        Text {
            id: labelLineHome
            text: qsTr("Home")
            leftPadding: 5
            topPadding: 5
            font.bold: true
            color: "white"
            font.pixelSize: 12
            Layout.fillWidth: true
            Layout.alignment: Qt.AlignLeft | Qt.AlignHCenter
        }

        RowLayout {
            id: layoutExplicitGame

            anchors.right: parent.right
            anchors.left: parent.left
            Layout.preferredWidth: columnLayout.width
            Layout.fillWidth: true

            Text {
                id: labelLineAway
                text: qsTr("Away")
                leftPadding: 5
                font.bold: true
                color: "white"
                font.pixelSize: 12
                Layout.fillWidth: true
                Layout.alignment: Qt.AlignLeft | Qt.AlignHCenter
            }

            Text {
                id: labelLineScore
                text: qsTr("")
                font.bold: true
                color: "white"
                font.pixelSize: 12
                rightPadding: layoutExplicitGame.width / 100 * 30
                Layout.alignment: Qt.AlignRight | Qt.AlignHCenter
            }
        }


    }

    function showGamesInfo(gamePlayItem) {
        if (gamePlayItem !== null) {
            m_gamePlayItem = gamePlayItem
            labelLineDate.text = gamePlayItem.timestampReadableLine();
            labelLineWhat.text = gamePlayItem.getCompetitionLine() + gamePlayItem.competition
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
                return;
            } else if (gamePlayItem.isGameRunning()) {
                mainRectangleGame.gradColorStart = "#f30707"
                mainRectangleGame.gradColorStop = "#ff4747"
            } else {
                var comp = gamePlayItem.competitionValue();
                if (comp === 4) { // dfb pokal
                    mainRectangleGame.gradColorStart = "#105010"
                    mainRectangleGame.gradColorStop = "#509050"
                }else if (comp === 5) { // badischer pokal
                    mainRectangleGame.gradColorStart = "#103050"
                    mainRectangleGame.gradColorStop = "#507090"
                } else if (comp === 6) { // TestSpiel
                    mainRectangleGame.gradColorStart = "#101050"
                    mainRectangleGame.gradColorStop = "#905090"
                }
            }

            var fixed = gamePlayItem.timeFixed
//            console.log("Fixed = " + fixed);
            if (!gamePlayItem.timeFixed)
                labelLineTimeNotFixed.visible = true

            var meetingPresent = gamePlayItem.getMeetingInfo();
            if (meetingPresent > 0) {
                imageMeetingIsPresent.visible = true
                var acceptedMeeting = gamePlayItem.getAcceptedMeetingCount();
                if (acceptedMeeting > 0) {
                    acceptedMeetingItem.visible = true;
                    labelAcceptedMeeting.text = acceptedMeeting;
                }
                var interestMeeting = gamePlayItem.getInterestedMeetingCount();
                if (interestMeeting > 0) {
                    interestMeetingItem.visible = true;
                    labelInterestMeeting.text = interestMeeting;
                }
            }

            if (!gamePlayItem.isGameASeasonTicketGame())
                return;

            var freeTickets = gamePlayItem.getFreeTickets();
            if (freeTickets > 0) {
                freeTicketsItem.visible = true;
                labelFreeTickets.text = freeTickets;
            } else {
                freeTicketsItem.visible = false;
            }

        }
    }


}







