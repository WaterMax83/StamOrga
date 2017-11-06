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

Item {
    id: itemCurrentGame
    property UserInterface userIntCurrentGame
    property var m_gamePlayCurrentItem

    Pane {
        id: mainPaneCurrentGame
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

            MyComponents.Games {
                id: gameHeader
                anchors.left: parent.left
                anchors.right: parent.right
                Layout.alignment: Qt.AlignTop
            }

            MyComponents.BusyLoadingIndicator {
                id: busyIndicatorCurrentGame
                width: parent.width
                Layout.alignment: Qt.AlignHCenter | Qt.AlignTop
                Layout.fillHeight: false
                Layout.topMargin: 5
                infoVisible: true
            }

            TabBar {
                  id: tabBar
                  currentIndex: swipeViewCurrentHomeGame.currentIndex
                  anchors.left: parent.left
                  anchors.right: parent.right
                  Layout.alignment: Qt.AlignHCenter | Qt.AlignTop

                  Repeater {
                      model: tabModel
                      delegate: TabButton {
                          font.family: txtForFontFamily.font
                          text: model.text
                      }
                  }
              }

            SwipeView {
                id: swipeViewCurrentHomeGame
                anchors.top : tabBar.bottom
                anchors.bottom: parent.bottom
                anchors.left: parent.left
                anchors.right: parent.right
                Layout.alignment: Qt.AlignHCenter | Qt.AlignTop
                Layout.fillHeight: true
                currentIndex: tabBar.currentIndex
            }
        }
    }

    ListModel {
        id: tabModel
    }

    CurrentTicketInfo {
        id: currentTicketInfo
        onDragStarted: {
            movedInfoIndex = 2; movedStartY = contentY;
            movedStartMargin = gameHeader.Layout.topMargin === 0 ? 0 : -movedInfoHeigth;
        }
        onDragEnded: { movedInfoIndex = 0; checkMovedInfoEnd(movedStartY - contentY); }
        onContentYChanged : checkMovedInfo(2, movedStartY - contentY);
    }



    function currentTicketInfoNewHeaderInfo(text, load) {
        if (swipeViewCurrentHomeGame.currentItem === currentTicketInfo) {
            busyIndicatorCurrentGame.loadingVisible = load;
            if (text === "")
                busyIndicatorCurrentGame.infoVisible = false;
            else
                busyIndicatorCurrentGame.infoVisible = true;
            busyIndicatorCurrentGame.infoText = text;
        }
    }

    CurrentMeetInfo {
        id: currentMeetInfo
        onDragStarted: {
            movedInfoIndex = 2; movedStartY = contentY;
            movedStartMargin = gameHeader.Layout.topMargin === 0 ? 0 : -movedInfoHeigth;
        }
        onDragEnded: { movedInfoIndex = 0; checkMovedInfoEnd(movedStartY - contentY); }
        onContentYChanged : checkMovedInfo(2, movedStartY - contentY);
    }

    function currentMeetInfoNewHeaderInfo(text, load) {
        if (swipeViewCurrentHomeGame.currentItem === currentMeetInfo) {
            busyIndicatorCurrentGame.loadingVisible = load
            if (text === "")
                busyIndicatorCurrentGame.infoVisible = false;
            else
                busyIndicatorCurrentGame.infoVisible = true;
            busyIndicatorCurrentGame.infoText = text;
        }
    }

    NumberAnimation {
        id: animateMoveInfoUp
        target: gameHeader
        property: "Layout.topMargin"
        to: -movedInfoHeigth
        duration: 250
    }
    NumberAnimation {
        id: animateMoveInfoDown
        target: gameHeader
        property: "Layout.topMargin"
        to: 0
        duration: 250
    }

    function checkMovedInfo(index, diff) {
        if (movedInfoIndex !== index)
            return;

        if (movedStartMargin === 0) {
            if (diff < 0)
                gameHeader.Layout.topMargin = Math.max(-movedInfoHeigth, (diff * 0.5));
        } else if (movedStartMargin === -movedInfoHeigth) {

            if (diff > 0)
                gameHeader.Layout.topMargin = Math.min(0, -movedInfoHeigth + (diff * 0.5));
        }
    }

    function checkMovedInfoEnd(diff){
        if (movedStartMargin === 0) {
            if (diff < -10)
                animateMoveInfoUp.start()
            else
                animateMoveInfoDown.start()
        } else if (movedStartMargin === -movedInfoHeigth){
            if (diff > 10)
                animateMoveInfoDown.start()
            else
                animateMoveInfoUp.start()
        }
    }

    property int movedInfoIndex : 0
    property int movedStartY : 0
    property int movedStartMargin : 0
    property int movedInfoHeigth : gameHeader.height + 20

    function toolButtonClicked() {

    }

    function showAllInfoAboutGame(sender) {

        m_gamePlayCurrentItem = sender
        gameHeader.showGamesInfo(sender)

        if (sender.isGameASeasonTicketGame()) {
            tabModel.append({ "text": "Karten"});
            swipeViewCurrentHomeGame.addItem(currentTicketInfo)
            currentTicketInfo.showInfoHeader.connect(currentTicketInfoNewHeaderInfo);
            currentTicketInfo.showAllInfoAboutGame(sender);
        }

        tabModel.append({ "text": "Treffen"});
        swipeViewCurrentHomeGame.addItem(currentMeetInfo)

        currentMeetInfo.showInfoHeader.connect(currentMeetInfoNewHeaderInfo);
        currentMeetInfo.showAllInfoAboutGame();
    }

    function pageOpenedUpdateView() {}

    function notifyUserIntSeasonTicketListFinished(result) {
        currentTicketInfo.notifyUserIntSeasonTicketListFinished(result);
    }

    function notifyAvailableTicketStateChangedFinished(result) {
        currentTicketInfo.notifyAvailableTicketStateChangedFinished(result);
    }

    function notifyAvailableTicketListFinished(result) {
        currentTicketInfo.notifyAvailableTicketListFinished(result);
        if (result === 1)
            gameHeader.showGamesInfo(m_gamePlayCurrentItem)
    }

    function notifyChangedMeetingInfoFinished(result) {
        currentMeetInfo.notifyChangedMeetingInfoFinished(result);
    }

    function notifyLoadMeetingInfoFinished(result) {
        currentMeetInfo.notifyLoadMeetingInfoFinished(result);
        if (result === 1)
            gameHeader.showGamesInfo(m_gamePlayCurrentItem)
    }

    function notifyAcceptMeetingFinished(result) {
        currentMeetInfo.notifyAcceptMeetingFinished(result);
    }

    function notifyUserIntConnectionFinished(result) {}

}
