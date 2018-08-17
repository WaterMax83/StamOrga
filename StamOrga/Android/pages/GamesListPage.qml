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
import "../controls" as MyControls

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
        if (flickableGames.contentY < -refreshItem.refreshHeight) {
            showInfoHeader("Lade Spielinfos")
            gDataGamesManager.startListGames();
            cleanGameLayout();
        }
    }

    signal showInfoHeader(var text);
    signal acceptedChangeGame(var dialog);
    property bool showOnlyPastGames : false

    MyComponents.RefreshItem {
        id: refreshItem
        contentY: flickableGames.contentY
    }

    Pane {
        id: mainPaneGames
        width: parent.width
        padding: 0


        ColumnLayout {
            id: mainColumnLayoutGames

            width: parent.width
            spacing: 10

            MyComponents.BusyLoadingIndicator {
                id: busyIndicatorGames
                width: parent.width
                Layout.alignment: Qt.AlignHCenter | Qt.AlignTop
                Layout.fillHeight: false
                Layout.topMargin: 10
                infoVisible: true
            }

            Column {
                id: columnLayoutGames
                anchors.right: parent.right
                anchors.left: parent.left
                width: parent.width
                spacing: 5
                Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
            }

            MyControls.Button {
                id: reloadGamesButton
                text: "weitere Laden"
                visible: bShowLoadMoreGames
                font.family: txtForFontFamily.font
                implicitWidth: parent.width / 3 * 2
                Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                onClicked: {
                    gDataGamesManager.startListGames(columnLayoutGames.children.length + 5);
                    bLoadMorePastGames = true;
                    bShowLoadMoreGames = false;
                }
            }

            MyComponents.BusyLoadingIndicator {
                width: parent.width
                Layout.alignment: Qt.AlignHCenter | Qt.AlignTop
                Layout.fillHeight: false
                infoVisible: false
                loadingVisible: true
                visible: bLoadMorePastGames
            }
        }
    }
    property bool bLoadMorePastGames: false
    property bool bShowLoadMoreGames: false

    function showListedGames() {

        cleanGameLayout();
        bLoadMorePastGames = false;
        bShowLoadMoreGames = false;

        if (gDataGamesManager.getGamePlayLength() > 0) {
            if (showOnlyPastGames) {
                for (var i = 0; i < gDataGamesManager.getGamePlayLength(); i++) {
                    var gameInfo = gDataGamesManager.getGamePlayFromArrayIndex(i);
                    if (gameInfo.isGameInPast()) {
                        var sprite = gameView.createObject(columnLayoutGames)
                        sprite.showGamesInfo(gameInfo)
                    }
                }
                bShowLoadMoreGames = gDataGamesManager.getSkipedOldGames();
            } else {
                for (var j = gDataGamesManager.getGamePlayLength() - 1; j >= 0; j--) {
                    var gameInfo2 = gDataGamesManager.getGamePlayFromArrayIndex(j);
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
        MyComponents.GamesDesignItem{
            onClickedCurrentGame: {
                var component = Qt.createComponent("../pages/CurrentGamePage.qml")
                if (component.status === Component.Ready) {
                    var sprite = stackView.push(component)
                    sprite.showAllInfoAboutGame(sender)

                    if (sender.event > 0)
                        gDataAppUserEvents.clearUserEventGamePlay(sender.index);

                } else
                    console.error("Fehler beim Laden von der Spielseite " + component.errorString())
            }
            onPressedAndHoldCurrentGame: {
                menuSender = sender;
                var openCounter = 0;
                menuItemEditGame.visible = false;
                menuItemFixedGameTime.visible = false;
                menuItemNotFixedGameTime.visible = false;
                if (gConUserSettings.userIsGameAddingEnabled() || userIntGames.isDebuggingEnabled()) {
                    menuItemEditGame.visible = true;
                    openCounter++;
                }

                if ((gConUserSettings.userIsGameFixedTimeEnabled() || userIntGames.isDebuggingEnabled()) && !sender.isGameInPast()) {
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
                    dialog.y = 0;
                    dialog.headerText = "Spiel ändern";
                    dialog.parentHeight = mainItemGamesMainPage.height
                    dialog.parentWidth = mainItemGamesMainPage.width
                    dialog.homeTeam = menuSender.home;
                    dialog.awayTeam = menuSender.away;
                    dialog.score = menuSender.score;
                    dialog.competitionIndex = menuSender.competitionValue() - 1;
                    dialog.seasonIndex = menuSender.seasonIndex;
                    dialog.date = menuSender.timestamp;
                    dialog.index = menuSender.index;
                    dialog.fixedTime = menuSender.timeFixed;
                    dialog.onlyFanclub = menuSender.onlyFanclub;
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
            onClicked: gDataGamesManager.startChangeGame(menuSender.index, menuSender.seasonIndex,
                                                         menuSender.competition, menuSender.home,
                                                         menuSender.away, menuSender.timestamp,
                                                         menuSender.score, true, menuSender.onlyFanclub);
        }

        MenuItem {
            id: menuItemNotFixedGameTime
            font.family: txtForFontFamily.font
            height: visible ? implicitHeight : 0
            text: "Spiel nicht fest terminieren"
            onClicked: gDataGamesManager.startChangeGame(menuSender.index, menuSender.seasonIndex,
                                                         menuSender.competition, menuSender.home,
                                                         menuSender.away, menuSender.timestamp,
                                                         menuSender.score, false, menuSender.onlyFanclub);
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

    function showLoadingGameInfosInternal(text, loading)
    {
        bLoadMorePastGames = false;
        busyIndicatorGames.loadingVisible = loading
        if (text !== "")
            busyIndicatorGames.infoText = text
    }
}
