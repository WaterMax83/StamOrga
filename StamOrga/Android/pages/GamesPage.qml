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
//import QtGraphicalEffects 1.0

import com.watermax.demo 1.0

import "../components" as MyComponents

Flickable {
    id: flickableGames
    property UserInterface userIntGames
    contentHeight: mainPaneGames.height

    onDragEnded: {
        if (flickableGames.contentY < -100) {
            busyLoadingIndicatorGames.visible = true
            txtInfoLoadingGames.text = "Lade Spielliste"
            userIntGames.startListGettingGames()
            for (var j = columnLayoutGames.children.length; j > 0; j--) {
                columnLayoutGames.children[j - 1].destroy()
            }
        }
    }

    Rectangle {
        Image {
            id: refreshImage
            source: "../images/refresh.png"
            rotation: (flickableGames.contentY > -100) ? (flickableGames.contentY * -1) * 2 : 220
            transformOrigin: Item.Center
        }
        opacity: (flickableGames.contentY * -1) / 100
        color: "black"
        width: refreshImage.width
        height: refreshImage.height
        radius: width * 0.5
        y: 50
        x: (mainWindow.width / 2) - (width / 2)
        z: 1000
    }

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
                    horizontalAlignment: Text.AlignHCenter
                    Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                    bottomPadding: 10
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
//        busyLoadingIndicatorGames.visible = true
//        txtInfoLoadingGames.text = "Lade Spielliste"
//        userIntGames.startListGettingGames()
    }

    function pageOpenedUpdateView() {
        showListedGames()
    }

    function notifyUserIntGamesListFinished(result) {
        busyLoadingIndicatorGames.visible = false
        if (result === 1)
            toastManager.show("Spiele geladen", 2000)
        else
            toastManager.show(userIntGames.getErrorCodeToString(result), 5000)

        showListedGames()
    }

    function showListedGames() {

        for (var j = columnLayoutGames.children.length; j > 0; j--) {
            columnLayoutGames.children[j - 1].destroy()
        }

        if (globalUserData.getGamePlayLength() > 0) {
            for (var i = 0; i < globalUserData.getGamePlayLength(); i++) {
                var sprite = gameView.createObject(columnLayoutGames)
                sprite.showGamesInfo(globalUserData.getGamePlayFromArrayIndex(i))
            }
            txtInfoLoadingGames.text = "Letztes Update am " + globalUserData.getGamePlayLastUpdate()
        } else
            txtInfoLoadingGames.text = "Keine Daten zum Anzeigen\nZiehen zum Aktualisieren"
    }

    function notifyUserIntConnectionFinished(result) {}

    Component {
        id: gameView

        MyComponents.Games {
            onClickedCurrentGame: {
                var component
                if (sender.isGameAHomeGame())
                    component = Qt.createComponent("../pages/CurrentHomeGamePage.qml")
                else
                    component = Qt.createComponent("../pages/CurrentAwayGamePage.qml")
                if (component.status === Component.Ready) {
                    var sprite = stackView.push(component)
                    //                   userIntGames.startListSeasonTickets();
                    sprite.userIntCurrentGame = userIntGames
                    sprite.showAllInfoAboutGame(sender)
                    if (sender.isGameAHomeGame())
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
