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

        ColumnLayout {
            id: mainColumnLayoutCurrentGame
            width: parent.width
                MyComponents.Games {
                    id: gameHeader
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

            Label {
                id: txtInfoCurrentGameBlockedTickets
                visible: true
                text: "Nicht verfügbar:"
                Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
            }

            ListView {
                id: listViewBlockedTickets

                focus: false
                implicitWidth: mainColumnLayoutCurrentGame.width
                implicitHeight: flickableCurrentGame.height
//                enabled: false

                delegate: ItemDelegate {
                    width: parent.width
                    text: model.title
                }

                model: ListModel {
                    id: listViewModelBlockedTickets
                }
            }

        }
    }

    function showAllInfoAboutGame(sender) {
        m_gamePlayCurrentItem = sender;
        gameHeader.showGamesInfo(sender)
    }

    function pageOpenedUpdateView() {

    }

    function notifyUserIntSeasonTicketListFinished(result) {
        if (result === 1) {
            listViewModelBlockedTickets.clear();

            if (globalUserData.getSeasonTicketLength() > 0) {
                for (var i=0; i<globalUserData.getSeasonTicketLength(); i++) {
                    listViewModelBlockedTickets.append( {title: globalUserData.getSeasonTicket(i).name })
                }
//                txtInfoSeasonTicket.text = "Letzes Update am " + globalUserData.getSeasonTicketLastUpdate()
            }
//            listViewModelBlockedTickets.append({title: "Benutzer1", src : ""});
//            listViewModelBlockedTickets.append({title: "Benutzer2", src : ""})
        } else {
            txtInfoCurrentGame.text = userIntCurrentGame.getErrorCodeToString(result);
        }
    }

    function notifyUserIntConnectionFinished(result) {}
}
