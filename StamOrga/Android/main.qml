
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

import com.watermax.demo 1.0

import "pages" as MyPages
import "components" as MyComponents

ApplicationWindow {
    id: mainWindow
    visible: true
    width: userInt.isDeviceMobile() ? 540 : 360
    height: userInt.isDeviceMobile() ? 960 : 600
    title: qsTr("StamOrga")

    property int iMainToolButtonEventCount : 0

    onClosing: {
        if (userInt.isDeviceMobile() && stackView.depth > 1) {
            close.accepted = false
            stackView.pop()
        } else {
            return
        }
    }

    Shortcut {
        sequence: "Esc"
        enabled: stackView.depth > 1
        onActivated: {
            stackView.pop()
        }
    }

    header: ToolBar {
        height: 50

        RowLayout {
            spacing: 20
            anchors.fill: parent

            ToolButton {
                implicitHeight: 50
                contentItem: Image {
                    fillMode: Image.Pad
                    horizontalAlignment: Image.AlignHCenter
                    verticalAlignment: Image.AlignVCenter
                    source: stackView.depth > 1 ? "images/back.png" : "images/drawer.png"
                    MyComponents.EventIndicator {
                        disableVisibility: stackView.depth > 1 ? true : false
                        eventCount : iMainToolButtonEventCount
                    }
                }
                onClicked: {
                    if (stackView.depth > 1) {
                        stackView.pop()
                    } else {
                        drawer.open()
                    }
                }
            }

            Text {
                id: titleLabel
                color: "white"
                font.pixelSize: 25
                elide: Text.ElideRight
                horizontalAlignment: Qt.AlignHCenter
                verticalAlignment: Qt.AlignVCenter
                Layout.fillWidth: true
            }

            ToolButton {
                implicitHeight: 50
                contentItem: Image {
                    id: imageToolButton
                    //                    anchors.fill: parent
                    //                    anchors.margins: 5
                    fillMode: Image.Pad
                    horizontalAlignment: Image.AlignHCenter
                    verticalAlignment: Image.AlignVCenter
                }
                onClicked: {
                    if (imageToolButton.visible === true) {
                        stackView.currentItem.toolButtonClicked()
                    }
                }
            }
        }
    }

    Drawer {
        id: drawer
        width: Math.min(mainWindow.width, mainWindow.height) / 4 * 2
        height: mainWindow.height

        ColumnLayout {
            implicitWidth: drawer.width
            spacing: 1
            anchors.fill: parent

            Rectangle {
                implicitWidth: drawer.width
                implicitHeight: drawer.width

                color: "white"
                Image {
                    height: parent.height
                    width: parent.width
                    fillMode: Image.Stretch
                    horizontalAlignment: Image.AlignHCenter
                    verticalAlignment: Image.AlignVCenter
                    source: "images/StammtischLogo.png"
                }
            }

            ListView {
                id: listView

                focus: true
                currentIndex: -1
                implicitWidth: drawer.width
                implicitHeight: drawer.height
                highlight: highlightBar

                delegate: listDelegate

                model: ListModel {
                    id: listViewListModel
                    Component.onCompleted: {
                        append({
                                   title: "Benutzerprofil",
                                   element: viewUserLogin,
                                   event: 0,
                                   toolButtonImgSource: "images/menu.png",
                                   listImgSource : "images/account.png"
                               })
                        append({
                                   title: "Dauerkarten",
                                   element: viewSeasonTickets,
                                   toolButtonImgSource: "images/add.png",
                                   listImgSource : "images/card.png"
                               })
                        append({
                                   title: "Statistik",
                                   element: viewStatistics,
                                   listImgSource : "images/chart.png"
                               })
                        if (userInt.isDebuggingEnabled())
                            append({
                                       title: "Fanclub",
                                       element: viewFanclubNewList,
                                       event: 0,
                                       listImgSource : "images/group.png"
                                   })
                        append({
                                   title: "Einstellungen",
                                   element: viewSettingsPage,
                                   listImgSource : "images/settings.png"
                               })
                        append({
                                   title: "Update",
                                   element: viewUpdatePage,
                                   listImgSource : "images/download.png"
                               })
                        if (userInt.isDebuggingEnabled())
                            append({
                                       title: "Logging",
                                       element: viewLoggingPage,
                                       listImgSource : "images/bug.png"
                                   })
                    }
                }

                ScrollIndicator.vertical: ScrollIndicator {}
            }
        }
    }

    function updateHeaderFromMain(text, img) {

        if (img !== "") {
            imageToolButton.visible = true
            imageToolButton.source = img
        } else {
            imageToolButton.visible = false
        }

        if (text !== "")
            titleLabel.text = text
    }

    StackView {
        id: stackView
        anchors.fill: parent

        initialItem: viewMainGames

        onCurrentItemChanged: {
            if (stackView.currentItem.pageOpenedUpdateView)
                stackView.currentItem.pageOpenedUpdateView()
        }
    }


    MyPages.GamesMainPage {
        id: viewMainGames
        userIntGames: userInt
    }

    MyComponents.ToastManager {
        id: toastManager
    }

    Component {
        id: highlightBar
        Rectangle {
            width: listView.width
            height: 50
            color: "#808080"
        }
    }

    Component {
        id: listDelegate
        Item {
            id: delegateItem
            width: listView.width
            height: 50

            RowLayout {
                anchors.fill: parent
                Image {
                    Layout.preferredHeight: parent.height / 1.5
                    Layout.preferredWidth: parent.height / 1.5
                    Layout.leftMargin: 10
                    horizontalAlignment: Qt.AlignHCenter
                    verticalAlignment: Qt.AlignVCenter
                    source: listImgSource
                }

                Text {
                    height: parent.height
                    text: title
                    color: "white"
                    font.pixelSize: 16
                    elide: Text.ElideRight
                    verticalAlignment: Qt.AlignVCenter
                    Layout.leftMargin: 10
                    Layout.fillWidth: true
                }
            }
            MyComponents.EventIndicator {
                disableVisibility: event ? false : true
                eventCount : event ? event : 0
            }
            MouseArea {
                anchors.fill: parent
                hoverEnabled: true
                onClicked: {
                    listView.currentIndex = index
                    if (model.element) {
                        titleLabel.text = title
                        if (toolButtonImgSource) {
                            imageToolButton.visible = true
                            imageToolButton.source = toolButtonImgSource
                        } else {
                            imageToolButton.visible = false
                        }
                        stackView.push(element)
                    }

                    drawer.close()
                }
            }
        }
    }

    /* Need components because otherwise they will be shown in main view */
    Component {
        id: viewUserLogin
        MyPages.UserLogin {
            userIntUser: userInt
        }
    }
    Component {
        id: viewSeasonTickets
        MyPages.SeasonTickets {}
    }
    Component {
        id: viewStatistics
        MyPages.Statistics {}
    }

    Component {
        id: viewSettingsPage
        MyPages.Settings {}
    }
    Component{
        id: viewLoggingPage
        MyPages.LogginPage {}
    }
    Component{
        id: viewUpdatePage
        MyPages.UpdatePage {}
    }
    Component {
        id: viewFanclubNewList
        MyPages.FanclubNewsList{}
    }
    Component {
        id: viewConsolePage
        MyPages.Console {}
    }

    UserInterface {
        id: userInt
        onNotifyConnectionFinished: {
            if (stackView.currentItem.notifyUserIntConnectionFinished)
                stackView.currentItem.notifyUserIntConnectionFinished(result, msg)
        }
        onNotifyVersionRequestFinished: {
            if (result === 5) {
                if (gStaGlobalSettings.getUseVersionPopup() && titleLabel.text !== "Update") {
                    var component = Qt.createComponent("/components/VersionDialog.qml")
                    if (component.status === Component.Ready) {
                        var dialog = component.createObject(stackView, {
                                                                popupType: 1
                                                            })
                        dialog.versionText = gStaVersionManager.getVersionInfo();
                        dialog.parentHeight = stackView.height
                        dialog.parentWidth = stackView.width
                        dialog.open()
                    }
                }
            }
            if (stackView.currentItem.notifyVersionRequestFinished)
                stackView.currentItem.notifyVersionRequestFinished(result);
        }
        property bool isLoggingWindowShown : false;
        property bool isFanclubNewsWindowShown : false;
        property bool isConsoleWindowShown : false;
        onNotifyUserPropertiesFinished: {
            if (result > 0 && !userInt.isDebuggingEnabled()) {
                if (!isLoggingWindowShown) {
                    isLoggingWindowShown = true;
                    if (gConUserSettings.userIsDebugEnabled()) {
                        listViewListModel.append({
                                                     title: "Logging",
                                                     element: viewLoggingPage,
                                                     toolButtonImgSource: "",
                                                     listImgSource : "images/bug.png"
                                                 })
                    }
                }
                if (!isFanclubNewsWindowShown) {
                    isFanclubNewsWindowShown = true;
                    if (gConUserSettings.userIsFanclubEnabled()) {
                        listViewListModel.insert(2, {
                                                     title: "Fanclub",
                                                     element: viewFanclubNewList,
                                                     toolButtonImgSource: "",
                                                     event : 0,
                                                     listImgSource : "images/group.png"
                                                 })
                    }
                }

            }
            if (result > 0 && !isConsoleWindowShown) {
                isConsoleWindowShown = true;
                if (gConUserSettings.userIsConsoleEnabled()) {
                    listViewListModel.append({
                                                 title: "Console",
                                                 element: viewConsolePage,
                                                 toolButtonImgSource: "",
                                                 listImgSource: ""
                                             });
                }
            }
            if (stackView.currentItem === viewMainGames) {
                if (gConUserSettings.userIsGameAddingEnabled() || userInt.isDebuggingEnabled())
                    updateHeaderFromMain("StamOrga", "images/add.png")
                else
                    updateHeaderFromMain("StamOrga", "")
            }

            /* User Events are also transported with UserProperties */
            iMainToolButtonEventCount = gDataAppUserEvents.getCurrentMainEventCounter();

            for(var i=0; i< listViewListModel.count; i++) {
                if (listViewListModel.get(i).title === "Update")
                    listViewListModel.get(i).event = gDataAppUserEvents.getCurrentUpdateEventCounter();
                else if (listViewListModel.get(i).title === "Fanclub") {
                    listViewListModel.get(i).event = gDataAppUserEvents.getCurrentFanclubEventCounter();
                }
            }

            if (stackView.currentItem.notifyGetUserProperties)
                stackView.currentItem.notifyGetUserProperties(result);
        }


        onNotifyUpdatePasswordRequestFinished: viewUserLogin.notifyUserIntUpdatePasswordFinished(result)
        onNotifyUpdateReadableNameRequest:  viewUserLogin.notifyUserIntUpdateReadableNameFinished(result)

        onNotifyGamesListFinished: viewMainGames.notifyUserIntGamesListFinished(result)
        onNotifyGamesInfoListFinished: viewMainGames.notifyUserIntGamesInfoListFinished(result);
        onNotifyChangedGameFinished: viewMainGames.notifyGameChangedFinished(result);
        onNotifyGetGameEventsFinished: stackView.currentItem.notifyGetGameEventsFinished(result);

        onNotifySeasonTicketAddFinished: viewSeasonTickets.notifyUserIntSeasonTicketAdd(result)
        onNotifySeasonTicketRemoveFinished: viewSeasonTickets.notifyUserIntSeasonTicketRemoveFinished(result)
        onNotifySeasonTicketEditFinished: viewSeasonTickets.notifyUserIntSeasonTicketEditFinished(result)
        onNotifySeasonTicketListFinished: if (stackView.currentItem.notifyUserIntSeasonTicketListFinished) stackView.currentItem.notifyUserIntSeasonTicketListFinished(result)

        onNotifyAvailableTicketStateChangedFinished: stackView.currentItem.notifyAvailableTicketStateChangedFinished(result);
        onNotifyAvailableTicketListFinsished: stackView.currentItem.notifyAvailableTicketListFinished(result);

        onNotifyChangedMeetingInfoFinished: stackView.currentItem.notifyChangedMeetingInfoFinished(result);
        onNotifyLoadMeetingInfoFinished: stackView.currentItem.notifyLoadMeetingInfoFinished(result);
        onNotifyAcceptMeetingFinished: stackView.currentItem.notifyAcceptMeetingFinished(result);
        onNotifyChangedAwayTripInfoFinished: stackView.currentItem.notifyChangedAwayTripInfoFinished(result);
        onNotifyLoadAwayTripInfoFinished: stackView.currentItem.notifyLoadAwayTripInfoFinished(result);
        onNotifyAcceptAwayTripFinished: stackView.currentItem.notifyAcceptAwayTripFinished(result);
        onNotifySendCommentMeetFinished: stackView.currentItem.notifySendCommentMeetFinished(result);
        onNotifySendCommentTripFinished: stackView.currentItem.notifySendCommentTripFinished(result);

        onNotifyChangeNewsDataFinished: viewFanclubNewList.notifyChangeNewsDataFinished(result);
        onNotifyFanclubNewsListFinished: viewFanclubNewList.notifyFanclubNewsListFinished(result);
        onNotifyGetFanclubNewsItemFinished: viewFanclubNewList.notifyGetFanclubNewsItemFinished(result);
        onNotifyDeleteFanclubNewsItemFinished: viewFanclubNewList.notifyDeleteFanclubNewsItemFinished(result);

        onNotifyStatisticsCommandFinished: viewStatistics.notifyStatisticsCommandFinished(result);
        onNotifyConsoleCommandFinished: viewConsolePage.notifyConsoleCommandFinished(result);
        onNotifyMediaCommandFinished: stackView.currentItem.notifyMediaCommandFinished(result);
    }

    Connections {
        target: gDataGamesManager
        onSendAppStateChangedToActive: {
            viewMainGames.showLoadingGameInfos("Lade Spielinfos", true)
            //           if (value === 1)
            //                gDataGamesManager.startListGamesInfo();
            //           else if (value === 2)
            //                gDataGamesManager.startListGames();
        }
    }

    function openUserLogin(open) {

        if (open === true) {
            stackView.push(viewUserLogin)
            imageToolButton.visible = false
        } else {
            //            stackView.currentItem.showListedGames()
            gStaGlobalSettings.checkNewStateChangedAtStart();

            if (!gStaVersionManager.isVersionChangeAlreadyShown()) {
                var component = Qt.createComponent("../pages/newVersionInfo.qml");
                if (component.status === Component.Ready) {
                    stackView.push(component);
                }
            }
        }
    }
}
