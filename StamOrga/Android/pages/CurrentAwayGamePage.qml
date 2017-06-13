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

Flickable {
    id: flickableCurrentGame
    property UserInterface userIntCurrentGame
    property var m_gamePlayCurrentItem

    contentHeight: mainPaneCurrentGame.height

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

            CurrentMeetInfo {
                id: currentMeetInfo
                width: parent.width
            }
        }
    }

    function currentMeetInfoNewHeaderInfo(text, load) {
        busyLoadingIndicatorCurrentGames.visible = load;
        if (text === "")
            txtInfoCurrentGame.visible = false
        else
            txtInfoCurrentGame.visible = true;
        txtInfoCurrentGame.text = text;
    }

    function showAllInfoAboutGame(sender) {

        m_gamePlayCurrentItem = sender
        gameHeader.showGamesInfo(sender)

        currentMeetInfo.showInfoHeader.connect(currentMeetInfoNewHeaderInfo);
        currentMeetInfo.showAllInfoAboutGame();
    }

    function pageOpenedUpdateView() {}

    function notifyUserIntSeasonTicketListFinished(result) {}

    function notifyAvailableTicketStateChangedFinished(result) {}

    function notifyAvailableTicketListFinished(result) {}

    function notifyChangedMeetingInfoFinished(result) {
        currentMeetInfo.notifyChangedMeetingInfoFinished(result);
    }

    function notifyLoadMeetingInfoFinished(result) {
        currentMeetInfo.notifyLoadMeetingInfoFinished(result);
    }

    function notifyAcceptMeetingFinished(result) {
        currentMeetInfo.notifyAcceptMeetingFinished(result);
    }

    function notifyUserIntConnectionFinished(result) {}
}
