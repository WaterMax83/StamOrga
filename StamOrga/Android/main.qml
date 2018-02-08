
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
                    fillMode: Image.PreserveAspectFit
                    horizontalAlignment: Image.AlignHCenter
                    verticalAlignment: Image.AlignVCenter
//                    source: "images/refresh.png"
                    //                    visible: stackView.depth > 1 ? false : true
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

                delegate: ItemDelegate {
                    width: parent.width
                    text: model.title
                    highlighted: ListView.isCurrentItem
                    onClicked: {
                        if (model.element) {
                            titleLabel.text = model.title
                            if (model.imgsource) {
                                imageToolButton.visible = true
                                imageToolButton.source = model.imgsource
                            } else {
                                imageToolButton.visible = false
                            }
                            stackView.push(model.element)
                        } else if (model.link) {
                            Qt.openUrlExternally(model.link);
                            if (model.title === "Update")
                                appUserEvents.clearUserEventUpdate(userInt);
                        }

                        drawer.close()
                    }
                    MyComponents.EventIndicator {
                        disableVisibility: model.event ? false : true
                        eventCount : model.event ? model.event : 0
                    }
                }

                model: ListModel {
                    id: listViewListModel
                    Component.onCompleted: {
                        append({
                                   title: "Benutzerprofil",
                                   element: viewUserLogin,
                                   imgsource: "images/menu.png"
                               })
                        append({
                                   title: "Dauerkarten",
                                   element: viewSeasonTickets,
                                   imgsource: "images/add.png"
                               })
                        if (userInt.isDebuggingEnabled())
                            append({
                                       title: "Fanclub",
                                       element: viewFanclubNewList,
                                       event: 0
                                   })
                        append({
                                   title: "Einstellungen",
                                   element: viewSettingsPage,
                               })
                        if (userInt.isDebuggingEnabled())
                            append({
                                       title: "Logging",
                                       element: viewLoggingPage,
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

    /* Need components because otherwise they will be shown in main view */
    Component {
        id: viewUserLogin
        MyPages.UserLogin {
            userIntUser: userInt
        }
    }
    Component {
        id: viewSeasonTickets
        MyPages.SeasonTickets {
            userIntTicket: userInt
        }
    }
    Component {
        id: viewSettingsPage
        MyPages.Settings {}
    }
    Component{
        id: viewLoggingPage
        MyPages.LogginPage {}
    }
    Component {
        id: viewFanclubNewList
        MyPages.FanclubNewsList{}
    }

    UserInterface {
        id: userInt
        globalData: globalUserData
        onNotifyConnectionFinished: {
            stackView.currentItem.notifyUserIntConnectionFinished(result)
        }
        onNotifyVersionRequestFinished: {
            //            stackView.currentItem.notifyUserIntVersionRequestFinished(result, msg);
            if (result === 5) {
                if (globalSettings.useVersionPopup) {
                    var component = Qt.createComponent("/components/VersionDialog.qml")
                    if (component.status === Component.Ready) {
                        var dialog = component.createObject(stackView, {
                                                                popupType: 1
                                                            })
                        dialog.versionText = msg
                        dialog.parentHeight = stackView.height
                        dialog.parentWidth = stackView.width
                        dialog.open()
                    }
                }

                if (!isNewVersionElementShown) {
                    isNewVersionElementShown = true;
                    listViewListModel.append({
                                            title: "Update",
                                            link: globalUserData.getUpdateLink(),
                                            event : 0
                                        })
                }
            }
        }
        property bool isLoggingWindowShown : false;
        property bool isFanclubNewsWindowShown : false;
        property bool isNewVersionElementShown : false;
        onNotifyUserPropertiesFinished: {
            if (result > 0 && !userInt.isDebuggingEnabled()) {
                if (!isLoggingWindowShown) {
                    isLoggingWindowShown = true;
                    if (globalUserData.userIsDebugEnabled()) {
                        listViewListModel.append({
                                                title: "Logging",
                                                element: viewLoggingPage,
                                                imgsource: ""
                                            })
                    }
                }
                if (!isFanclubNewsWindowShown) {
                    isFanclubNewsWindowShown = true;
                    if (globalUserData.userIsFanclubEnabled()) {
                        listViewListModel.insert(2, {
                                                title: "Fanclub",
                                                element: viewFanclubNewList,
                                                imgsource: "",
                                                event : 0
                                            })
                    }
                }
            }
            if (stackView.currentItem === viewMainGames) {
                if (globalUserData.userIsGameAddingEnabled() || userInt.isDebuggingEnabled())
                    updateHeaderFromMain("StamOrga", "images/add.png")
                else
                    updateHeaderFromMain("StamOrga", "")
            }
        }
        onNotifyGetUserEvents: {
            iMainToolButtonEventCount = appUserEvents.getCurrentMainEventCounter();

            for(var i=0; i< listViewListModel.count; i++) {
                if (listViewListModel.get(i).title === "Update")
                    listViewListModel.get(i).event = appUserEvents.getCurrentUpdateEventCounter();
                else if (listViewListModel.get(i).title === "Fanclub") {
                    listViewListModel.get(i).event = appUserEvents.getCurrentFanclubEventCounter();
                }
            }

            if (stackView.currentItem.notifyGetUserEvents)
                stackView.currentItem.notifyGetUserEvents(result);
        }

        onNotifyUpdatePasswordRequestFinished: {
            stackView.currentItem.notifyUserIntUpdatePasswordFinished(result)
        }
        onNotifyUpdateReadableNameRequest: {
            stackView.currentItem.notifyUserIntUpdateReadableNameFinished(result)
        }
        onNotifyGamesListFinished: viewMainGames.notifyUserIntGamesListFinished(result)
        onNotifyGamesInfoListFinished: viewMainGames.notifyUserIntGamesInfoListFinished(result);
        onNotifySetGamesFixedTimeFinished: viewMainGames.notifySetGamesFixedTimeFinished(result);
        onNotifyChangedGameFinished: stackView.currentItem.notifyGameChangedFinished(result);

        onNotifySeasonTicketAddFinished: stackView.currentItem.notifyUserIntSeasonTicketAdd(result)
        onNotifySeasonTicketListFinished: stackView.currentItem.notifyUserIntSeasonTicketListFinished(result)
        onNotifySeasonTicketRemoveFinished: stackView.currentItem.notifyUserIntSeasonTicketRemoveFinished(result)
        onNotifySeasonTicketEditFinished: stackView.currentItem.notifyUserIntSeasonTicketEditFinished(result)

        onNotifyAvailableTicketStateChangedFinished: stackView.currentItem.notifyAvailableTicketStateChangedFinished(result);
        onNotifyAvailableTicketListFinsished: stackView.currentItem.notifyAvailableTicketListFinished(result);

        onNotifyChangedMeetingInfoFinished: stackView.currentItem.notifyChangedMeetingInfoFinished(result);
        onNotifyLoadMeetingInfoFinished: stackView.currentItem.notifyLoadMeetingInfoFinished(result);
        onNotifyAcceptMeetingFinished: stackView.currentItem.notifyAcceptMeetingFinished(result);
        onNotifyChangedAwayTripInfoFinished: stackView.currentItem.notifyChangedAwayTripInfoFinished(result);
        onNotifyLoadAwayTripInfoFinished: stackView.currentItem.notifyLoadAwayTripInfoFinished(result);
        onNotifyAcceptAwayTripFinished: stackView.currentItem.notifyAcceptAwayTripFinished(result);

        onNotifyChangeNewsDataFinished: stackView.currentItem.notifyChangeNewsDataFinished(result);
        onNotifyFanclubNewsListFinished: stackView.currentItem.notifyFanclubNewsListFinished(result);
        onNotifyGetFanclubNewsItemFinished: stackView.currentItem.notifyGetFanclubNewsItemFinished(result);
        onNotifyDeleteFanclubNewsItemFinished: stackView.currentItem.notifyDeleteFanclubNewsItemFinished(result);
    }

    Connections {
       target: globalSettings
       onSendAppStateChangedToActive: {
           viewMainGames.showLoadingGameInfos("Lade Spielinfos", true)
           if (value === 1)
               userInt.startListGettingGamesInfo();
           else if (value === 2)
               userInt.startListGettingGames();
       }
    }

    function openUserLogin(open) {

        if (open === true) {
            stackView.push(viewUserLogin)
            imageToolButton.visible = false
        } else {
            stackView.currentItem.showListedGames()
            globalSettings.checkNewStateChangedAtStart();

            if (!globalSettings.isVersionChangeAlreadyShown()) {
                var component = Qt.createComponent("../pages/newVersionInfo.qml");
                if (component.status === Component.Ready) {
                    stackView.push(component);
                }
            }
        }
    }
}
