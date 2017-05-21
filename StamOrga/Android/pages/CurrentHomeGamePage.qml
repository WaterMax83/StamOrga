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



        ColumnLayout {
            id: mainColumnLayoutCurrentGame
            width: parent.width
            MyComponents.Games {
                id: gameHeader
                width: parent.width
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

            ToolSeparator {
                id: toolSeparator1
                orientation: "Horizontal"
                implicitWidth: mainPaneCurrentGame.width / 3 * 1
                Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
            }

            CurrentTicketInfo {
                id: currentTicketInfo
                width: parent.width
                implicitHeight: itemCurrentGame.height
            }
        }
    }



    function acceptedEditReserveNameDialog(text)
    {
        currentTicketInfo.acceptedEditReserveNameDialog(text);
    }

    function showAllInfoAboutGame(sender) {

        m_gamePlayCurrentItem = sender
        gameHeader.showGamesInfo(sender)

        currentTicketInfo.showAllInfoAboutGame(sender);
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
    }

    function notifyUserIntConnectionFinished(result) {}

}
