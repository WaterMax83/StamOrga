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
    height: mainGameLayout.implicitHeight
    Layout.minimumHeight: mainGameLayout.height

    property bool showGameSeperator: true

    property var m_gamePlayItem

    signal clickedCurrentGame(var sender)
    signal pressedAndHoldCurrentGame(var sender)

    property color gradColorStart: "#105050"
    property color gradColorStop: "#509090"
    property int imageSize : 20
    property int marginBetweenImages : 3

    MouseArea {
        anchors.fill: parent
        onClicked: {
            clickedCurrentGame(m_gamePlayItem);
        }
        onPressAndHold: {
            pressedAndHoldCurrentGame(m_gamePlayItem);
        }
    }

    MyComponents.EventIndicator {
        id: eventIndicator
        disableVisibility: true
        eventCount : 1
        itemSize: imageSize * 2
        z : 1000
    }

    ColumnLayout {
        id: mainGameLayout
        anchors.fill : parent
        spacing: 3
        Rectangle {
            color: Material.hintTextColor
            height: 1
            visible: showGameSeperator
            Layout.fillWidth: true
        }

        RowLayout {
            width: parent.width
            Layout.minimumHeight: columnItem.height
            Layout.bottomMargin: 5

            ColumnLayout {
                Layout.alignment: Qt.AlignLeft | Qt.AlignTop
                Layout.leftMargin: 3
                Layout.topMargin: 3

                Rectangle {
                    id: itemCompetitionInfo
                    width: 50
                    height: 50
                    radius: width / 2
                    border.color: "grey"
                    border.width: 2
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
                    Image {
                        id: itemImageTimeLaps
                        height: 24
                        width: 24
                        source: "../images/timelaps.png"
                        anchors.right : parent.right
                        anchors.top: parent.top
                        anchors.topMargin: -3
                        anchors.rightMargin: -3
                    }
                    Text {
                        id: labelLineCompetition
                        text: "BL"
                        font.pixelSize: 22
                        font.bold: true
                        color: "#B0BEC5"
                        anchors.centerIn: parent
                    }
                }
            }

            Item {
                id: columnItem
                Layout.fillWidth: true
                Layout.leftMargin: 5
                Layout.minimumHeight: columnItemLayout.implicitHeight
                ColumnLayout {
                    id: columnItemLayout
                    width: parent.width
                    spacing: 0

                    RowLayout {
                        Text {
                            id: labelLineDate
                            Layout.alignment: Qt.AlignLeft
                            text: qsTr("Date")
                            color: "#B0BEC5"
                            font.pixelSize: 14
                        }

                        //                        Text {
                        //                            id: labelLineTimeNotFixed
                        //                            text: "<i>(nicht terminiert)</i>"
                        //                            textFormat: Text.RichText
                        //                            font.pixelSize: 12
                        //                            color: "grey"
                        //                            leftPadding: 5
                        //                            //                            Layout.alignment: Qt.AlignLeft | Qt.AlignHCenter
                        //                            visible: false
                        //                        }
                        Text {
                            text: " - "
                            visible: labelLineGameInfo.text.length > 0
                            font.pixelSize: 14
                            color: "#B0BEC5"
                        }
                        Text {
                            id: labelLineGameInfo
                            text: ""
                            font.pixelSize: 14
                            color: "#B0BEC5"
                        }
                    }
                    RowLayout {
                        Text {
//                            anchors.left: parent.left
                            Layout.fillWidth: true
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
                            Layout.fillWidth: true
                            text: qsTr("Away")
                            font.bold: true
                            color: "white"
                            font.pixelSize: 14
                        }
                        Text {
                            id: labelLineScore
                            Layout.alignment: Qt.AlignRight
                            Layout.rightMargin: 5
                            text: qsTr("")
                            font.bold: true
                            color: "white"
                            font.pixelSize: 14
                        }
                    }

                    GridLayout {
                        width: parent.width
                        Layout.fillWidth: true
                        columns: 3
                        columnSpacing: 0

                        GridLayout {
                            Layout.fillWidth: true
                            Layout.preferredWidth: parent.width / 3
                            columns: 10
                            columnSpacing: 0

                            Item {
                                Layout.column: 0
                                width: imageSize
                                height: imageSize
                                Image {
                                    id: imageTripIsPresent
                                    anchors.fill : parent
                                    source: "../images/train.png";
                                }
                            }

                            Text {
                                Layout.leftMargin: marginBetweenImages
                                Layout.column: 2
                                id: labelAcceptedTrip
                                color: "white"
                                font.pixelSize: 14
                            }

                            Rectangle {
                                id: acceptedTripItem
                                Layout.column: 3
                                width: imageSize / 1.5
                                height: imageSize / 1.5
                                radius: width * 0.5
                                color: "green"
                                Image {
                                    anchors.fill: parent
                                    source: "../images/done.png";
                                }
                            }

                            Text {
                                Layout.leftMargin: marginBetweenImages
                                Layout.column: 5
                                id: labelInterestTrip
                                color: "white"
                                font.pixelSize: 14
                            }

                            Rectangle {
                                id: interestTripItem
                                Layout.column: 6
                                width: imageSize / 1.5
                                height: imageSize / 1.5
                                radius: width * 0.5
                                color: "orange"
                                Image {
                                    anchors.fill: parent
                                    source: "../images/help.png";
                                }
                            }
                            Text { Layout.fillWidth: true }
                        }

                        GridLayout {
                            Layout.fillWidth: true
                            Layout.preferredWidth: parent.width / 3
                            columns: 10
                            columnSpacing: 0

                            Item {
                                Layout.column: 0
                                width: imageSize
                                height: imageSize
                                Image {
                                    id: imageMeetingIsPresent
                                    anchors.fill : parent
                                    source: "../images/place.png";
                                }
                            }

                            Text {
                                Layout.leftMargin: marginBetweenImages
                                Layout.column: 2
                                id: labelAcceptedMeeting
                                color: "white"
                                font.pixelSize: 14

                            }
                            Rectangle {
                                Layout.column: 3
                                id : acceptedMeetingItem
                                width: imageSize / 1.5
                                height: imageSize / 1.5
                                radius: width * 0.5
                                color: "green"
                                Image {
                                    anchors.fill: parent
                                    source: "../images/done.png";
                                }
                            }

                            Text {
                                Layout.leftMargin: marginBetweenImages
                                Layout.column: 5
                                id: labelInterestMeeting
                                color: "white"
                                font.pixelSize: 14
                            }
                            Rectangle {
                                Layout.column: 6
                                id : interestMeetingItem
                                width: imageSize / 1.5
                                height: imageSize / 1.5
                                radius: width * 0.5
                                color: "orange"
                                Image {
                                    anchors.fill: parent
                                    source: "../images/help.png";
                                }
                            }

                            Text { Layout.fillWidth: true }
                        }

                        GridLayout {
                            id: layoutCurrentInfoItem
                            Layout.fillWidth: true
                            Layout.preferredWidth: parent.width / 3
                            columns: 10
                            columnSpacing: 0

                            Item {
                                Layout.column: 0
                                width: imageSize
                                height: imageSize
                                Image {
                                    id: imageTicketItems
                                    anchors.fill : parent
                                    source: "../images/card.png";
                                }
                            }

                            Text {
                                Layout.leftMargin: marginBetweenImages
                                Layout.column: 2
                                id: labelFreeTickets
                                color: "white"
                                font.pixelSize: 14
                            }
                            Rectangle {
                                Layout.column: 3
                                id: itemFreeTickets
                                width: imageSize / 1.5
                                height: imageSize / 1.5
                                radius: width * 0.5
                                color: "green"
                            }

                            Item {
                                Layout.leftMargin: marginBetweenImages
                                Layout.column: 5
                                id: itemTicketBookmark
                                width: imageSize
                                height: imageSize
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
                                Layout.leftMargin: marginBetweenImages
                                Layout.column: 7
                                id: itemTicketShare
                                width: imageSize
                                height: imageSize
                                visible: false

                                Image {
                                    id: imageItemTicketShare
                                    anchors.fill : parent
                                    source: "../images/share.png";
                                }
                            }
                            Text { Layout.fillWidth: true }
                        }
                    }
                }
            }
        }
    }

    function showGamesInfo(gamePlayItem) {
        if (gamePlayItem !== null) {

            clearAllInfos();

            var bIsOnlyMeeting = false;
            if (gamePlayItem.competitionValue() === 7)
                bIsOnlyMeeting = true;

            m_gamePlayItem = gamePlayItem
            labelLineDate.text = gamePlayItem.timestampReadableLine();
            labelLineCompetition.text = gamePlayItem.getShortCompetition();
            labelLineGameInfo.text = gamePlayItem.getCompetitionShortRound();
            labelLineHome.text = gamePlayItem.home;
            labelLineAway.text = gamePlayItem.away;
            labelLineScore.text = gamePlayItem.score;
            if (gamePlayItem.home === "KSC")
                labelLineHome.font.letterSpacing = 2
            if (gamePlayItem.away === "KSC")
                labelLineAway.font.letterSpacing = 2

            if (gamePlayItem.isGameInPast()) {
                gradColorStart = "#505050"
                gradColorStop = "#909090"
                //                layoutCurrentInfoItem.visible = false;
                return;
            } else if (gamePlayItem.isGameRunning()) {
                gradColorStart = "#f30707"
                gradColorStop = "#ff4747"
            } else {
                var comp = gamePlayItem.competitionValue();
                if (comp === 4) { // dfb pokal
                    gradColorStart = "#105010"
                    gradColorStop = "#509050"
                }else if (comp === 5) { // badischer pokal
                    gradColorStart = "#103050"
                    gradColorStop = "#507090"
                } else if (comp === 6) { // TestSpiel
                    gradColorStart = "#101050"
                    gradColorStop = "#905090"
                } else if (comp === 7) {   // Meeting
                    gradColorStart = "#FF5050"
                    gradColorStop = "#FF7040"
                }
            }
            if (gamePlayItem.isGameInPast())
                return;

            var favIndex = gDataGameUserData.getFavoriteGameIndex(gamePlayItem.index);
            if (favIndex <= 0) {
                itemCompetitionInfo.border.color = "grey";
                itemCompetitionInfo.border.width = 2;
            }
            else{
                itemCompetitionInfo.border.color = "orange";
                itemCompetitionInfo.border.width = 3;
            }

            var eventCount = gamePlayItem.event;
            if (eventCount > 0) {
                eventIndicator.disableVisibility = false;
                eventIndicator.eventCount = eventCount;
            } else
                eventIndicator.disableVisibility = true;

            if (!gamePlayItem.timeFixed) {
                //                labelLineDate.text = gamePlayItem.timestampReadableLineWithoutTime();
                labelLineDate.font.italic = true;
                itemImageTimeLaps.visible = true;
            }

            if (!bIsOnlyMeeting)
                showTripInfo(gamePlayItem);

            showMeetingInfo(gamePlayItem);

            if (!bIsOnlyMeeting)
                showTicketInfo(gamePlayItem);
        }
    }

    function clearAllInfos() {
        itemImageTimeLaps.visible = false;

        acceptedTripItem.visible = false;
        labelAcceptedTrip.text = ""
        interestTripItem.visible = false;
        labelInterestTrip.text = ""
        imageTripIsPresent.visible = false;

        acceptedMeetingItem.visible = false;
        labelAcceptedMeeting.text = "";
        interestMeetingItem.visible = false;
        labelInterestMeeting.text = "";
        imageMeetingIsPresent.visible = false;

        imageTicketItems.visible = false;
        itemTicketShare.visible = false;
        itemTicketBookmark.visible = false;
        itemFreeTickets.visible = false;
        labelFreeTickets.text = ""
    }

    function showTripInfo(gamePlayItem) {

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
        }

        var freeTickets = gamePlayItem.getFreeTickets();
        if (freeTickets > 0) {
            itemFreeTickets.visible = true;
            labelFreeTickets.text = freeTickets;
        }

        if (freeTickets > 0 || ticketIndex > 1)
            imageTicketItems.visible = true;
        //        else
        //            itemFreeTickets.visible = false;
    }
}
