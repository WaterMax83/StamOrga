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
            width: parent.width
            height: parent.height
            Layout.alignment: Qt.AlignTop
            spacing: 0

            MyComponents.Games {
                id: gameHeader
                width: parent.width
                Layout.alignment: Qt.AlignTop
//                Layout.leftMargin: 10
//                Layout.rightMargin: 10
//                Layout.topMargin: 10
            }

            ColumnLayout {
                id: columnLayoutBusyInfoCurrGame
                spacing: 0
                Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter

                BusyIndicator {
                    id: busyLoadingIndicatorCurrentGames
                    visible: false
                    Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                }

                Label {
                    id: txtInfoCurrentGame
                    visible: true
                    Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                }
            }

            TabBar {
                  id: tabBar
                  currentIndex: swipeViewCurrentHomeGame.currentIndex
                  width: parent.width
                  height: tabButton1.height - 10

                  TabButton {
                      id: tabButton1
                      text: "Karten"
                  }
                  TabButton {
                      text: "Treffen"
                  }
              }

            SwipeView {
                id: swipeViewCurrentHomeGame
                anchors.top : tabBar.bottom
                anchors.bottom: parent.bottom
                width: parent.width
                Layout.alignment: Qt.AlignTop
                currentIndex: tabBar.currentIndex
                onCurrentItemChanged: {
                    if (currentItem == currentMeetInfo) {
                        if (currentMeetInfo.isInputAlreadyChanged)
                            updateHeaderFromMain("", "images/save.png");
                        else
                            updateHeaderFromMain("", "images/edit.png");
                    }
                    else
                        updateHeaderFromMain("", "")
                }
            }
        }
    }

//    PageIndicator {
//        count: swipeViewCurrentHomeGame.count
//        currentIndex: swipeViewCurrentHomeGame.currentIndex
//        anchors.bottom: parent.bottom
//        anchors.horizontalCenter: parent.horizontalCenter
//    }

    CurrentTicketInfo {
        id: currentTicketInfo
    }

    CurrentMeetInfo {
        id: currentMeetInfo
    }

    function toolButtonClicked() {

    }


    function acceptedEditReserveNameDialog(text)
    {
        currentTicketInfo.acceptedEditReserveNameDialog(text);
    }

    function showAllInfoAboutGame(sender) {

        m_gamePlayCurrentItem = sender
        gameHeader.showGamesInfo(sender)

        if (sender.isGameASeasonTicketGame()) {
            swipeViewCurrentHomeGame.addItem(currentTicketInfo)
            currentTicketInfo.showAllInfoAboutGame(sender);
        }

        swipeViewCurrentHomeGame.addItem(currentMeetInfo)

//        swipeViewCurrentHomeGame.currentIndex = swipeViewCurrentHomeGame.count - 1
        currentMeetInfo.showAllInfoAboutGame(sender);
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

    function notifyUserIntConnectionFinished(result) {}

}
