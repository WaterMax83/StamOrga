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

import "../pages" as MyPages
import "../components" as MyComponents
import "../controls" as MyControls

Page {
    id: mainItemGamesMainPage
    width: parent.width
    height: parent.height
    property UserInterface userIntGames

    GridLayout {
        id: mainColumnGamesMainPage
        anchors.fill: parent
        Layout.alignment: Qt.AlignHCenter | Qt.AlignTop
        flow: GridLayout.TopToBottom
        rows: 3
        columns: 2

        Text {
            id: txtForFontFamily
            visible: false
        }

//        MyComponents.BusyLoadingIndicator {
//            id: busyIndicatorGames
//            width: parent.width
//            Layout.alignment: Qt.AlignHCenter | Qt.AlignTop
//            Layout.fillHeight: false
//            Layout.topMargin: 10
//            infoVisible: true
//            Layout.columnSpan: 2
//        }

        MyControls.TabBar {
                id: tabBarGamesMainPage
                anchors.left: parent.left
                anchors.right: parent.right
                Layout.alignment: Qt.AlignHCenter | Qt.AlignTop
                currentIndex: swipeViewGamesMainPage.currentIndex
                Layout.columnSpan: 2


                TabButton {
                    text: "Aktuell"
                    font.family: txtForFontFamily.font
                }
                TabButton {
                    text: "Vergangenheit"
                    font.family: txtForFontFamily.font
                }
        }

        SwipeView {
            id: swipeViewGamesMainPage
            anchors.top : tabBarGamesMainPage.bottom
            anchors.bottom: parent.bottom
            anchors.left: parent.left
            anchors.right: parent.right
            Layout.alignment: Qt.AlignHCenter | Qt.AlignTop
            Layout.fillHeight: true
            currentIndex: tabBarGamesMainPage.currentIndex
            Layout.columnSpan: 2

            MyPages.GamesListPage {
                id: gamesListPagePresent
                showOnlyPastGames : false
                onShowInfoHeader: showLoadingGameInfos(text, true);
                onAcceptedChangeGame: startEditGame(dialog);
//                onDragStarted: {
//                    movedInfoIndex = 1; movedStartY = contentY;
//                    movedStartMargin = busyIndicatorGames.Layout.topMargin > 0 ? 10 : -movedInfoHeigth;
//                }
//                onDragEnded: { movedInfoIndex = 0; checkMovedInfoEnd(movedStartY - contentY); }
//                onContentYChanged : checkMovedInfo(1, movedStartY - contentY);

            }
            MyPages.GamesListPage {
                id: gamesListPagePast
                showOnlyPastGames : true
                onShowInfoHeader: showLoadingGameInfos(text, true);
                onAcceptedChangeGame: startEditGame(dialog);
//                onDragStarted: {
//                    movedInfoIndex = 2; movedStartY = contentY;
//                    movedStartMargin = busyIndicatorGames.Layout.topMargin > 0 ? 10 : -movedInfoHeigth;
//                }
//                onDragEnded: { movedInfoIndex = 0; checkMovedInfoEnd(movedStartY - contentY); }
//                onContentYChanged : checkMovedInfo(2, movedStartY - contentY);
            }
        }

    }

//    NumberAnimation {
//        id: animateMoveInfoUp
//        target: busyIndicatorGames
//        property: "Layout.topMargin"
//        to: -movedInfoHeigth
//        duration: 250
//    }
//    NumberAnimation {
//        id: animateMoveInfoDown
//        target: busyIndicatorGames
//        property: "Layout.topMargin"
//        to: 10
//        duration: 250
//    }

//    function checkMovedInfo(index, diff) {
//        if (movedInfoIndex !== index)
//            return;

//        if (movedStartMargin === 10) {
//            if (diff < 0)
//                busyIndicatorGames.Layout.topMargin = Math.max(-movedInfoHeigth, (diff * 0.5) + 10);
//        } else if (movedStartMargin === -movedInfoHeigth) {

//            if (diff > 0)
//                busyIndicatorGames.Layout.topMargin = Math.min(10, -movedInfoHeigth + (diff * 0.5));
//        }
//    }

//    function checkMovedInfoEnd(diff){
//        if (movedStartMargin === 10) {
//            if (diff < -10)
//                animateMoveInfoUp.start()
//            else
//                animateMoveInfoDown.start()
//        } else if (movedStartMargin === -movedInfoHeigth){
//            if (diff > 10)
//                animateMoveInfoDown.start()
//            else
//                animateMoveInfoUp.start()
//        }
//    }

//    property int movedInfoIndex : 0
//    property int movedStartY : 0
//    property int movedStartMargin : 10
//    property int movedInfoHeigth : busyIndicatorGames.infoHeight + 5

    property var addGameDialog;

    function toolButtonClicked() {

        if (gConUserSettings.userIsGameAddingEnabled() || userIntGames.isDebuggingEnabled()) {
            menuAddNewGame.open();
        }
    }

    Menu {
        id: menuAddNewGame
        x: (mainItemGamesMainPage.width - width) / 2
        y: mainItemGamesMainPage.height / 6

        background: Rectangle {
            implicitWidth: menuItemAddGame.width
            color: "#4f4f4f"
        }

        MenuItem {
            id: menuItemAddGame
            font.family: txtForFontFamily.font
            height: visible ? implicitHeight : 0
            text: "Spiel hinzufügen"
            onClicked: {
                var component = Qt.createComponent("../components/ChangeGameDialog.qml");
                if (component.status === Component.Ready) {
                    var dialog = component.createObject(mainItemGamesMainPage,{popupType: 1});
                    dialog.y = 0;
                    dialog.headerText = text;
                    dialog.parentHeight = mainItemGamesMainPage.height
                    dialog.parentWidth = mainItemGamesMainPage.width
                    dialog.homeTeam = "";
                    dialog.awayTeam = "";
                    dialog.score = "";
                    dialog.seasonIndex = 1;
                    dialog.competitionIndex = 2;
                    dialog.date = "";
                    dialog.index = 0;
                    dialog.fixedTime = false;
                    dialog.onlyFanclub = false;
                    dialog.font.family= txtForFontFamily.font
                    dialog.acceptedDialog.connect(acceptedAddGameDialog);
                    addGameDialog = dialog
                    dialog.open();
                }
            }
        }

        MenuItem {
            id: menuItemAddEvent
            font.family: txtForFontFamily.font
            height: visible ? implicitHeight : 0
            text: "Event hinzufügen"
            onClicked: {
                var component = Qt.createComponent("../components/ChangeEventDialog.qml");
                if (component.status === Component.Ready) {
                    var dialog = component.createObject(mainItemGamesMainPage,{popupType: 1});
                    dialog.y = 0;
                    dialog.headerText = text;
                    dialog.parentHeight = mainItemGamesMainPage.height
                    dialog.parentWidth = mainItemGamesMainPage.width
                    dialog.homeTeam = "";
                    dialog.awayTeam = "";
                    dialog.score = "";
                    dialog.seasonIndex = 1;
                    dialog.competitionIndex = 2;
                    dialog.date = "";
                    dialog.index = 0;
                    dialog.fixedTime = false;
                    dialog.onlyFanclub = false;
                    dialog.font.family= txtForFontFamily.font
                    dialog.acceptedDialog.connect(acceptedAddGameDialog);
                    addGameDialog = dialog
                    dialog.open();
                }
            }
        }
    }

    function acceptedAddGameDialog(){
        startEditGame(addGameDialog)
    }

    function startEditGame(dialog) {
        var result = gDataGamesManager.startChangeGame(dialog.index, dialog.seasonIndex,
                                                  dialog.competition, dialog.homeTeam.trim(),
                                                  dialog.awayTeam.trim(), dialog.date.trim(),
                                                  dialog.score.trim(), dialog.fixedTime,
                                                  dialog.onlyFanclub);
        if (result !== 1) {
            toastManager.show(userIntGames.getErrorCodeToString(result), 5000)            
        } else {
            if (dialog.index === 0) {
                showLoadingGameInfos("Füge Spiel hinzu", true);
            }
            else {
                showLoadingGameInfos("Ändere Spiel", true);
            }
        }
    }

    function notifyUserIntConnectionFinished(result) {}

    function notifyUserIntGamesListFinished(result) {
        if (result !== 1) {
            toastManager.show(userIntGames.getErrorCodeToString(result), 5000)
            showLoadingGameInfos("", false);
            showListedGames()
        }
    }

    function notifyUserIntGamesInfoListFinished(result) {
        showLoadingGameInfos("", false);
        if (result === 1) {
            toastManager.show("Spiele geladen", 2000)
        }
        else{
            toastManager.show(userIntGames.getErrorCodeToString(result), 5000)
            /* If success then user properties will be called */
            showListedGames()
        }
    }

    function notifyGetUserProperties(result) {
            showListedGames();
    }

    function notifyGameChangedFinished(result) {
        if (result === 1) {
            showLoadingGameInfos("Lade Spielinfos", true)
            gDataGamesManager.startListGames();
            gamesListPagePresent.cleanGameLayout();
            gamesListPagePast.cleanGameLayout();
        }
        else
            toastManager.show(userIntGames.getErrorCodeToString(result), 5000)
    }

    function pageOpenedUpdateView() {

        if (gConUserSettings.userIsGameAddingEnabled() || userIntGames.isDebuggingEnabled())
            updateHeaderFromMain("StamOrga", "images/add.png")
        else
            updateHeaderFromMain("StamOrga", "")

        showListedGames()
    }

    function showListedGames() {
        gamesListPagePresent.showListedGames()
        gamesListPagePast.showListedGames()

        if (gDataGamesManager.getGamePlayLength() > 0) {
            showLoadingGameInfos("Letztes Update vor " + gDataGamesManager.getGamePlayLastLocalUpdateString(), false);
        } else {
            showLoadingGameInfos("Keine Daten gespeichert\nZiehen zum Aktualisieren", false);
        }
    }

    function showLoadingGameInfos(text, loading)
    {
        gamesListPagePresent.showLoadingGameInfosInternal(text, loading);
        gamesListPagePast.showLoadingGameInfosInternal(text, loading);
    }

}
