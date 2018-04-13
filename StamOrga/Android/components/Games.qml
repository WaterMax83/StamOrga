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

import com.watermax.demo 1.0

import "../components" as MyComponents

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

    MyComponents.EventIndicator {
        id: eventIndicator
        disableVisibility: true
        eventCount : 1
        itemSize: labelLineDate.height * 2
    }

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
                font.pixelSize: 14
                Layout.alignment: Qt.AlignLeft | Qt.AlignHCenter
            }

            Text {
                id: labelLineTimeNotFixed
                text: "<i>(nicht terminiert)</i>"
                textFormat: Text.RichText
                font.pixelSize: 14
                color: "white"
                leftPadding: 5
                topPadding: 3
                Layout.fillWidth: true
                Layout.alignment: Qt.AlignLeft | Qt.AlignHCenter
                visible: false
            }
        }

        RowLayout {
            anchors.right: parent.right
            anchors.left: parent.left
            Layout.preferredWidth: columnLayout.width
            Layout.fillWidth: true

            Text {
                id: labelLineWhat
                text: qsTr("What")
                leftPadding: 5
                color: "white"
                font.pixelSize: 14
                Layout.fillWidth: true
                Layout.alignment: Qt.AlignLeft | Qt.AlignHCenter
            }

            RowLayout {
                Layout.alignment: Qt.AlignRight
                Layout.rightMargin: 5
                spacing: 5


                Image {
                    id: imageTripIsPresent
                    Layout.preferredWidth: labelLineDate.height / 1.2
                    Layout.preferredHeight: labelLineDate.height / 1.2
                    source: "../images/train.png";
                    visible: false
                }


                RowLayout {
                    id: acceptedTripItem
                    visible: false
                    spacing: 1
                    Text {
                        id: labelAcceptedTrip
                        color: "white"
                        font.pixelSize: 14
                    }

                    Rectangle {
                        width: labelAcceptedTrip.height / 1.5
                        height: labelAcceptedTrip.height / 1.5
                        radius: width * 0.5
                        color: "green"
                        Image {
                            anchors.fill: parent
                            source: "../images/done.png";
                        }
                    }
                }

                RowLayout {
                    id: interestTripItem
                    visible: false
                    spacing: 1
                    Text {
                        id: labelInterestTrip
                        color: "white"
                        font.pixelSize: 14
                        Layout.alignment: Qt.AlignRight
                    }

                    Rectangle {
                        width: labelInterestTrip.height / 1.5
                        height: labelInterestTrip.height / 1.5
                        radius: width * 0.5
                        color: "orange"
                        Image {
                            anchors.fill: parent
                            source: "../images/help.png";
                        }
                    }
                }
            }
        }

        RowLayout {
            anchors.right: parent.right
            anchors.left: parent.left
            Layout.preferredWidth: parent.width
            Layout.fillWidth: true

            Text {
                id: labelLineHome
                text: qsTr("Home")
                leftPadding: 5
                font.bold: true
                color: "white"
                font.pixelSize: 14
                Layout.fillWidth: true
                Layout.alignment: Qt.AlignLeft | Qt.AlignHCenter
            }

            RowLayout {
                Layout.alignment: Qt.AlignRight
                Layout.rightMargin: 5
                spacing: 5


                Image {
                    id: imageMeetingIsPresent
                    Layout.preferredWidth: labelLineDate.height / 1.2
                    Layout.preferredHeight: labelLineDate.height / 1.2
                    source: "../images/place.png";
                    visible: false
                }


                RowLayout {
                    id: acceptedMeetingItem
                    visible: false
                    spacing: 1
                    Text {
                        id: labelAcceptedMeeting
                        color: "white"
                        font.pixelSize: 14

                    }

                    Rectangle {
                        width: labelAcceptedMeeting.height / 1.5
                        height: labelAcceptedMeeting.height / 1.5
                        radius: width * 0.5
                        color: "green"
                        Image {
                            anchors.fill: parent
                            source: "../images/done.png";
                        }
                    }
                }

                RowLayout {
                    id: interestMeetingItem
                    visible: false
                    spacing: 1
                    Text {
                        id: labelInterestMeeting
                        color: "white"
                        font.pixelSize: 14
                    }

                    Rectangle {
                        width: labelInterestMeeting.height / 1.5
                        height: labelInterestMeeting.height / 1.5
                        radius: width * 0.5
                        color: "orange"
                        Image {
                            anchors.fill: parent
                            source: "../images/help.png";
                        }
                    }
                }
            }
        }

        RowLayout {
            anchors.right: parent.right
            anchors.left: parent.left
            Layout.preferredWidth: parent.width
            Layout.fillWidth: true

            Text {
                id: labelLineAway
                text: qsTr("Away")
                leftPadding: 5
                font.bold: true
                color: "white"
                font.pixelSize: 14
                Layout.fillWidth: true
                Layout.alignment: Qt.AlignLeft // | Qt.AlignHCenter
            }

            Text {
                id: labelLineScore
                text: qsTr("")
                font.bold: true
                color: "white"
                font.pixelSize: 14
                rightPadding: parent.width / 100 * 30
                Layout.alignment: Qt.AlignRight // | Qt.AlignHCenter
            }

            RowLayout {
                Layout.alignment: Qt.AlignRight
                Layout.rightMargin: 5
                spacing: 5
                Image {
                    id: imageTicketItems
                    Layout.preferredHeight: labelFreeTickets.height / 1.2
                    Layout.preferredWidth: labelFreeTickets.height / 1.2
                    visible: false
                    source: "../images/card.png";
                }
                RowLayout {
                    id: itemFreeTickets
                    visible: false
                    spacing: 1
                    Text {
                        id: labelFreeTickets
                        color: "white"
                        font.pixelSize: 14
                    }

                    Rectangle {
                        width: labelFreeTickets.height / 1.5
                        height: labelFreeTickets.height / 1.5
                        radius: width * 0.5
                        color: "green"
                    }
                }

                Item {
                    id: itemTicketBookmark
                    Layout.preferredHeight: labelFreeTickets.height / 1.2
                    Layout.preferredWidth: labelFreeTickets.height / 1.2
                    Layout.leftMargin: 6    // to get offset same as line above
                    visible: false

                    Image {
                        id: imageItemTicketBookmark
                        anchors.fill : parent
                        source: "../images/bookmark.png";
                    }
                    ColorOverlay {
                        anchors.fill: imageItemTicketBookmark
                        source: imageItemTicketBookmark
                        color: "yellow"
                    }
                }

                Item {
                    id: itemTicketShare
                    Layout.preferredHeight: labelFreeTickets.height / 1.2
                    Layout.preferredWidth: labelFreeTickets.height / 1.2
                    Layout.leftMargin: 6    // to get offset same as line above
                    visible: false

                    Image {
                        id: imageItemTicketShare
                        anchors.fill : parent
                        source: "../images/share.png";
                    }
//                    ColorOverlay {
//                        anchors.fill: imageItemTicketShare
//                        source: imageItemTicketShare
//                        color: "green"
//                    }
                }
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

            var favIndex = gDataGameUserData.getFavoriteGameIndex(gamePlayItem.index);
            if (favIndex <= 0) {
                mainRectangleGame.border.color = "grey";
                mainRectangleGame.border.width = 2;
            }
            else{
                mainRectangleGame.border.color = "orange";
                mainRectangleGame.border.width = 3;
            }

            var eventCount = gamePlayItem.event;
            if (eventCount > 0) {
                eventIndicator.disableVisibility = false;
                eventIndicator.eventCount = eventCount;
            } else
                eventIndicator.disableVisibility = true;

            var fixed = gamePlayItem.timeFixed
            if (!gamePlayItem.timeFixed)
                labelLineTimeNotFixed.visible = true


            showTripInfo(gamePlayItem);

            showMeetingInfo(gamePlayItem);

            showTicketInfo(gamePlayItem);

        }
    }

    function showTripInfo(gamePlayItem) {
        acceptedTripItem.visible = false;
        interestTripItem.visible = false;
        imageTripIsPresent.visible = false;
        var tripPresend = gamePlayItem.getTripInfo();
        if (tripPresend > 0) {
            imageTripIsPresent.visible = true
            var acceptedTrip = gamePlayItem.getAcceptedTripCount();
            if (acceptedTrip > 0) {
                acceptedTripItem.visible = true;
                labelAcceptedTrip.text = acceptedTrip;
            }
            var interestTrip = gamePlayItem.getInterestedTripCount();
            if (interestTrip > 0) {
                interestTripItem.visible = true;
                labelInterestTrip.text = interestTrip;
            }
        }
    }

    function showMeetingInfo(gamePlayItem) {
        acceptedMeetingItem.visible = false;
        interestMeetingItem.visible = false;
        imageMeetingIsPresent.visible = false;
        var meetingPresent = gamePlayItem.getMeetingInfo();
        if (meetingPresent > 0) {
            imageMeetingIsPresent.visible = true
            if (gamePlayItem.getAcceptedMeetingCount() > 0) {
                acceptedMeetingItem.visible = true;
                labelAcceptedMeeting.text = gamePlayItem.getAcceptedMeetingCount();
            }

            if (gamePlayItem.getInterestedMeetingCount() > 0) {
                interestMeetingItem.visible = true;
                labelInterestMeeting.text = gamePlayItem.getInterestedMeetingCount();
            }
        }
    }

    function showTicketInfo(gamePlayItem) {

        if (!gamePlayItem.isGameASeasonTicketGame())
            return;

        var ticketIndex = gDataGameUserData.getTicketGameIndex(gamePlayItem.index);
            if (ticketIndex === 2) {
                itemTicketShare.visible = true;
                itemTicketBookmark.visible = false;
            } else if (ticketIndex === 3) {
                itemTicketShare.visible = false;
                itemTicketBookmark.visible = true;
            }else {
                itemTicketShare.visible = false;
                itemTicketBookmark.visible = false;
            }

        var freeTickets = gamePlayItem.getFreeTickets();
        if (freeTickets > 0) {
            itemFreeTickets.visible = true;
            labelFreeTickets.text = freeTickets;
        } else {
            itemFreeTickets.visible = false;
        }

        if (freeTickets > 0 || ticketIndex > 1)
            imageTicketItems.visible = true;
        else
            itemFreeTickets.visible = false;
    }


}







