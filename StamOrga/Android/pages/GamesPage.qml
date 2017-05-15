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
    id: flickableGames
    property UserInterface userIntGames
    contentHeight: mainPaneGames.height
    Pane {
        id: mainPaneGames
        width: parent.width

        ColumnLayout {
            id: mainColumnLayoutGames
            //           anchors.right: parent.right
            //           anchors.left: parent.left
            width: parent.width
            spacing: 0
            ColumnLayout {
                id: columnLayoutBusyInfo
                spacing: 5
                Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter

                BusyIndicator {
                    id: busyLoadingIndicatorGames
                    visible: false
                    Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                }

                Label {
                    id: txtInfoLoadingGames
                    visible: true
                    Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                }
            }
            Column {
                id: columnLayoutGames
                anchors.right: parent.right
                anchors.left: parent.left
                width: parent.width
                spacing: 10
                Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
            }
        }
    }

    function toolButtonClicked() {
        busyLoadingIndicatorGames.visible = true
        txtInfoLoadingGames.text = "Lade Spielliste"
        userIntGames.startListGettingGames()
    }

    function pageOpenedUpdateView() {
        showListedGames()
    }

    function notifyUserIntGamesListFinished(result) {
        busyLoadingIndicatorGames.visible = false
        if (result === 1) {
            showListedGames()
        } else {
            txtInfoLoadingGames.text = userIntGames.getErrorCodeToString(result)
        }
    }

    function showListedGames() {

        for (var j = columnLayoutGames.children.length; j > 0; j--) {
            columnLayoutGames.children[j - 1].destroy()
        }

        if (globalUserData.getGamePlayLength() > 0) {
            for (var i = 0; i < globalUserData.getGamePlayLength(); i++) {
                var sprite = gameView.createObject(columnLayoutGames)
                sprite.showGamesInfo(globalUserData.getGamePlay(i))
            }
            txtInfoLoadingGames.text = "Letzes Update am " + globalUserData.getGamePlayLastUpdate()
        } else
            txtInfoLoadingGames.text = "Keine Daten zum anzeigen"
    }

    function notifyUserIntConnectionFinished(result) {}

    Component {
        id: gameView

        MyComponents.Games {
            onClickedCurrentGame: {
                var component
                if (sender.home === "KSC")
                    component = Qt.createComponent("../pages/CurrentHomeGamePage.qml")
                else
                    component = Qt.createComponent("../pages/CurrentAwayGamePage.qml")
                if (component.status === Component.Ready) {
                    var sprite = stackView.push(component)
                    //                   userIntGames.startListSeasonTickets();
                    sprite.userIntCurrentGame = userIntGames
                    sprite.showAllInfoAboutGame(sender)
                    if (sender.home === "KSC")
                        updateHeaderFromMain("Heimspiel", "")
                    else
                        updateHeaderFromMain("Auswärts", "")
                } else
                    console.log("Fehler beim laden " + component.errorString())
            }
        }
    }

    ScrollIndicator.vertical: ScrollIndicator {
    }
}
