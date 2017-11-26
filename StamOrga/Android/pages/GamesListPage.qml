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
//import QtGraphicalEffects 1.0

import com.watermax.demo 1.0

import "../components" as MyComponents

Flickable {
    id: flickableGames
    clip: true
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
            showInfoHeader("Lade Spielinfos")
            userIntGames.startListGettingGames()
            cleanGameLayout();
        }
    }

    signal showInfoHeader(var text);
    signal acceptedChangeGame(var dialog);
    property bool showOnlyPastGames : false

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

    function showListedGames() {

        cleanGameLayout();

        if (globalUserData.getGamePlayLength() > 0) {
            if (showOnlyPastGames) {
                for (var i = globalUserData.getGamePlayLength() - 1; i >= 0; i--) {
                    var gameInfo = globalUserData.getGamePlayFromArrayIndex(i);
                    if (gameInfo.isGameInPast()) {
                        var sprite = gameView.createObject(columnLayoutGames)
                        sprite.showGamesInfo(gameInfo)
                    }
                }
            } else {
                for (var j = 0; j < globalUserData.getGamePlayLength(); j++) {
                    var gameInfo2 = globalUserData.getGamePlayFromArrayIndex(j);
                    if (!gameInfo2.isGameInPast()) {
                        var sprite2 = gameView.createObject(columnLayoutGames)
                        sprite2.showGamesInfo(gameInfo2)
                    }
                }
            }
        }
    }

    function cleanGameLayout()
    {
        for (var j = columnLayoutGames.children.length; j > 0; j--) {
            columnLayoutGames.children[j - 1].destroy()
        }
    }

    Component {
        id: gameView

        MyComponents.Games {
            onClickedCurrentGame: {
                var component = Qt.createComponent("../pages/CurrentGamePage.qml")
                if (component.status === Component.Ready) {
                    var sprite = stackView.push(component)
                    sprite.userIntCurrentGame = userIntGames
                    sprite.showAllInfoAboutGame(sender)
                    if (sender.isGameAHomeGame())
                        updateHeaderFromMain("Heimspiel", "")
                    else
                        updateHeaderFromMain("Auswärts", "")

                    if (sender.event > 0)
                        appUserEvents.clearUserEventGamPlay(userIntGames, sender.index);

                } else
                    console.error("Fehler beim laden von der Spielseite " + component.errorString())
            }
            onPressedAndHoldCurrentGame: {
                menuSender = sender;
                var openCounter = 0;
                menuItemEditGame.visible = false;
                menuItemFixedGameTime.visible = false;
                menuItemNotFixedGameTime.visible = false;
                if (globalUserData.userIsGameAddingEnabled() || userIntGames.isDebuggingEnabled()) {
                    menuItemEditGame.visible = true;
                    openCounter++;
                }

                if (globalUserData.userIsGameFixedTimeEnabled() && !sender.isGameInPast()) {
                    if(sender.timeFixed)
                        menuItemNotFixedGameTime.visible = true;
                    else
                        menuItemFixedGameTime.visible = true;
                    openCounter++;
                }

                if (openCounter > 0) {
                    var globalCoordinates = mapToItem(mainItemGamesMainPage, 0, 0)
                    pressAndHoldCurrentGameMenu.y = globalCoordinates.y - height / 2
                    pressAndHoldCurrentGameMenu.open();
                }
            }
        }
    }

    Text {
        id: txtForFontFamily
        visible: false
    }

    Menu {
        id: pressAndHoldCurrentGameMenu
        x: (mainItemGamesMainPage.width - width) / 2
        y: mainItemGamesMainPage.height / 6

        background: Rectangle {
                implicitWidth: menuItemFixedGameTime.width
                color: "#4f4f4f"
            }

        MenuItem {
            id: menuItemEditGame
            font.family: txtForFontFamily.font
            height: visible ? implicitHeight : 0
            text: "Spiel editieren"
            onClicked: {
                var component = Qt.createComponent("../components/ChangeGameDialog.qml");
                if (component.status === Component.Ready) {
                    var dialog = component.createObject(mainItemGamesMainPage,{popupType: 1});
                    dialog.headerText = "Spiel ändern";
                    dialog.parentHeight = mainItemGamesMainPage.height
                    dialog.parentWidth = mainItemGamesMainPage.width
                    dialog.homeTeam = menuSender.home;
                    dialog.awayTeam = menuSender.away;
                    dialog.score = menuSender.score;
                    dialog.seasonIndex = menuSender.seasonIndex;
                    dialog.competitionIndex = menuSender.competitionValue() - 1;
                    dialog.date = menuSender.timestamp;
                    dialog.index = menuSender.index;
                    dialog.font.family= txtForFontFamily.font
                    dialog.acceptedDialog.connect(acceptedChangeGameDialog);
                    changeGameDialog = dialog
                    dialog.open();
                }
            }
        }

        MenuItem {
            id: menuItemFixedGameTime
            font.family: txtForFontFamily.font
            height: visible ? implicitHeight : 0
            text: "Spiel fest terminieren"
            onClicked: userIntGames.startSetFixedGameTime(menuSender.index, 1)
        }

        MenuItem {
            id: menuItemNotFixedGameTime
            font.family: txtForFontFamily.font
            height: visible ? implicitHeight : 0
            text: "Spiel nicht fest terminieren"
            onClicked: userIntGames.startSetFixedGameTime(menuSender.index, 0)
        }
    }

    property var changeGameDialog;
    property var menuSender;

    function acceptedChangeGameDialog()
    {
        acceptedChangeGame(changeGameDialog);
    }

    ScrollIndicator.vertical: ScrollIndicator {
    }
}
