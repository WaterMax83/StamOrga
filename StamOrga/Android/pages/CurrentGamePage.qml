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

            MyComponents.Games {
                id: gameHeader
                anchors.left: parent.left
                anchors.right: parent.right
                Layout.alignment: Qt.AlignTop
//                Layout.leftMargin: 10
//                Layout.rightMargin: 10
//                Layout.topMargin: 10
            }

            ColumnLayout {
                id: columnLayoutBusyInfoCurrGame
                spacing: 0
                width: parent.width
                Layout.alignment: Qt.AlignHCenter | Qt.AlignTop

                BusyIndicator {
                    id: busyLoadingIndicatorCurrentGames
                    visible: false
                    Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                }

                Label {
                    id: txtInfoCurrentGame
                    visible: false
                    Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                }
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
    }



    function currentTicketInfoNewHeaderInfo(text, load) {
        if (swipeViewCurrentHomeGame.currentItem === currentTicketInfo) {
            busyLoadingIndicatorCurrentGames.visible = load;
            if (text === "")
                txtInfoCurrentGame.visible = false
            else
                txtInfoCurrentGame.visible = true;
            txtInfoCurrentGame.text = text;
        }
    }

    CurrentMeetInfo {
        id: currentMeetInfo
    }

    function currentMeetInfoNewHeaderInfo(text, load) {
        if (swipeViewCurrentHomeGame.currentItem === currentMeetInfo) {
            busyLoadingIndicatorCurrentGames.visible = load;
            if (text === "")
                txtInfoCurrentGame.visible = false
            else
                txtInfoCurrentGame.visible = true;
            txtInfoCurrentGame.text = text;
        }
    }

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