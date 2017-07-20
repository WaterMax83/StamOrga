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
    rebound: Transition {
            NumberAnimation {
                properties: "y"
                duration: 1000
                easing.type: Easing.OutBounce
            }
        }

    onDragEnded: {
        if (flickableGames.contentY < -100) {
            showLoadingGameInfos()
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

            width: parent.width
            spacing: 10
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
                    Layout.alignment:  Qt.AlignHCenter | Qt.AlignVCenter
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

    property var changeGameDialog;

    function toolButtonClicked() {
        if (globalUserData.userIsGameAddingEnabled() || userIntGames.isDebuggingEnabled()) {
            var component = Qt.createComponent("../components/ChangeGameDialog.qml");
            if (component.status === Component.Ready) {
                var dialog = component.createObject(flickableGames,{popupType: 1});
                dialog.headerText = "Neues Spiel";
                dialog.parentHeight = flickableGames.height
                dialog.parentWidth = flickableGames.width
                dialog.homeTeam = "";
                dialog.awayTeam = "";
                dialog.score = "";
                dialog.seasonIndex = 1;
                dialog.competitionIndex = 2;
                dialog.date = "";
                dialog.index = 0;
                dialog.acceptedDialog.connect(acceptedChangeGameDialog);
                changeGameDialog = dialog
                dialog.open();
            }
        }
    }

    function acceptedChangeGameDialog() {
        var result = userIntGames.startChangeGame(changeGameDialog.index, changeGameDialog.seasonIndex,
                                                  changeGameDialog.competition, changeGameDialog.homeTeam.trim(),
                                                  changeGameDialog.awayTeam.trim(), changeGameDialog.date.trim(),
                                                  changeGameDialog.score.trim());
        if (result !== 1) {
            toastManager.show(userIntGames.getErrorCodeToString(result), 5000)
            busyLoadingIndicatorGames.visible = true
            if (changeGameDialog.index === 0)
                txtInfoLoadingGames.text = "Füge Spiel hinzu"
            else
                txtInfoLoadingGames.text = "Ändere Spiel"
        }
    }


    function pageOpenedUpdateView() {

        if (globalUserData.userIsGameAddingEnabled() || userIntGames.isDebuggingEnabled())
            updateHeaderFromMain("StamOrga", "images/add.png")
        else
            updateHeaderFromMain("StamOrga", "")
        showListedGames()
    }

    function notifyUserIntGamesListFinished(result) {        
        if (result !== 1) {
            toastManager.show(userIntGames.getErrorCodeToString(result), 5000)
            busyLoadingIndicatorGames.visible = false
            showListedGames()
        }
    }

    function notifyUserIntGamesInfoListFinished(result) {
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
            txtInfoLoadingGames.text = "Keine Daten gespeichert\nZiehen zum Aktualisieren"
    }

    function notifyGameChangedFinished(result) {
        if (result === 1) {
            showLoadingGameInfos()
            userIntGames.startListGettingGames()
            for (var j = columnLayoutGames.children.length; j > 0; j--) {
                columnLayoutGames.children[j - 1].destroy()
            }
        }
        else
            toastManager.show(userIntGames.getErrorCodeToString(result), 5000)
    }

    function notifyUserIntConnectionFinished(result) {}

    function showLoadingGameInfos()
    {
        busyLoadingIndicatorGames.visible = true
        txtInfoLoadingGames.text = "Lade Spielinfos"
    }

    Component {
        id: gameView

        MyComponents.Games {
            onClickedCurrentGame: {
                var component = Qt.createComponent("../pages/CurrentGamePage.qml")
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
                    console.error("Fehler beim laden von der Spielseite " + component.errorString())
            }
            onPressedAndHoldCurrentGame: {
                if (globalUserData.userIsGameAddingEnabled() || userIntGames.isDebuggingEnabled()) {
                    var component = Qt.createComponent("../components/ChangeGameDialog.qml");
                    if (component.status === Component.Ready) {
                        var dialog = component.createObject(flickableGames,{popupType: 1});
                        dialog.headerText = "Spiel ändern";
                        dialog.parentHeight = flickableGames.height
                        dialog.parentWidth = flickableGames.width
                        dialog.homeTeam = sender.home;
                        dialog.awayTeam = sender.away;
                        dialog.score = sender.score;
                        dialog.seasonIndex = sender.seasonIndex;
                        dialog.competitionIndex = sender.competitionValue() - 1;
                        dialog.date = sender.timestamp;
                        dialog.index = sender.index;
                        dialog.acceptedDialog.connect(acceptedChangeGameDialog);
                        changeGameDialog = dialog
                        dialog.open();
                    }
                }
            }
        }
    }

    ScrollIndicator.vertical: ScrollIndicator {
    }
}
