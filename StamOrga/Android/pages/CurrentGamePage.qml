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

import com.watermax.demo 1.0

import "../components" as MyComponents
import "../controls" as MyControls

Item {
    id: mainPaneCurrentGame
    property var m_gamePlayCurrentItem
    width: parent.width
    height: parent.height

    ColumnLayout {
        id: mainColumnLayoutCurrentGame
        anchors.fill: parent
        Layout.alignment: Qt.AlignHCenter | Qt.AlignTop
        spacing: 0


        Text {
            id: txtForFontFamily
            visible: false
        }

        RowLayout {
            id: gameHeaderLayout
            width: parent.width
            Layout.topMargin: gameTopMargin
            Layout.alignment: Qt.AlignTop
            spacing: 0

            MyComponents.GamesDesignItem{
                id: gameHeader
                Layout.fillWidth: true
                Layout.topMargin: gameTopMargin
                Layout.alignment: Qt.AlignTop
                showGameSeperator: false
                onPressedAndHoldCurrentGame: Qt.openUrlExternally("https://www.youtube.com/watch?v=4DNGMoMNLRY")
            }

            Rectangle {
                width: 25
                height: 25
                anchors.top: parent.top
                anchors.right: parent.right
                anchors.margins: 5
                color : "transparent"
                visible: gConUserSettings.userIsGameAddingEnabled()
                Image {
                    id: addImage
                    width: parent.width / 3 * 2
                    height: parent.width / 3 * 2
                    source: "../images/back.png"
                    rotation: -90
                    anchors.centerIn: parent
                }
                MouseArea {
                    anchors.fill: parent
                    onClicked: {
                        if (m_gamePlayCurrentItem.hasGameMeetingEvent())
                            menuItemAddMeeting.visible = false
                        else
                            menuItemAddMeeting.visible = true

                        if (m_gamePlayCurrentItem.hasGameAwayTripEvent())
                            menuItemAddAwayTrip.visible = false
                        else
                            menuItemAddAwayTrip.visible = true

                        menuAddNewEvent.open()
                    }
                }
            }
        }

        MyComponents.BusyLoadingIndicator {
            id: busyIndicatorCurrentGame
            width: parent.width
            Layout.alignment: Qt.AlignHCenter | Qt.AlignTop
            Layout.fillHeight: false
            Layout.topMargin: infoVisible ? 5 : 0
            infoVisible: true
        }

        MyControls.TabBar {
            id: tabBar
            currentIndex: swipeViewCurrentHomeGame.currentIndex
            Layout.fillWidth: true
            Layout.alignment: Qt.AlignHCenter | Qt.AlignTop

            Repeater {
                model: tabModel
                delegate: TabButton {
                    font.family: txtForFontFamily.font
                    text: model.text
                }
            }
            onCurrentIndexChanged: {
                var bar = tabModel.get(tabBar.currentIndex);
                if (bar === undefined)
                    bShowComment = false;
                else
                    bShowComment = bar.useCommentLine
            }
        }

        SwipeView {
            id: swipeViewCurrentHomeGame
            anchors.top: tabBar.bottom
            anchors.bottom: bShowComment ? rowComment.top : parent.bottom
            anchors.left: parent.left
            anchors.right: parent.right
            Layout.alignment: Qt.AlignHCenter | Qt.AlignTop
            Layout.fillHeight: true
            currentIndex: tabBar.currentIndex
        }

        RowLayout {
            id: rowComment
            width: parent.width
            anchors.bottom: parent.bottom
            anchors.bottomMargin: Qt.inputMethod.visible ? 25: 5
            Layout.fillWidth: true
            Layout.margins: 5
            visible: bShowComment

            MyComponents.EditableTextWithHint {
                id: textInputConsole
                Layout.fillWidth: true
                Layout.topMargin: 5
                hint: "Kommentar hinzufügen"
                imageSource: ""
                enableImage: false
                enableKeyEnterSignal: true
                enabled: true
                color: "#FFFFFF"
                onKeysEnterPressed: {
                    if (textInputConsole.input === "")
                        return;

                    if (swipeViewCurrentHomeGame.currentItem === currentMeetInfo) {
                        currentMeetInfo.sendNewComment(textInputConsole.input)
                    } else {
                        currentAwayTripInfo.sendNewComment(textInputConsole.input)
                    }
                }
            }

            MyComponents.GraphicalButton {
                id: graphicalButtonComment
                imageSource: "../images/send.png"
                Layout.topMargin: 5
                enabled: true
                onClickedButton: {
                    Qt.inputMethod.commit()
                    if (textInputConsole.input === "")
                        return;

                    if (swipeViewCurrentHomeGame.currentItem === currentMeetInfo) {
                        currentMeetInfo.sendNewComment(textInputConsole.input);
                    } else {
                        currentAwayTripInfo.sendNewComment(textInputConsole.input);
                    }
                    enabled = false;
                }
                Layout.alignment: Qt.AlignRight
            }
        }
    }

    ListModel {
        id: tabModel
    }

    Menu {
        id: menuAddNewEvent
        x: (mainPaneCurrentGame.width - width) / 2
        y: mainPaneCurrentGame.height / 6

        background: Rectangle {
            implicitWidth: menuItemAddMeeting.width
            color: "#4f4f4f"
        }

        MenuItem {
            id: menuItemAddMeeting
            font.family: txtForFontFamily.font
            height: visible ? implicitHeight : 0
            text: "Treffen hinzufügen"
            onClicked: {
                bAddedNewEvent = true;
                gDataMeetingInfo.startSaveMeetingInfo(m_gamePlayCurrentItem.index, "", "", "", 0);
            }
        }
        MenuItem {
            id: menuItemAddAwayTrip
            font.family: txtForFontFamily.font
            height: visible ? implicitHeight : 0
            text: "Fahrt hinzufügen"
            onClicked: {
                bAddedNewEvent = true;
                gDataTripInfo.startSaveMeetingInfo(m_gamePlayCurrentItem.index, "", "", "", 1);
            }
        }
    }

    CurrentTicketInfo {
        id: currentTicketInfo
        onDragStarted: {
            movedInfoIndex = 2
            movedStartY = contentY
            movedStartMargin = gameHeaderLayout.Layout.topMargin
                    === gameTopMargin ? gameTopMargin : -movedInfoHeigth
        }
        onDragEnded: {
            movedInfoIndex = 0
            checkMovedInfoEnd(movedStartY - contentY)
        }
        onContentYChanged: {
            checkMovedInfo(2, movedStartY - contentY)
        }
    }

    function currentTicketInfoNewHeaderInfo(text, load) {
        if (swipeViewCurrentHomeGame.currentItem === currentTicketInfo) {
            busyIndicatorCurrentGame.loadingVisible = load
            if (text === "")
                busyIndicatorCurrentGame.infoVisible = false
            else
                busyIndicatorCurrentGame.infoVisible = true
            busyIndicatorCurrentGame.infoText = text
        }
    }

    CurrentMeetInfo {
        id: currentMeetInfo
        onDragStarted: {
            movedInfoIndex = 2
            movedStartY = contentY
            movedStartMargin = gameHeaderLayout.Layout.topMargin
                    === gameTopMargin ? gameTopMargin : -movedInfoHeigth
        }
        onDragEnded: {
            movedInfoIndex = 0
            checkMovedInfoEnd(movedStartY - contentY)
        }
        onContentYChanged: checkMovedInfo(2, movedStartY - contentY)
    }

    CurrentMeetInfo {
        id: currentAwayTripInfo
        onDragStarted: {
            movedInfoIndex = 2
            movedStartY = contentY
            movedStartMargin = gameHeaderLayout.Layout.topMargin
                    === gameTopMargin ? gameTopMargin : -movedInfoHeigth
        }
        onDragEnded: {
            movedInfoIndex = 0
            checkMovedInfoEnd(movedStartY - contentY)
        }
        onContentYChanged: checkMovedInfo(2, movedStartY - contentY)
    }

    function currentMeetInfoNewHeaderInfo(text, load) {
        if (swipeViewCurrentHomeGame.currentItem === currentMeetInfo) {
            busyIndicatorCurrentGame.loadingVisible = load
            if (text === "")
                busyIndicatorCurrentGame.infoVisible = false
            else
                busyIndicatorCurrentGame.infoVisible = true
            busyIndicatorCurrentGame.infoText = text
        }
    }

    function currentAwayTriptInfoNewHeaderInfo(text, load) {
        if (swipeViewCurrentHomeGame.currentItem === currentAwayTripInfo) {
            busyIndicatorCurrentGame.loadingVisible = load
            if (text === "")
                busyIndicatorCurrentGame.infoVisible = false
            else
                busyIndicatorCurrentGame.infoVisible = true
            busyIndicatorCurrentGame.infoText = text
        }
    }

    CurrentMediaInfo {
        id: currentMediaInfo
        onDragStarted: {
            movedInfoIndex = 2
            movedStartY = contentY
            movedStartMargin = gameHeaderLayout.Layout.topMargin
                    === gameTopMargin ? gameTopMargin : -movedInfoHeigth
        }
        onDragEnded: {
            movedInfoIndex = 0
            checkMovedInfoEnd(movedStartY - contentY)
        }
        onContentYChanged: checkMovedInfo(2, movedStartY - contentY)
    }

    function currentMediaInfoNewHeaderInfo(text, load) {
        if (swipeViewCurrentHomeGame.currentItem === currentMediaInfo) {
            busyIndicatorCurrentGame.loadingVisible = load
            if (text === "")
                busyIndicatorCurrentGame.infoVisible = false
            else
                busyIndicatorCurrentGame.infoVisible = true
            busyIndicatorCurrentGame.infoText = text
        }
    }

    NumberAnimation {
        id: animateMoveInfoUp
        target: gameHeaderLayout
        property: "Layout.topMargin"
        to: -movedInfoHeigth
        duration: 250
    }
    NumberAnimation {
        id: animateMoveInfoDown
        target: gameHeaderLayout
        property: "Layout.topMargin"
        to: gameTopMargin
        duration: 250
    }

    function checkMovedInfo(index, diff) {
        if (movedInfoIndex !== index)
            return

        if (movedStartMargin === gameTopMargin) {
            if (diff < 0)
                gameHeaderLayout.Layout.topMargin = Math.max(-movedInfoHeigth,
                                                       (diff * 0.5))
        } else if (movedStartMargin === -movedInfoHeigth) {

            if (diff > 0)
                gameHeaderLayout.Layout.topMargin = Math.min(
                            gameTopMargin, -movedInfoHeigth + (diff * 0.5))
        }
    }

    function checkMovedInfoEnd(diff) {
        if (movedStartMargin === gameTopMargin) {
            if (diff < -10)
                animateMoveInfoUp.start()
            else
                animateMoveInfoDown.start()
        } else if (movedStartMargin === -movedInfoHeigth) {
            if (diff > 10)
                animateMoveInfoDown.start()
            else
                animateMoveInfoUp.start()
        }
    }

    property int gameTopMargin: 5
    property int movedInfoIndex: 0
    property int movedStartY: 0
    property int movedStartMargin: 0
    property int movedInfoHeigth: gameHeaderLayout.height + 20
    property bool bShowComment: false
    property bool bAddedNewEvent : false

    function toolButtonClicked() {
        var favIndex = gDataGameUserData.getFavoriteGameIndex(
                    m_gamePlayCurrentItem.index)

        if (favIndex <= 0) {
            gDataGameUserData.setFavoriteGameIndex(m_gamePlayCurrentItem.index,
                                                   1)
            updateHeaderFromMain("", "images/star.png")
            toastManager.show("Spiel als Favorit markiert", 2000)
        } else {
            gDataGameUserData.setFavoriteGameIndex(m_gamePlayCurrentItem.index,
                                                   0)
            updateHeaderFromMain("", "images/star_border.png")
            toastManager.show("Spiel als Favorit entfernt", 2000)
        }
        gameHeader.showGamesInfo(m_gamePlayCurrentItem)
    }

    function getPreventEscape() {
        if (swipeViewCurrentHomeGame.currentItem === currentMediaInfo){
            return currentMediaInfo.getPreventEscape();
        }

        return false;
    }

    function showAllInfoAboutGame(sender) {

        m_gamePlayCurrentItem = sender
        gameHeader.showGamesInfo(sender)

        gDataGamesManager.startGetGamesEvents(m_gamePlayCurrentItem.index);

        busyIndicatorCurrentGame.infoVisible = true
        busyIndicatorCurrentGame.loadingVisible = true
        busyIndicatorCurrentGame.infoText = "Lade Daten"

        var icon = ""
        var favIndex = gDataGameUserData.getFavoriteGameIndex(sender.index)
        if (favIndex <= 0)
            icon = "images/star_border.png"
        else
            icon = "images/star.png"

        if (sender.isGameAHomeGame())
            updateHeaderFromMain("Heimspiel", icon)
        else {
            if (sender.isGameAAwayGame())
                updateHeaderFromMain("Auswärts", icon)
            else
                updateHeaderFromMain("Treffen", icon)
        }
    }

    function pageOpenedUpdateView() {
        gDataMeetingInfo.resetMeetingInfo();
        gDataTripInfo.resetMeetingInfo();
    }

    function notifyGetUserProperties(result) {
        gameHeader.showGamesInfo(m_gamePlayCurrentItem)
    }

    function notifyGetGameEventsFinished(result) {
        if (result !== 1) {
            toastManager.show(userIntGames.getErrorCodeToString(result), 4000);
            busyIndicatorCurrentGame.infoVisible = true
            busyIndicatorCurrentGame.loadingVisible = false
            busyIndicatorCurrentGame.infoText = "Konnte keine Infos über dieses Spiel laden"
            return;
        }


        var isAnythingOpen = false;
        if (m_gamePlayCurrentItem.hasGameTicketEvent()) {
            tabModel.append({text: "Karten", useCommentLine: false  })
            swipeViewCurrentHomeGame.addItem(currentTicketInfo)
            currentTicketInfo.showInfoHeader.connect(currentTicketInfoNewHeaderInfo)
            currentTicketInfo.showAllInfoAboutGame(m_gamePlayCurrentItem)
            isAnythingOpen = true;
        }

        if (m_gamePlayCurrentItem.hasGameMeetingEvent()) {
            tabModel.append({text: "Treffen", useCommentLine: true })
            swipeViewCurrentHomeGame.addItem(currentMeetInfo)

            currentMeetInfo.meetingType = 0
            currentMeetInfo.showInfoHeader.connect(currentMeetInfoNewHeaderInfo)
            currentMeetInfo.showAllInfoAboutGame()
            isAnythingOpen = true;
        }

        if (m_gamePlayCurrentItem.hasGameAwayTripEvent()) {
            tabModel.append({text: "Fahrt", useCommentLine: true })
            swipeViewCurrentHomeGame.addItem(currentAwayTripInfo)

            currentAwayTripInfo.meetingType = 1
            currentAwayTripInfo.showInfoHeader.connect(currentAwayTriptInfoNewHeaderInfo)
            currentAwayTripInfo.showAllInfoAboutGame()
            isAnythingOpen = true;
        }

//        if (m_gamePlayCurrentItem.hasGameMediaEvent()) {
//            tabModel.append({text: "Medien", useCommentLine: false })
//            swipeViewCurrentHomeGame.addItem(currentMediaInfo)
//            currentMediaInfo.showInfoHeader.connect(currentMediaInfoNewHeaderInfo)
//            currentMediaInfo.showAllInfoAboutGame(m_gamePlayCurrentItem)
//            isAnythingOpen = true;
//        }

        if (!isAnythingOpen) {
            busyIndicatorCurrentGame.infoVisible = true
            busyIndicatorCurrentGame.loadingVisible = false
            busyIndicatorCurrentGame.infoText = "Keine Elemente vorhanden"
        }
    }

    /* ********************** TICKETS *************************** */

    function notifyUserIntSeasonTicketListFinished(result) {
        currentTicketInfo.notifyUserIntSeasonTicketListFinished(result)
    }

    function notifyAvailableTicketStateChangedFinished(result) {
        currentTicketInfo.notifyAvailableTicketStateChangedFinished(result)
    }

    function notifyAvailableTicketListFinished(result) {
        currentTicketInfo.notifyAvailableTicketListFinished(result)
        if (result === 1)
            gameHeader.showGamesInfo(m_gamePlayCurrentItem)
    }


    /* ************************ MEETING ***************************/
    function notifyLoadMeetingInfoFinished(result) {
        currentMeetInfo.notifyLoadMeetingInfoFinished(result)
        if (result === 1)
            gameHeader.showGamesInfo(m_gamePlayCurrentItem)
    }

    function notifyChangedMeetingInfoFinished(result) {
        if (bAddedNewEvent) {
            bAddedNewEvent = false;
            gDataGamesManager.startGetGamesEvents(m_gamePlayCurrentItem.index);
            return;
        }

        currentMeetInfo.notifyChangedMeetingInfoFinished(result)
    }

    function notifyAcceptMeetingFinished(result) {
        currentMeetInfo.notifyAcceptMeetingFinished(result)
    }

    function notifySendCommentMeetFinished(result) {
        if (result === 1) {
            Qt.inputMethod.hide();
            textInputConsole.clear()
        }
        graphicalButtonComment.enabled = true;
        currentMeetInfo.notifySendCommentMeetFinished(result)
    }


    /* ************************ AwayTrip ***************************/
    function notifyLoadAwayTripInfoFinished(result) {
        currentAwayTripInfo.notifyLoadMeetingInfoFinished(result)
        if (result === 1)
            gameHeader.showGamesInfo(m_gamePlayCurrentItem)
    }

    function notifyChangedAwayTripInfoFinished(result) {
        if (bAddedNewEvent) {
            bAddedNewEvent = false;
            gDataGamesManager.startGetGamesEvents(m_gamePlayCurrentItem.index);
            return;
        }

        currentAwayTripInfo.notifyChangedMeetingInfoFinished(result)
    }

    function notifyAcceptAwayTripFinished(result) {
        currentAwayTripInfo.notifyAcceptMeetingFinished(result)
    }

    function notifySendCommentTripFinished(result) {
        if (result === 1) {
            Qt.inputMethod.hide();
            textInputConsole.clear()
        }
        graphicalButtonComment.enabled = true;
        currentAwayTripInfo.notifySendCommentMeetFinished(result)
    }

    /************************ Media ************************************/
    function notifyMediaCommandFinished(result){
        currentMediaInfo.notifyMediaCommandFinished(result);
    }

    function notifyUserIntConnectionFinished(result) {}
}
